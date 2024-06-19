#include <cinttypes>           // intmax_t
#include <cstdlib>             // strtoimax
#include <signal.h>            // sigprocmask
#include <strings.h>
#include <unistd.h>            // STDIN_FILENO
#include <fcntl.h>             // O_RDONLY

#include "handleton.hpp"
#include "logger.hpp"
#include "crow_all.h"          // crow::SimpleApp
#include "cli.hpp"             // Command Line Interface
#include "event_checkers.hpp"     // IEventTypeChecker and its implementaions
#include "event_handler.hpp"   // EventHandler

int g_port = 0;
const char *g_bindAddr = nullptr;
CLI *g_commandLineInterface = nullptr;

static EventHandler *s_eventHandler = nullptr;

inline void Log(const std::string& message, ilrd::Logger::Severity severity);

static void BlockAllSignals();
static void InitApp(crow::SimpleApp& app);
static void InitEventHandler();
static void SetRouteCallback(crow::SimpleApp& app);

static bool IsValidAddress(const char *address);
static void ParseArgs(int argc, const char **argv);

static void ErrUsage();
static void ErrInvalidAddress();
static void ErrInvalidPort();

int main(int argc, const char **argv)
{
    ParseArgs(argc, argv);

    crow::SimpleApp app;
    g_commandLineInterface = new CLI;

    BlockAllSignals();
    InitApp(app);
    InitEventHandler();
    SetRouteCallback(app);

    std::future<void> future = app.run_async();
    
    g_commandLineInterface->AcceptUserCommands();

    app.stop();

    return 0;
}


void SetRouteCallback(crow::SimpleApp& app)
{        
    CROW_ROUTE(app, "/")
    .methods("POST"_method)(
        [](const crow::request& req)
        {
            try
            {
                const std::string& eventName = req.get_header_value("X-GitHub-Event");
                Log("[SERVER][Received " + eventName + " event]", ilrd::Logger::INFO);
                
                crow::json::rvalue payload = crow::json::load(req.body.c_str());
                
                if (s_eventHandler->IsEventSuspicious(eventName, payload))
                {
                    g_commandLineInterface->DisplaySuspiciousEvent(eventName, req, payload);
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
    std::cout << "Usage: " << "<exec> <bind_address> <port>\n";
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

static void ParseArgs(int argc, const char **argv)
{
    Log("Parsing arguments", ilrd::Logger::INFO);
    if (argc < 3)
    {
        ErrUsage();
        std::exit(EXIT_FAILURE);
    }

    g_bindAddr = argv[1];
    if (!IsValidAddress(g_bindAddr))
    {
        ErrInvalidAddress();
        ErrUsage();
        std::exit(EXIT_FAILURE);
    }
    
    g_port = std::strtoimax(argv[2], nullptr, 10);
    if (0 == g_port)
    {
        ErrInvalidPort();
        ErrUsage();
        std::exit(EXIT_FAILURE);
    }
}

void InitApp(crow::SimpleApp& app)
{
    Log("[APP][Initializing address]", ilrd::Logger::INFO);
    app.bindaddr(g_bindAddr);
    Log("[APP][Initializing port]", ilrd::Logger::INFO);
    app.port(g_port);
    Log("[APP][Setting to multithreaded]", ilrd::Logger::INFO);
    app.multithreaded();
    #ifndef NDEBUG
    app.loglevel(crow::LogLevel::Debug);
    #else
    app.loglevel(crow::LogLevel::Error);
    #endif // NDEBUG
}
void InitEventHandler()
{
    Log("Registering events", ilrd::Logger::INFO);
    s_eventHandler = new EventHandler();
    s_eventHandler->RegisterEvent("push", std::shared_ptr<IEventTypeChecker>(new PushEventChecker));
    s_eventHandler->RegisterEvent("team", std::shared_ptr<IEventTypeChecker>(new TeamEventChecker));
    s_eventHandler->RegisterEvent("repository", std::shared_ptr<IEventTypeChecker>(new RepoEventChecker));
}