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
#include <P300Protocol.h>
#include <cassert>
#include <Log.h>

P300Protocol::P300Protocol(const P300PacketGeneratorPtr& packetGenerator_, const KettlePortPtr& kettle_) :
  packetGenerator{packetGenerator_},
  kettle{kettle_},
  doProcessing{false}
{
  assert(packetGenerator);
  assert(kettle);
}

P300Protocol::~P300Protocol()
{
  stop();
}

IoStatus P300Protocol::init()
{
  auto prompt = packetGenerator->generate();
  auto ioStatus = prompt->receive([&](void* buffer, size_t size){return kettle->read(buffer, size, initIoTimeout);});
  if (ioStatus == IoStatus::error)
  {
    return ioStatus;
  }
  else if (ioStatus == IoStatus::timeout)
  {
    LOGI("P300Protocol::init: Init prompt not received. Must be already initialized.\n");
    return IoStatus::ok;
  }
  LOGI("P300Protocol::init: Init prompt received. Sending init request.\n");

  auto initRequest = packetGenerator->generate();
  initRequest->prepareInitRequest();
  ioStatus = initRequest->send([&](const void* buffer, size_t size){return kettle->write(buffer, size);});
  if (ioStatus != IoStatus::ok)
  {
    return ioStatus;
  }

  auto& response = prompt;
  for (int checkNo = 0; checkNo < initResponseChecks; checkNo++)
  {
    ioStatus = response->receive([&](void* buffer, size_t size){return kettle->read(buffer, size, initIoTimeout);});
    if ((ioStatus == IoStatus::error) ||
        ((ioStatus == IoStatus::ok) && (response->getType() == P300Packet::Type::acknowledge)))
    {
      return ioStatus;
    }
    LOGI("P300Protocol::init: Init request not acknowledged! Check no.: %d\n", checkNo + 1);
  }
  LOGE("P300Protocol::init: Init request not acknowledged!\n");
  return IoStatus::error;
}

void P300Protocol::start()
{
  std::lock_guard<std::mutex> processingLock{processingControl};
  if (!mainLoopThread.joinable())
  {
    doProcessing.store(true);
    mainLoopThread = std::thread{&P300Protocol::mainLoop, this};
  }
}

void P300Protocol::stop()
{
  std::lock_guard<std::mutex> processingLock{processingControl};
  if (mainLoopThread.joinable())
  {
    doProcessing.store(false);
    mainLoopThread.join();
  }
}

void P300Protocol::read(const ParamBodyPtr& paramBody, const ParamReadWriteCallbackPtr& callback)
{
  assert(paramBody);
  addTaskToQueue(Action::read, paramBody, callback);
}

void P300Protocol::write(const ParamBodyPtr& paramBody, const ParamReadWriteCallbackPtr& callback)
{
  assert(paramBody);
  addTaskToQueue(Action::write, paramBody, callback);
}

void P300Protocol::addTaskToQueue(Action action, const ParamBodyPtr& paramBody, const ParamReadWriteCallbackPtr& callback)
{
  {
    std::lock_guard<std::mutex> taskQueueLock{taskQueueControl};
    taskQueue.push(Task{action, paramBody, callback});
  }
  taskReady.notify_all();
}

IoStatus P300Protocol::readParam(const ParamBodyPtr& paramBody)
{
  auto request = packetGenerator->generate();
  auto response = packetGenerator->generate();

  if (!paramBody->isReadable())
  {
    LOGE("P300Protocol::readParam: Parameter 0x%04X is not readable!\n", paramBody->getParamAddress());
    return IoStatus::error;
  }

  request->prepareReadRequest(paramBody->getParamAddress(), paramBody->getParamSize());
  auto ioStatus = sendReceive(request, response);
  if (ioStatus == IoStatus::ok)
  {
    paramBody->setRawValue(response->getParamValue());
  }

  return ioStatus;
}

IoStatus P300Protocol::writeParam(const ConstParamBodyPtr& paramBody)
{
  auto request = packetGenerator->generate();
  auto response = packetGenerator->generate();

  if (!paramBody->isWriteable())
  {
    LOGE("P300Protocol::readParam: Parameter 0x%04X is not writeable!\n", paramBody->getParamAddress());
    return IoStatus::error;
  }

  request->prepareWriteRequest(paramBody->getParamAddress(), paramBody->getParamSize(), paramBody->getRawValue());
  auto ioStatus = sendReceive(request, response);

  return ioStatus;
}

IoStatus P300Protocol::sendReceive(const ConstP300PacketPtr& request, const P300PacketPtr& response)
{
  IoStatus ioStatus;

  ioStatus = request->send(std::bind(&KettlePort::write, kettle.get(), std::placeholders::_1, std::placeholders::_2));
  if (ioStatus != IoStatus::ok)
  {
    return ioStatus;
  }

  ioStatus = response->receive([&](void* buffer, size_t size){return kettle->read(buffer, size, serialReadTimeout);});
  if (ioStatus != IoStatus::ok)
  {
    return ioStatus;
  }
  if (response->getType() != P300Packet::Type::acknowledge)
  {
    LOGE("P300Protocol::readParam: Request not acknowledged!\n");
    return IoStatus::error;
  }

  ioStatus = response->receive([&](void* buffer, size_t size){return kettle->read(buffer, size, serialReadTimeout);});
  if (ioStatus != IoStatus::ok)
  {
    return ioStatus;
  }
  if (response->getType() != P300Packet::Type::userData)
  {
    LOGE("P300Protocol::readParam: Bad message type received!\n");
    return IoStatus::error;
  }
  if (response->getDirection() != P300Packet::Direction::response)
  {
    LOGE("P300Protocol::readParam: No valid response received!\n");
    return IoStatus::error;
  }
  if (request->getParamAddress() != response->getParamAddress())
  {
    LOGE("P300Protocol::readParam: Response to wrong request received!\n");
    return IoStatus::error;
  }
  if (response->calculateChecksum() != response->getChecksum())
  {
    LOGE("P300Protocol::readParam: Wrong response checksum!\n");
    return IoStatus::error;
  }
  if (request->getParamSize() != response->getParamSize())
  {
    LOGW("P300Protocol::readParam: Parameter data size in request differs form parameter data size in response.\n");
  }

  return ioStatus;
}

void P300Protocol::mainLoop()
{
  while (doProcessing.load())
  {
    LOGD("P300Protocol::mainLoop: loop rolling\n");
    std::unique_lock<std::mutex> taskQueueLock{taskQueueControl};
    taskReady.wait_for(taskQueueLock, std::chrono::milliseconds(taskWaitTime));

    while (taskQueue.size() > 0)
    {
      auto task = taskQueue.front();
      taskQueue.pop();
      taskQueueLock.unlock();

      auto IoStatus = IoStatus::error;
      if (task.action == Action::read)
      {
        IoStatus = readParam(task.paramBody);
      }
      else if (task.action == Action::write)
      {
        IoStatus = writeParam(task.paramBody);
      }
      auto callback = task.callbackLink.lock();
      if (callback)
      {
        callback->statusCb(IoStatus);
      }

      taskQueueLock.lock();
    }
  }
}
