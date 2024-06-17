#include <unistd.h>       // STDIN_FILENO
#include <fcntl.h>        // O_RDONLY

#include "crow_all.h"     // crow::SimpleApp

const size_t g_bufSize = 512;
bool g_isAcceptingCommands = true;

static void Init(crow::SimpleApp& app);
static void AcceptCommands();
static void ExecuteCommand(std::string& command);
static bool IsValidCommand(std::string& command);

static void HandleWebhookEvent();

int main()
{
    crow::SimpleApp app;
    
    Init(app);
    
    CROW_ROUTE(app, "/")
    .methods("GET"_method)(
        [](const crow::request& req)
        {
            crow::response response("Hello world!");
            std::cout << req.get_body_params();
            std::cout << "Body:\n" << req.body;
            return response;
        }
    );

    std::future<void> future = app.run_async();

    AcceptCommands();

    return 0;
}

void Init(crow::SimpleApp& app)
{
    app.bindaddr("127.0.0.1");
    app.port(25565);
    app.multithreaded();
}

void AcceptCommands()
{
    char buf[g_bufSize] = { 0 };

    while (!g_isAcceptingCommands)
    {
        std::cin.getline(buf, g_bufSize - 1);
        std::string command(buf);

        size_t lineEndIndex = command.find('\n');
        if (lineEndIndex != command.npos)
        {
            command[lineEndIndex] = '\0';
        }
        else
        {
            std::cout << "Failed to read line \n";
            continue;
        }

        if (!IsValidCommand(command))
        {
            std::cerr << "Invalid command\n";
            continue;
        }

        ExecuteCommand(command);
    }
}



// TODO: add logic later
bool IsValidCommand(std::string& command)
{
    return true;
}


// TODO: add map of commands later, maybe create object for it
void ExecuteCommand(std::string& command)
{
    if (command == "exit")
    {
        g_isAcceptingCommands = false;
    }
}


void HandleWebhookEvent()
{    
}