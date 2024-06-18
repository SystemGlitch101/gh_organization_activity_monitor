#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <string>         // std::string
#include <unordered_map>  // std::unordered_map

#include "handleton.hpp"  // getInstance()
#include "logger.hpp"     // ilrd::Logger
#include "crow_all.h"     // crow::json::rvalue


class ICommand
{
public:
    ICommand() = default;
    ICommand(const ICommand& rhs) = delete;
    ICommand(const ICommand&& rhs) = delete;
    ICommand& operator=(const ICommand& rhs) = delete;
    ICommand& operator=(const ICommand&& rhs) = delete;
    ~ICommand() = default;
    virtual void Execute() = 0;
};

class ExitCommand: public ICommand
{
public:
    inline ExitCommand(bool& runFlag);
    inline void Execute();
    ~ExitCommand() = default;
private:
    bool& m_runFlag;
};

class CLI
{
public:
    inline CLI();
    CLI(const CLI& rhs) = delete;
    CLI(const CLI&& rhs) = delete;
    CLI& operator=(const CLI& rhs) = delete;
    CLI& operator=(const CLI&& rhs) = delete;
    ~CLI() = default;

    void AcceptUserCommands();

    void ExecuteCommand(const std::string& name);
    void RegisterCommand(const std::string& name, std::shared_ptr<ICommand> command);
    void UnregisterCommand(const std::string& name);

    static void DisplaySuspiciousEvent(const std::string& name, 
                                    const crow::request& request,
                                    const crow::json::rvalue& payload);
    static void DisplayRequest(const crow::request& req, 
                               const crow::json::rvalue& payload);
    static void DisplayJsonUnformatted(const crow::json::rvalue& payload);
private:
    bool IsValidCommand(const std::string& name);

    static ilrd::Logger *s_logger;

    bool m_isAcceptingCommands;
    std::unordered_map<std::string, std::shared_ptr<ICommand> > m_commandsMap;
};

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

#endif // 