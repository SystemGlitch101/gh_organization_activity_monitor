/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File: event_handler.hpp                               *
 *                                                       *
 * Description:                                          *
 *    The EventHandler is responsible for checking the   *
 *    type of the event and forwarding it to the         *
 *    appropriate event checker.                         *
 *                                                       *
 *    It does so by storing a map, with the key being    *
 *    the name (string), and the value being the event   *
 *    checker (shared pointer).                          *
 *                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __EVENT_HANDLER_HPP__
#define __EVENT_HANDLER_HPP__

#include <string>              // std::string
#include <memory>              // std::shared_ptr
#include <unordered_map>       // std::unordered_map

#include "crow_all.h"          // crow::json::rvalue
#include "event_checkers.hpp"  // IEventTypeChecker
#include "handleton.hpp"       // ilrd::Handleton
#include "logger.hpp"          // ilrd::Logger

class EventHandler
{
public:
    // non-copyable object
    EventHandler(const EventHandler& rhs) = delete;
    EventHandler(const EventHandler&& rhs) = delete;
    EventHandler& operator=(const EventHandler& rhs) = delete;
    EventHandler& operator=(const EventHandler&& rhs) = delete;

    // default destructor
    ~EventHandler() = default;

    // Registers an event type checker to the map
    void RegisterEvent(const std::string& eventName, std::shared_ptr<IEventTypeChecker> eventType);
    
    // Unregisters an event type checker
    void UnregisterEvent(const std::string& eventName);

    // Calls the corresponding event checker's IsEventSuspicious() function
    bool IsEventSuspicious(const std::string& eventName, const crow::json::rvalue& payLoad);
private:
    // private constructor for singleton pattern, initializes the logger reference
    EventHandler();

    // Ensures the handleton can call the EventHandler constructor.
    friend class ilrd::Handleton<EventHandler>;

    static ilrd::Logger *s_logger;
    using SharedEvent = std::shared_ptr<IEventTypeChecker>;
  
    std::unordered_map<std::string, SharedEvent> m_eventsMap;
};

#endif // __EVENT_HANDLER