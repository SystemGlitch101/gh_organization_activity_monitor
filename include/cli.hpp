/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File: cli.hpp                                         *
 *                                                       *
 * Description:                                          *
 *    The CLI is responsible for accepting terminal      *
 *    commands.                                          *
 *                                                       *
 *    Commands can be registered and unregistered, and   *
 *    the command object is an interface called ICommand *
 *    from which concrete commands will inherit.         *
 *                                                       *
 *    The commands will be stored in a map, the name     *
 *    being the key, and the value being an object that  *
 *    inherits from the ICommand interface.              *
 *                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <string>         // std::string
#include <unordered_map>  // std::unordered_map

#include "handleton.hpp"  // ilrd::Handleton
#include "logger.hpp"     // ilrd::Logger
#include "crow_all.h"     // crow::json::rvalue


class ICommand
{
public:
    // Default constructor for the interface.
    ICommand() = default;


    // Non copyable object.
    ICommand(const ICommand& rhs) = delete;
    ICommand(const ICommand&& rhs) = delete;
    ICommand& operator=(const ICommand& rhs) = delete;
    ICommand& operator=(const ICommand&& rhs) = delete;
    
    // Default destructor.
    ~ICommand() = default;
    
    // Pure virtual method for executing the command
    virtual void Execute() = 0;
};

// Concrete exit command which is automatically registered
// to the CLI.
class ExitCommand: public ICommand
{
public:
    // Constructor receiving a reference to the 
    // run flag, which is located in the CLI.
    ExitCommand(bool& runFlag);

    // Implementation of the virtual method. 
    void Execute();

    // Default destructor.
    ~ExitCommand() = default;
private:
    bool& m_runFlag;
};

class CLI
{
public:
    // Non copyable object.
    CLI(const CLI& rhs) = delete;
    CLI(const CLI&& rhs) = delete;
    CLI& operator=(const CLI& rhs) = delete;
    CLI& operator=(const CLI&& rhs) = delete;
    
    // Default destructor.
    ~CLI() = default;


    // Accepts user commands. 
    // This function blocks the current thread.
    void AcceptUserCommands();

    // Executes a command received from the user. 
    // If no command is found, an error message is
    // displayed.
    void ExecuteCommand(const std::string& name);

    // Registers a command to the commands map.
    void RegisterCommand(const std::string& name, std::shared_ptr<ICommand> command);
    
    // Removes a command from the commands map.
    void UnregisterCommand(const std::string& name);


    // Displays the suspicious event's name, 
    // without additional information.
    static void DisplaySuspiciousEvent(const std::string& name);

    // Displays the full detail of the suspicious event
    static void DisplaySuspiciousEvent(const std::string& name, 
                                    const crow::request& request,
                                    const crow::json::rvalue& payload);
    
    // Displays the a request's information, including the
    // payload (assumed to be json).
    static void DisplayRequest(const crow::request& req, 
                               const crow::json::rvalue& payload);
    
    // Displays a raw json payload.
    static void DisplayJsonUnformatted(const crow::json::rvalue& payload);
private:
    // Private CLI Constructor, for singleton pattern.
    CLI();

    friend class ilrd::Handleton<CLI>;

    // Helper function to check if a command is registered.
    bool IsValidCommand(const std::string& name);

    static ilrd::Logger *s_logger;

    bool m_isAcceptingCommands;
    std::unordered_map<std::string, std::shared_ptr<ICommand> > m_commandsMap;
};

#endif // 