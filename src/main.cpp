#include <cinttypes>           // intmax_t
#include <cstdlib>             // strtoimax
#include <signal.h>            // sigprocmask
#include <unistd.h>            // STDIN_FILENO
#include <fcntl.h>             // O_RDONLY

#include "handleton.hpp"       // ilrd::Handleton
#include "logger.hpp"          // ilrd:Logger
#include "crow_all.h"          // crow::SimpleApp
#include "cli.hpp"             // Command Line Interface
#include "event_checkers.hpp"     // IEventTypeChecker and its implementaions
#include "event_handler.hpp"   // EventHandler

static const char *g_XGithubEvent = "X-GitHub-Event";
static const char *g_push = "push";
static const char *g_team = "team";
static const char *g_repository = "repository";

static const int g_baseTen = 10;
static const int g_addrArgIndex = 1;
static const int g_portArgIndex = 2;
static const int g_argsRequired = 3;


// A wrapper function for ilrd::Logger::Log(), for ease of use
inline void Log(const std::string& message, ilrd::Logger::Severity severity);

// Blocks all incoming signals, otherwise the server runs into problems.
static void BlockAllSignals();

// Initializes the server app
void InitApp(crow::SimpleApp& app, const char* bindAddr ,int port);

// Initializes the event handler
static void InitEventHandler(EventHandler *eventHandler);

// Sets the callback function upon reception of a POST request.
static void SetRouteCallback(crow::SimpleApp& app);

// Helper function that checks the  validity of an address.
static bool IsValidAddress(const char *address);

// Parses the command line arguments 
static void ParseArgs(int argc, const char **argv, 
    const char *&bindAddrOutParam, int &portOutParam);


// Called when there are not enough command line arguments.
static void ErrUsage();

// Called when the address argument is invalid.
static void ErrInvalidAddress();

// Called when the address argument is invalid.
static void ErrInvalidPort();

int main(int argc, const char **argv)
{
    int port;
    const char *bindAddress;
    ParseArgs(argc, argv, bindAddress, port);

    crow::SimpleApp app;
    CLI *cli = ilrd::Handleton<CLI>::getInstance();
    EventHandler *eventHandler = ilrd::Handleton<EventHandler>::getInstance();

    BlockAllSignals();
    InitApp(app, bindAddress, port);
    InitEventHandler(eventHandler);
    SetRouteCallback(app);

    std::future<void> future = app.run_async();
    
    cli->AcceptUserCommands();

    app.stop();

    return 0;
}

void SetRouteCallback(crow::SimpleApp& app)
{     
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::POST)(
        [](const crow::request& req)
        {
            EventHandler *eventHandler = ilrd::Handleton<EventHandler>::getInstance();
            CLI *cli = ilrd::Handleton<CLI>::getInstance();

            try
            {
                const std::string& eventName = req.get_header_value(g_XGithubEvent);
                Log("[SERVER][Received " + eventName + " event]", ilrd::Logger::INFO);
                
                crow::json::rvalue payload = crow::json::load(req.body.c_str());
                
                if (eventHandler->IsEventSuspicious(eventName, payload))
                {
                    cli->DisplaySuspiciousEvent(eventName);
                }
            }
            catch (const std::exception& except)
            {
                std::string errMsg = 
                    "[SERVER][Failed to parse webhook: ";
                errMsg += except.what();
                errMsg += ']';
                Log(errMsg, ilrd::Logger::ERROR);
                std::cerr << errMsg << "\n";
                return crow::response(crow::NOT_IMPLEMENTED);
            }

            return crow::response(crow::OK);
        }
    );
}

void Log(const std::string& message, ilrd::Logger::Severity severity)
{
    ilrd::Handleton<ilrd::Logger>::getInstance()->Log(message, severity);
}

void BlockAllSignals()
{
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, nullptr); 
}

void ErrUsage()
{
    Log("Invalid argument count", ilrd::Logger::ERROR);
    std::cerr << "Usage: " << "<exec> <bind_address> <port>\n";
}

void ErrInvalidPort()
{
    Log("Invalid port argument", ilrd::Logger::ERROR);
    std::cerr << "Invalid port.\n";
}

void ErrInvalidAddress()
{
    Log("Invalid port address argument", ilrd::Logger::ERROR);
    std::cerr << "Invalid address.\n";
}

static bool IsValidAddress(const char *address)
{
    struct sockaddr_in sa;
    return 1 == inet_pton(AF_INET, address, &(sa.sin_addr));
}

static void ParseArgs(int argc, const char **argv, 
    const char *&bindAddrOutParam, int &portOutParam)
{
    Log("Parsing arguments", ilrd::Logger::INFO);
    if (argc < g_argsRequired)
    {
        ErrUsage();
        std::exit(EXIT_FAILURE);
    }

    bindAddrOutParam = argv[g_addrArgIndex];
    if (!IsValidAddress(bindAddrOutParam))
    {
        ErrInvalidAddress();
        ErrUsage();
        std::exit(EXIT_FAILURE);
    }
    
    portOutParam = std::strtoimax(argv[g_portArgIndex], nullptr, g_baseTen);
    if (0 == portOutParam)
    {
        ErrInvalidPort();
        ErrUsage();
        std::exit(EXIT_FAILURE);
    }
}

void InitApp(crow::SimpleApp& app, const char* bindAddr ,int port)
{
    Log("[APP][Initializing address]", ilrd::Logger::INFO);
    app.bindaddr(bindAddr);
    Log("[APP][Initializing port]", ilrd::Logger::INFO);
    app.port(port);
    Log("[APP][Setting to multithreaded]", ilrd::Logger::INFO);
    app.multithreaded();
    #ifndef NDEBUG
        app.loglevel(crow::LogLevel::Debug);
    #else
        app.loglevel(crow::LogLevel::Error);
    #endif // NDEBUG
}
void InitEventHandler(EventHandler *eventHandler)
{
    Log("Registering events", ilrd::Logger::INFO);
    
    eventHandler->RegisterEvent(g_push, std::shared_ptr<IEventTypeChecker>(new PushEventChecker));
    eventHandler->RegisterEvent(g_team, std::shared_ptr<IEventTypeChecker>(new TeamEventChecker));
    eventHandler->RegisterEvent(g_repository, std::shared_ptr<IEventTypeChecker>(new RepoEventChecker));
}