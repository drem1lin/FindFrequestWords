// This is a personal academic project.Dear PVS - Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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