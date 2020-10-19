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
#include <TempSetter.h>
#include <cassert>
#include <Log.h>

TempSetter::TempSetter(ParamSetterPtr&& paramSetter_) :
  paramSetter{std::move(paramSetter_)},
  doProcessing{false}
{
  assert(paramSetter);
}

void TempSetter::start()
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (!mainLoopThread.joinable())
  {
    doProcessing.store(true);
    mainLoopThread = std::thread{&TempSetter::mainLoop, this};
  }
}

void TempSetter::stop()
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (mainLoopThread.joinable())
  {
    doProcessing.store(false);
    mainLoopThread.join();
  }
}

void TempSetter::checkStatus(IoStatus status)
{
  if (status == IoStatus::ok)
  {
    LOGI("TempSetter::checkStatus: Parameter setting success\n");
  }
  else
  {
    LOGE("TempSetter::checkStatus: Parameter setting %s\n", ioStatusName.at(status).c_str());
  }
}

void TempSetter::mainLoop()
{
  uint32_t tempOffset = 0;
  while (doProcessing.load())
  {
    LOGD("TempSetter::mainLoop: loop rolling\n");
    float temp = 21 + (tempOffset++ & 1);

    LOGI("TempSetter::mainLoop: Setting day temperature target: %2.1f\n", temp);
    auto status = paramSetter->set(ParamId::TEMP_ROOM_DAY_NORMAL_TARGET, temp);

    checkStatus(status);
    std::this_thread::sleep_for(std::chrono::milliseconds(tempWriteInterval));
  }
}
