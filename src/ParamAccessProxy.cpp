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
#include <ParamAccessProxy.h>
#include <cassert>
#include <Log.h>

ParamAccessProxy::ParamAccessProxy(std::shared_ptr<ParamReader> paramReader_,
                                   std::shared_ptr<ParamWriter> paramWriter_,
                                   int64_t retentionTime_) :
  paramReader{paramReader_},
  paramWriter{paramWriter_},
  retentionTime{retentionTime_}
{
}

void ParamAccessProxy::read(std::shared_ptr<ParamBody> paramBody, ParamReadWriteCallback* callback)
{
  std::lock_guard<std::mutex> synchoLock{synchronizer};
  auto paramAddress = paramBody->getParamAddress();
  if (paramMemory.count(paramAddress) > 0)
  {
    auto paramHistory = paramMemory.at(paramAddress);
    auto currentTime = std::chrono::high_resolution_clock::now();
    int64_t elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - paramHistory.updateTimes).count();
    if (elapsedTime <= retentionTime)
    {
      LOGD("ParamAccessProxy::read: Read param %04X from history\n", paramAddress);
      *paramBody = paramHistory.paramBody;
      auto callbackThead = std::thread{&ParamReadWriteCallback::statusCb, callback, IoStatus::ok};
      callbackThead.detach();
      return;
    }
  }
  paramQueue.push({paramBody, callback});
  paramReader->read(paramBody, this);
}

void ParamAccessProxy::write(std::shared_ptr<ParamBody> paramBody, ParamReadWriteCallback* callback)
{
  std::lock_guard<std::mutex> synchoLock{synchronizer};
  paramQueue.push({paramBody, callback});
  paramWriter->write(paramBody, this);
}

void ParamAccessProxy::statusCb(IoStatus status)
{
  std::lock_guard<std::mutex> synchoLock{synchronizer};
  assert(paramQueue.size() > 0);
  auto param = paramQueue.front();
  paramQueue.pop();

  if (status == IoStatus::ok)
  {
    uint16_t     paramAddress = param.paramBody->getParamAddress();
    auto         currentTime = std::chrono::high_resolution_clock::now();
    ParamHistory paramHistory{ParamBody{*param.paramBody}, currentTime};
    if (paramMemory.count(paramAddress) > 0)
    {
      paramMemory.at(paramAddress) = paramHistory;
    }
    else
    {
      paramMemory.insert({paramAddress, paramHistory});
    }
  }
  param.callback->statusCb(status);
}
