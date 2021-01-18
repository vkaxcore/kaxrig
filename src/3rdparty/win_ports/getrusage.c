/*
 * Copyright (c) 2014 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <time.h>
#include "ressource.h"

static void usage_to_timeval(FILETIME *ft, struct timeval *tv)
{
  ULARGE_INTEGER time;
  time.LowPart = ft->dwLowDateTime;
  time.HighPart = ft->dwHighDateTime;
  
  tv->tv_sec = time.QuadPart / 10000000;
  tv->tv_usec = (time.QuadPart % 10000000) / 10;
}

int getrusage(int who, struct rusage *usage)
{
  FILETIME kernel_time, user_time, creation_time, exit_time; 
  
  memset(usage, 0, sizeof(struct rusage));

  if (who == RUSAGE_SELF) {
    if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
                         &kernel_time, &user_time)) {
      return -1;
    }

    usage_to_timeval(&kernel_time, &usage->ru_stime);
    usage_to_timeval(&user_time, &usage->ru_utime);

    return 0;

  } else if (who == RUSAGE_THREAD) {
	
	//Get Thread user and kernel times
    if(!GetThreadTimes(GetCurrentThread(), &creation_time, &exit_time,
                       &kernel_time, &user_time)) {
        return -1;
	}
		
    //Calculates time spent by this thread in user and kernel mode.
	
	usage_to_timeval(&kernel_time, &usage->ru_stime);
    usage_to_timeval(&user_time, &usage->ru_utime);
    
    return 0;
  } else {
    return -1;
  }
}