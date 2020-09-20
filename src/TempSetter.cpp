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
#include <Log.h>

TempSetter::TempSetter(const ParamGeneratorPtr& paramGenerator_, const ParamWriterPtr& paramWriter_) :
  paramGenerator{paramGenerator_},
  paramWriter{paramWriter_},
  status{IoStatus::error},
  doProcessing{false}
{
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

void TempSetter::statusCb(IoStatus status_)
{
  {
    std::lock_guard<std::mutex> paramReadyLock{paramReadyControl};
    status = status_;
    LOGD("TempSetter::statusCb: Callback received, code = %d\n", (int)status_);
  }
  paramReady.notify_all();
}

void TempSetter::checkStatus()
{
  if (status == IoStatus::ok)
  {
    LOGI("TempSetter::checkStatus: Parameter setting success\n");
  }
  else if (status == IoStatus::error)
  {
    LOGE("TempSetter::checkStatus: Parameter setting failure\n");
  }
  else if (status == IoStatus::timeout)
  {
    LOGE("TempSetter::checkStatus: Parameter setting timeout\n");
  }
  else
  {
    LOGE("TempSetter::checkStatus: Parameter setting unknown state\n");
  }
}

void TempSetter::mainLoop()
{
  uint32_t tempOffset = 0;
  while (doProcessing.load())
  {
    LOGD("TempSetter::mainLoop: loop rolling\n");

    status = IoStatus::error;
    auto param = paramGenerator->generate(ParamId::TEMP_ROOM_DAY_NORMAL_TARGET);
    if (param == nullptr)
    {
      doProcessing.store(false);
      break;
    }

    float temp = 21 + (tempOffset++ & 1);
    param->setValue(temp);

    LOGI("TempSetter::mainLoop: Setting day temperature target: %2.1f\n", temp);
    std::unique_lock<std::mutex> paramReadyLock{paramReadyControl};
    paramWriter->write(param, this);
    paramReady.wait(paramReadyLock);

    checkStatus();

    std::this_thread::sleep_for(std::chrono::milliseconds(tempWriteInterval));
  }
}
