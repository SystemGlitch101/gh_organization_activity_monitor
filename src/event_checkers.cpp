#include <chrono>             // std::chrono::system_clock::now()
#include <cinttypes>          // strtoimax
#include "event_checkers.hpp" 



// ==================== PushEventChecker =========================

PushEventChecker::PushEventChecker(const Hours& startHour, const Hours& invalidDuration):
    m_startHour(startHour % 24), 
    m_endHour(m_startHour + (invalidDuration % (24h - m_startHour)))
{
}

PushEventChecker::Hours PushEventChecker::GetHourOfDay(const crow::json::rvalue& payLoad)
{
    char hourBuf[3] = { 0 };
    std::string timeStampStr = payLoad["head_commit"]["timestamp"].s();

    // format: 2024-06-18T12:00:10+03:00
    size_t hourIndex = timeStampStr.find("T") + 1;
    timeStampStr.copy(hourBuf, sizeof(hourBuf) - 1, hourIndex);
    int actualHours = strtoimax(hourBuf, nullptr, 10);

    return Hours(actualHours);
}


bool PushEventChecker::IsEventSuspicious(const crow::json::rvalue& payLoad)
{
    Hours hourOfDay = GetHourOfDay(payLoad);
    
    return (m_startHour <= hourOfDay && hourOfDay <= m_endHour);
}


// ==================== TeamEventChecker =========================

bool TeamEventChecker::IsEventSuspicious(const crow::json::rvalue& payLoad)
{
    return IsCreated(payLoad) && HasHackerPrefix(payLoad);
}

bool TeamEventChecker::IsCreated(const crow::json::rvalue& payLoad)
{
    return  payLoad["action"] == "created";
}

bool TeamEventChecker::HasHackerPrefix(const crow::json::rvalue& payLoad)
{
    std::string name(payLoad["team"]["name"].s());

    return 0 == strncasecmp(name.c_str(), "hacker", 5);
}


// ==================== RepoEventChecker =========================

RepoEventChecker::Minutes RepoEventChecker::GetRepoAge(uint64_t repoId)
{
    return std::chrono::duration_cast<Minutes>(std::chrono::system_clock::now() - m_repoCreationTimes.at(repoId));
}

bool RepoEventChecker::IsEventSuspicious(const crow::json::rvalue& payLoad)
{
    uint64_t repoId = 0;
    if (payLoad["action"] == "deleted")
    {
        repoId = payLoad["repository"]["id"].u();
        bool ret = (GetRepoAge(repoId) <= 10min);
        m_repoCreationTimes.erase(repoId);

        return ret;
    }

    if (payLoad["action"] == "created")
    {
        m_repoCreationTimes[payLoad["repository"]["id"].u()] = std::chrono::system_clock::now();
    }

    return false;
}

