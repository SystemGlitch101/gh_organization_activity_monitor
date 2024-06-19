#include "event_handler.hpp"
#include "handleton.hpp"
#include "logger.hpp"

ilrd::Logger *EventHandler::s_logger = nullptr;

EventHandler::EventHandler()
{
    s_logger = ilrd::Handleton<ilrd::Logger>::getInstance();
}

void EventHandler::RegisterEvent(const std::string& eventName, std::shared_ptr<IEventTypeChecker> eventType)
{
    s_logger->Log("Registering " + eventName + " event callback", ilrd::Logger::INFO);
    m_eventsMap.insert(std::make_pair(eventName, eventType));
}

void EventHandler::UnregisterEvent(const std::string& eventName)
{
    s_logger->Log("[SERVER][Unregistering " + eventName + " event callback function]", ilrd::Logger::INFO);

    m_eventsMap.erase(eventName);
}

bool EventHandler::IsEventSuspicious(const std::string& eventName, const crow::json::rvalue& payLoad)
{
    s_logger->Log("[SERVER][Checking if " + eventName + " event is suspicious]", ilrd::Logger::INFO);
    return m_eventsMap.at(eventName)->IsEventSuspicious(payLoad);
}