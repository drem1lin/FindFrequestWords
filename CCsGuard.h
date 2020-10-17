#pragma once

#include <atlsync.h>

class CCsGuard {

public:
    CCsGuard(CRITICAL_SECTION* cs)
        : m_cs(cs)
    {
        EnterCriticalSection(m_cs);
    }
    ~CCsGuard() {
        LeaveCriticalSection(m_cs);
    }
    CCsGuard(const CCsGuard&) = delete;
    CCsGuard& operator = (const CCsGuard&) = delete;
private:
    CRITICAL_SECTION* m_cs;
};