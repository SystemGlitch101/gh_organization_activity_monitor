#include <iostream>    // io functions
#include "crow_all.h"  // crow::json::rvalue, crow::json::request
#include "logger.hpp"  // ilrd::Logger

#include "cli.hpp"

ilrd::Logger *CLI::s_logger = nullptr;

CLI::CLI(): m_isAcceptingCommands(false), m_commandsMap() 
{
    s_logger = ilrd::Handleton<ilrd::Logger>::getInstance();
    RegisterCommand("exit", std::shared_ptr<ICommand>(new ExitCommand(m_isAcceptingCommands)));
}

ExitCommand::ExitCommand(bool& runFlag): m_runFlag(runFlag) 
{
}

void ExitCommand::Execute() 
{
    m_runFlag = false;
}

bool CLI::IsValidCommand(const std::string& name)
{
    return m_commandsMap.find(name) != m_commandsMap.end();
}

void CLI::AcceptUserCommands()
{
    m_isAcceptingCommands = true;
    while (m_isAcceptingCommands)
    {
        std::string command;
        std::cout << "> Type 'exit' to stop the application.\n";
        std::cout << "> ";
        std::getline(std::cin, command);

        if (!IsValidCommand(command))
        {
            std::string errMsg = "[APP][Invalid command: ";
            errMsg += command;
            errMsg += "]";
            s_logger->Log(errMsg, ilrd::Logger::ERROR);
            std::cerr << errMsg << "\n";
          
            continue;
        }

        ExecuteCommand(command);
    }
}

void CLI::ExecuteCommand(const std::string& name)
{
    s_logger->Log("Executing command: " + name, ilrd::Logger::INFO);
    m_commandsMap.at(name)->Execute();
}

void CLI::RegisterCommand(const std::string& name, std::shared_ptr<ICommand> command)
{
    m_commandsMap.insert(std::make_pair(name, command));
}

void CLI::UnregisterCommand(const std::string& name)
{
    m_commandsMap.erase(name);
}

void CLI::DisplaySuspiciousEvent(const std::string& eventName)
{
    std::string warnMessage("[SERVER][Suspicious event detected][Event Name: ");
    warnMessage += eventName;
    warnMessage += "]";
    s_logger->Log(warnMessage, ilrd::Logger::WARN);
    std::cout << warnMessage << "\n";
}

void CLI::DisplaySuspiciousEvent(const std::string& eventName, 
                              const crow::request& request,
                              const crow::json::rvalue& payload)
{
    DisplaySuspiciousEvent(eventName);
    DisplayRequest(request, payload);
}

void CLI::DisplayRequest(const crow::request& req, 
                         const crow::json::rvalue& payload)
{
    auto headers = req.headers;
    std::cout << "Headers:\n";
    for (auto& header : headers)
    {
        std::cout << "\t" << header.first << ", " << header.second << "\n";
    }

    std::cout << "URL params:\n" << req.url_params << '\n';
    
    DisplayJsonUnformatted(payload);
}

void CLI::DisplayJsonUnformatted(const crow::json::rvalue& payload)
{
    std::cout << payload;
}