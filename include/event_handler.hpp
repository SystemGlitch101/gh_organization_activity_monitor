#ifndef __EVENT_HANDLER_HPP__
#define __EVENT_HANDLER_HPP__

#include <string> // std::string
#include <memory> // std::shared_ptr
#include <unordered_map> // std::unordered_map

#include "crow_all.h"
#include "event_checkers.hpp"  // IEventTypeChecker
#include "logger.hpp"

class EventHandler
{
public:
    EventHandler();
    EventHandler(const EventHandler& rhs) = delete;
    EventHandler(const EventHandler&& rhs) = delete;
    EventHandler& operator=(const EventHandler& rhs) = delete;
    EventHandler& operator=(const EventHandler&& rhs) = delete;
    ~EventHandler() = default;

    void RegisterEvent(const std::string& eventName, std::shared_ptr<IEventTypeChecker> eventType);
    void UnregisterEvent(const std::string& eventName);
    bool IsEventSuspicious(const std::string& eventName, const crow::json::rvalue& payload);
private:
    static ilrd::Logger *s_logger;
    using SharedEvent = std::shared_ptr<IEventTypeChecker>;
  
    std::unordered_map<std::string, SharedEvent> m_eventsMap;
};

#endif // __EVENT_HANDLER