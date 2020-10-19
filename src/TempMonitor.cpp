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
#include <TempMonitor.h>
#include <cassert>
#include <Log.h>

TempMonitor::TempMonitor(ParamGetterPtr&& paramGetter_) :
  paramGetter{std::move(paramGetter_)},
  doProcessing{false}
{
  assert(paramGetter);
}

void TempMonitor::start()
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (!mainLoopThread.joinable())
  {
    doProcessing.store(true);
    mainLoopThread = std::thread{&TempMonitor::mainLoop, this};
  }
}

void TempMonitor::stop()
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (mainLoopThread.joinable())
  {
    doProcessing.store(false);
    mainLoopThread.join();
  }
}

void TempMonitor::checkParamValue(const char* paramString, ParamGetter::Result result)
{
  if (result.ioStatus == IoStatus::ok)
  {
    LOGI("TempMonitor::checkParamValue: %s: %2.1f\n", paramString, result.value);
  }
  else
  {
    LOGE("TempMonitor::checkParamValue: %s read %s\n", paramString, ioStatusName.at(result.ioStatus).c_str());
  }
}

void TempMonitor::mainLoop()
{
  while (doProcessing.load())
  {
    LOGD("TempMonitor::mainLoop: loop rolling\n");

    checkParamValue("Kettle temperature",     paramGetter->get(ParamId::TEMP_KETTLE_CURRENT));
    checkParamValue("Day temperature target", paramGetter->get(ParamId::TEMP_ROOM_DAY_NORMAL_TARGET));

    std::this_thread::sleep_for(std::chrono::milliseconds(tempReadInterval));
  }
}
