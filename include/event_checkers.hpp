/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File: event_checkers.hpp                              *
 *                                                       *
 * Description:                                          *
 *    The IEventTypeChecker is an interface from which   *
 *    different types objects representing various       *
 *    events.                                            *
 *                                                       *
 *    This header includes the three concrete            *
 *    classes which represent the events in the          *
 *    requirements of the exercise, and more may be      *
 *    added later on.                                    *
 *                                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __EVENT_CHECKERS_HPP__
#define __EVENT_CHECKERS_HPP__

#include <chrono>          // std::chrono::hours
#include <cstdint>         // uint64_t
#include <unordered_map>   // std::unordered_map

#include "crow_all.h"      // std::chrono::json::rvalue

using namespace std::chrono_literals; // hours, minutes literals

class IEventTypeChecker
{
public:
    // Default constructor.
    IEventTypeChecker() = default;

    // Non copyable object.
    IEventTypeChecker(const IEventTypeChecker& rhs) = delete;
    IEventTypeChecker(const IEventTypeChecker&& rhs) = delete;
    IEventTypeChecker& operator=(const IEventTypeChecker& rhs) = delete;
    IEventTypeChecker& operator=(const IEventTypeChecker&& rhs) = delete;
    
    // Default destructor.
    ~IEventTypeChecker() = default;

    // Pure virtual method, to be overridden by inheritors.
    // Returns true if the event is considered suspicious.
    virtual bool IsEventSuspicious(const crow::json::rvalue& payLoad) = 0;
};


// The "push" event checker class.
class PushEventChecker: public IEventTypeChecker
{
public:
    // Alias to ease writing.
    using Hours = std::chrono::hours;

    // Constructor. 
    // Args:
    //   startHour       - The starting hour of the invalid period
    //   invalidDuration - The duration of the invalid period.
    PushEventChecker(const Hours& startHour = 14h, const Hours& invalidDuration = 2h);

    // Default destructor.
    ~PushEventChecker() = default;
    
    // Implementation of the virtual method.
    bool IsEventSuspicious(const crow::json::rvalue& payLoad);
private:

    // Helper function that extracts the hour of the push event.
    Hours GetHourOfDay(const crow::json::rvalue& payLoad);

    Hours m_startHour;
    Hours m_endHour;
};

// The "team" event checker class.
class TeamEventChecker: public IEventTypeChecker
{
public:
    // Default constructor.
    TeamEventChecker() = default;
    
    // Default destructor.
    ~TeamEventChecker() = default;
    
    // Implementation of the virtual method.
    bool IsEventSuspicious(const crow::json::rvalue& payLoad);
private:

    // Helper function, checks if it the team was created.
    bool IsCreated(const crow::json::rvalue& payLoad);
    
    // Helper function, checks if it the team name starts with "hacker".
    // The comparison is case insensitive.
    bool HasHackerPrefix(const crow::json::rvalue& payLoad);
};

// The "repository" event checker class.
class RepoEventChecker: public IEventTypeChecker
{
public:
    // Default constructor.
    RepoEventChecker() = default;
    
    // Default destructor.
    ~RepoEventChecker() = default;
    
    // Pure virtual method implementation.
    bool IsEventSuspicious(const crow::json::rvalue& payLoad);

private:
    // Aliases for time types.
    using TimePoint = std::chrono::system_clock::time_point;
    using Minutes = std::chrono::minutes;
    
    // Helper that returns the repository age in minutes.
    Minutes GetRepoAge(uint64_t repoId);
    std::unordered_map<uint64_t, TimePoint> m_repoCreationTimes;
};

#endif // __EVENT_CHECKERS_HPP__