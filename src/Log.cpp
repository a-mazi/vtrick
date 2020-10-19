/* Copyright © Artur Maziarek MMXX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <Log.h>
#include <time.h>
#include <string.h>
#include <mutex>
#include <sstream>
#include <iomanip>

std::string timestamp()
{
  time_t timeNow;
  time(&timeNow);

  static std::mutex timeLibControl{};
  struct tm* timeNowStruct = new tm;
  bool isTimeNowOk = false;

  {
    std::lock_guard<std::mutex> timeLibLock{timeLibControl};
    struct tm* timeNowStructTmp = localtime(&timeNow);
    if (timeNowStruct != nullptr) {
      memcpy(timeNowStruct, timeNowStructTmp, sizeof(tm));
      isTimeNowOk = true;
    }
  }

  std::stringstream stampStream{};
  if (isTimeNowOk) {
  stampStream <<  std::setfill('0') << std::setw(2) <<  timeNowStruct->tm_mon + 1
              <<  std::setfill('0') << std::setw(2) <<  timeNowStruct->tm_mday
              <<  std::setfill('0') << std::setw(2) <<  timeNowStruct->tm_hour
              <<  std::setfill('0') << std::setw(2) <<  timeNowStruct->tm_min
              <<  std::setfill('0') << std::setw(2) <<  timeNowStruct->tm_sec;
  }

  delete timeNowStruct;
  return stampStream.str();
}
