/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XMRIG_PLATFORM_H
#define XMRIG_PLATFORM_H


#include <cstdint>
#include "base/tools/String.h"
#include "base/tools/Chrono.h"

#ifdef WIN32
#include "win_ports/ressource.h"
#else
#include <sys/times.h>
#include <sys/resource.h>
#include <cstdio>
#include <thread>
#include <string>

#endif

namespace xmrig {

static thread_local uint64_t m_threadTimeToSleep = {0};
static thread_local uint64_t m_threadUsageTime = {0};
static thread_local uint64_t m_systemTime = {0};

class Platform
{
public:
    static inline bool trySetThreadAffinity(int64_t cpu_id)
    {
        if (cpu_id < 0) {
            return false;
        }

        return setThreadAffinity(static_cast<uint64_t>(cpu_id));
    }

    static inline uint64_t getThreadSleepTimeToLimitMaxCpuUsage(uint8_t maxCpuUsage)
    {
      uint64_t currentSystemTime = Chrono::highResolutionMicroSecs();

      struct rusage usage {};
      if (getrusage(RUSAGE_THREAD, &usage) == 0) {
        uint64_t currentThreadUsageTime = usage.ru_utime.tv_usec + (usage.ru_utime.tv_sec * 1000000)
                                        + usage.ru_stime.tv_usec + (usage.ru_stime.tv_sec * 1000000);

        if (m_threadUsageTime > 0 || m_systemTime > 0)
        {
          m_threadTimeToSleep = ((currentThreadUsageTime - m_threadUsageTime) * 100 / maxCpuUsage)
                              - (currentSystemTime - m_systemTime - m_threadTimeToSleep);
        }

        m_threadUsageTime = currentThreadUsageTime;
        m_systemTime = currentSystemTime;
      }

      return m_threadTimeToSleep;
    }

    static bool setThreadAffinity(uint64_t cpu_id);
    static uint32_t setTimerResolution(uint32_t resolution);
    static void init(const char *userAgent);
    static void restoreTimerResolution();
    static void setProcessPriority(int priority);
    static void setThreadPriority(int priority);
    static inline const char* userAgent() { return m_userAgent; }

private:
    static char *createUserAgent();
    static String m_userAgent;
};


} // namespace xmrig


#endif /* XMRIG_PLATFORM_H */
