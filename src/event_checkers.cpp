#include <chrono>             // std::chrono::system_clock::now
#include <cinttypes>          // strtoimax
#include "event_checkers.hpp" 



// ==================== PushEventChecker =========================

PushEventChecker::PushEventChecker(const Hours& startHour, const Hours& invalidDuration):
    m_startHour(startHour % 24), 
    m_endHour(m_startHour + (invalidDuration % (24h - m_startHour)))
{
}

PushEventChecker::Hours PushEventChecker::GetToHourOfDay(const crow::json::rvalue& payload)
{
    char hourBuf[3] = { 0 };
    std::string timeStampStr = payload["head_commit"]["timestamp"].s();

    // format: 2024-06-18T12:00:10+03:00
    size_t hourIndex = timeStampStr.find("T") + 1;
    timeStampStr.copy(hourBuf, sizeof(hourBuf) - 1, hourIndex);
    int actualHours = strtoimax(hourBuf, nullptr, 10);

    return Hours(actualHours);
}


bool PushEventChecker::IsEventSuspicious(const crow::json::rvalue& payload)
{
    Hours hourOfDay = GetToHourOfDay(payload);
    
    return (m_startHour <= hourOfDay && hourOfDay <= m_endHour);
}


// ==================== TeamEventChecker =========================

bool TeamEventChecker::IsEventSuspicious(const crow::json::rvalue& payload)
{
    return isCreated(payload) && hasHackerPrefix(payload);
}

bool TeamEventChecker::isCreated(const crow::json::rvalue& payload)
{
    return  payload["action"] == "created";
}

bool TeamEventChecker::hasHackerPrefix(const crow::json::rvalue& payload)
{
    std::string name(payload["team"]["name"].s());

    return 0 == strncasecmp(name.c_str(), "hacker", 5);
}


// ==================== RepoEventChecker =========================

RepoEventChecker::Minutes RepoEventChecker::GetRepoAge(uint64_t repoId)
{
    return std::chrono::duration_cast<Minutes>(std::chrono::system_clock::now() - m_repoCreationTimes.at(repoId));
}

bool RepoEventChecker::IsEventSuspicious(const crow::json::rvalue& payload)
{
    uint64_t repoId = 0;
    if (payload["action"] == "deleted")
    {
        repoId = payload["repository"]["id"].u();
        bool ret = (GetRepoAge(repoId) <= 10min);
        m_repoCreationTimes.erase(repoId);

        return ret;
    }

    if (payload["action"] == "created")
    {
        m_repoCreationTimes[payload["repository"]["id"].u()] = std::chrono::system_clock::now();
    }

    return false;
}

