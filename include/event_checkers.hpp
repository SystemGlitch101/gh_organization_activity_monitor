#ifndef __EVENT_CHECKERS_HPP__
#define __EVENT_CHECKERS_HPP__

#include <chrono>          // std::chrono::hours
#include <cstdint>         // uint64_t
#include <unordered_map>   // std::unordered_map

#include "crow_all.h"      // std::chrono::json::rvalue

using namespace std::chrono_literals; // hours, minutes

class IEventTypeChecker
{
public:
    IEventTypeChecker() = default;

    IEventTypeChecker(const IEventTypeChecker& rhs) = delete;
    IEventTypeChecker(const IEventTypeChecker&& rhs) = delete;
    IEventTypeChecker& operator=(const IEventTypeChecker& rhs) = delete;
    IEventTypeChecker& operator=(const IEventTypeChecker&& rhs) = delete;
    ~IEventTypeChecker() = default;

    virtual bool IsEventSuspicious(const crow::json::rvalue& payLoad) = 0;
};

class PushEventChecker: public IEventTypeChecker
{
public:
    using Hours = std::chrono::hours;
    PushEventChecker(const Hours& startHour = 14h, const Hours& invalidDuration = 2h);

    bool IsEventSuspicious(const crow::json::rvalue& payLoad);
private:
    Hours GetHourOfDay(const crow::json::rvalue& payLoad);
    Hours m_startHour;
    Hours m_endHour;
};

class TeamEventChecker: public IEventTypeChecker
{
public:
    TeamEventChecker() = default;
    ~TeamEventChecker() = default;
    bool IsEventSuspicious(const crow::json::rvalue& payLoad);
private:
    bool IsCreated(const crow::json::rvalue& payLoad);
    bool HasHackerPrefix(const crow::json::rvalue& payLoad);
};

class RepoEventChecker: public IEventTypeChecker
{
public:
    RepoEventChecker() = default;
    ~RepoEventChecker() = default;
    bool IsEventSuspicious(const crow::json::rvalue& payLoad);

private:
    using TimePoint = std::chrono::system_clock::time_point;
    using Minutes = std::chrono::minutes;
    Minutes GetRepoAge(uint64_t repoId);
    std::unordered_map<uint64_t, TimePoint> m_repoCreationTimes;
};

#endif // __EVENT_CHECKERS_HPP__