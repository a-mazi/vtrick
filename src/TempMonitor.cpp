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
#include <Log.h>

TempMonitor::TempMonitor(const ParamGeneratorPtr& paramGenerator_, const ParamReaderPtr& paramReader_) :
  paramGenerator{paramGenerator_},
  paramReader{paramReader_},
  status{IoStatus::error},
  doProcessing{false}
{
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

void TempMonitor::statusCb(IoStatus status_)
{
  {
    std::lock_guard<std::mutex> paramReadyLock{paramReadyControl};
    status = status_;
    LOGD("TempMonitor::statusCb: Callback received, code = %d\n", (int)status_);
  }
  paramReady.notify_all();
}

void TempMonitor::checkParamValue(const char* paramString, float value)
{
  if (status == IoStatus::ok)
  {
    LOGI("TempMonitor::checkParamValue: %s: %2.1f\n", paramString, value);
  }
  else if (status == IoStatus::error)
  {
    LOGE("TempMonitor::checkParamValue: %s read failure\n", paramString);
  }
  else if (status == IoStatus::timeout)
  {
    LOGE("TempMonitor::checkParamValue: %s read timeout\n", paramString);
  }
  else
  {
    LOGE("TempMonitor::checkParamValue: %s read unknown state\n", paramString);
  }
}

void TempMonitor::mainLoop()
{
  while (doProcessing.load())
  {
    LOGD("TempMonitor::mainLoop: loop rolling\n");

    status = IoStatus::error;
    auto param = paramGenerator->generate(ParamId::TEMP_KETTLE_CURRENT);
    if (param == nullptr)
    {
      doProcessing.store(false);
      break;
    }

    std::unique_lock<std::mutex> paramReadyLock{paramReadyControl};
    paramReader->read(param, this);
    paramReady.wait(paramReadyLock);

    checkParamValue("Kettle temperature", param->getValue());

    status = IoStatus::error;
    param = paramGenerator->generate(ParamId::TEMP_ROOM_DAY_NORMAL_TARGET);
    if (param == nullptr)
    {
      doProcessing.store(false);
      break;
    }

    paramReader->read(param, this);
    paramReady.wait(paramReadyLock);

    checkParamValue("Day temperature target", param->getValue());

    std::this_thread::sleep_for(std::chrono::milliseconds(tempReadInterval));
  }
}
