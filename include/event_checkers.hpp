#ifndef __EVENT_CHECKERS_HPP__
#define __EVENT_CHECKERS_HPP__

#include <chrono>          // std::chrono::hours
#include <cstdint>         // uint64_t
#include <unordered_map>   // std::unordered_map

#include "crow_all.h"

using namespace std::chrono_literals;

// CHECKER must provide a function of the following type:
// bool IsEventSuspicious(const crow::json::rvalue&);

class IEventTypeChecker
{
public:
    IEventTypeChecker() = default;

    IEventTypeChecker(const IEventTypeChecker& rhs) = delete;
    IEventTypeChecker(const IEventTypeChecker&& rhs) = delete;
    IEventTypeChecker& operator=(const IEventTypeChecker& rhs) = delete;
    IEventTypeChecker& operator=(const IEventTypeChecker&& rhs) = delete;
    ~IEventTypeChecker() = default;

    virtual bool IsEventSuspicious(const crow::json::rvalue& payload) = 0;
};

class PushEventChecker: public IEventTypeChecker
{
public:
    using Hours = std::chrono::hours;
    PushEventChecker(const Hours& startHour = 14h, const Hours& invalidDuration = 2h);

    bool IsEventSuspicious(const crow::json::rvalue& payload);
private:
    Hours GetToHourOfDay(const crow::json::rvalue& payload);
    Hours m_startHour;
    Hours m_endHour;
};

class TeamEventChecker: public IEventTypeChecker
{
public:
    TeamEventChecker() = default;
    ~TeamEventChecker() = default;
    bool IsEventSuspicious(const crow::json::rvalue& payload);
private:
    bool isCreated(const crow::json::rvalue& payload);
    bool hasHackerPrefix(const crow::json::rvalue& payload);
};

class RepoEventChecker: public IEventTypeChecker
{
public:
    RepoEventChecker() = default;
    ~RepoEventChecker() = default;
    bool IsEventSuspicious(const crow::json::rvalue& payload);

private:
    using TimePoint = std::chrono::system_clock::time_point;
    using Minutes = std::chrono::minutes;
    Minutes GetRepoAge(uint64_t repoId);
    std::unordered_map<uint64_t, TimePoint> m_repoCreationTimes;
};

#endif // __EVENT_CHECKERS_HPP__