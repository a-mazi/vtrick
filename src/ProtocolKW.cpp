#include <cstring>
#include <iostream>
#include <ostream>
#include <future>
#include <chrono>
#include <thread>
#include <ProtocolKW.h>

ProtocolKW::ProtocolKW() :
  doProcessing{false},
  mainLoopThread{nullptr},
  paramQueueBeg{0},
  paramQueueEnd{0}
{
}

ProtocolKW::~ProtocolKW()
{
  stop();
}

void ProtocolKW::start(std::string serialPortName)
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (mainLoopThread.get() == nullptr)
  {
    serialPortFile.open(serialPortName, std::ios::in | std::ios::out | std::ios::binary);
    doProcessing.store(true);
    mainLoopThread = std::unique_ptr<std::thread>{new std::thread{&ProtocolKW::mainLoop, this}};
  }
}

void ProtocolKW::stop()
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (mainLoopThread.get() != nullptr)
  {
    doProcessing.store(false);
    mainLoopThread->join();
    mainLoopThread.reset();
    serialPortFile.close();
  }
}

void ProtocolKW::readParam(ParamBody* paramBody, ParamReadWriteCallback* callback)
{
  addToQueue(Action::READ, paramBody, callback);
}

void ProtocolKW::writeParam(ParamBody* paramBody, ParamReadWriteCallback* callback)
{
  addToQueue(Action::WRITE, paramBody, callback);
}

void ProtocolKW::addToQueue(Action action_, ParamBody* paramBody_, ParamReadWriteCallback* callback_)
{
  std::unique_lock<std::mutex> sameDataAccessControlLock{sameDataAccessControl, std::defer_lock};
  if ((paramQueueEnd & paramQueueMask) == (paramQueueBeg & paramQueueMask))
  {
      sameDataAccessControlLock.lock();
  }
  action[paramQueueEnd & paramQueueMask] = action_;
  paramBody[paramQueueEnd & paramQueueMask] = paramBody_;
  callback[paramQueueEnd & paramQueueMask] = callback_;

  if (sameDataAccessControlLock)
  {
      sameDataAccessControlLock.unlock();
  }
  std::lock_guard<std::mutex> paramQueueEndControlLock{paramQueueEndControl};
  paramQueueEnd++;
}

bool ProtocolKW::isSynchroEventDetected()
{
  serialPortFile.read(datagramBuffer, 1);
  return (*datagramBuffer == synchroByte);
}

void ProtocolKW::ackSynchroEvent()
{
  serialPortFile.write(&synchroAckByte, 1);
}

ParamReadWriteCallback::Status ProtocolKW::sendParamReadDatagram(ParamBody* paramBody)
{
  uint32_t paramAddress        = paramBody->getParamAddress();
  size_t   paramBodyDataSize   = paramBody->getDataSize();
  char*    paramBodyDataBuffer = paramBody->getDataBuffer().get();
  size_t   datagramBufferFill  = 0;

  datagramBuffer[datagramBufferFill++] =  readTransissionTypeByte;
  datagramBuffer[datagramBufferFill++] = (paramAddress | 0xFF00) >> 8;
  datagramBuffer[datagramBufferFill++] =  paramAddress | 0x00FF;
  datagramBuffer[datagramBufferFill++] =  paramBodyDataSize;

  serialPortFile.write(datagramBuffer, datagramBufferFill);
  serialPortFile.read(paramBodyDataBuffer, paramBodyDataSize);

  return ParamReadWriteCallback::Status::OK;
}

ParamReadWriteCallback::Status ProtocolKW::sendParamWriteDatagram(ParamBody* paramBody)
{
  uint32_t paramAddress        = paramBody->getParamAddress();
  size_t   paramBodyDataSize   = paramBody->getDataSize();
  char*    paramBodyDataBuffer = paramBody->getDataBuffer().get();
  size_t   datagramBufferFill  = 0;

  datagramBuffer[datagramBufferFill++] =  readTransissionTypeByte;
  datagramBuffer[datagramBufferFill++] = (paramAddress | 0xFF00) >> 8;
  datagramBuffer[datagramBufferFill++] =  paramAddress | 0x00FF;
  datagramBuffer[datagramBufferFill++] =  paramBodyDataSize;
  datagramBufferFill += paramBodyDataSize;
  if (datagramBufferFill > maxDatagramBufferSize)
  {
    return ParamReadWriteCallback::Status::PARAM_DATA_SIZE_TO_BIG;
  }
  memcpy(datagramBuffer, paramBodyDataBuffer, paramBodyDataSize);

  serialPortFile.write(datagramBuffer, datagramBufferFill);
  serialPortFile.read(paramBodyDataBuffer, 1);

  return ParamReadWriteCallback::Status::OK;
}

void ProtocolKW::mainLoop()
{
  while (doProcessing.load())
  {
    bool doQueueChecking{false};
    std::future<bool> isSynchroDetectedFuture{std::async(&ProtocolKW::isSynchroEventDetected, this)};
    if (isSynchroDetectedFuture.wait_for(std::chrono::seconds(synchoEventWaitTime)) ==
        std::future_status::ready)
    {
      doQueueChecking = isSynchroDetectedFuture.get();
    }

    if (doQueueChecking)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(synchoEventAckWaitTime));
      ackSynchroEvent();
      int datagramBurstCount{0};

      std::unique_lock<std::mutex> paramQueueEndControlLock{paramQueueEndControl};
      unsigned int sdkMessageQueueActualSize = paramQueueEnd - paramQueueBeg;
      while ((sdkMessageQueueActualSize > 0) && (datagramBurstCount <= maxDatagramBurstCount))
      {
        if (sdkMessageQueueActualSize > paramQueueSize)
        {
            paramQueueBeg = paramQueueEnd - paramQueueSize;
            std::cout << "ProtocolKW::Lost the following number of queue elements: "
                      << sdkMessageQueueActualSize - paramQueueSize << std::endl;
        }
        paramQueueEndControlLock.unlock();

        ParamReadWriteCallback::Status operationStatus{ParamReadWriteCallback::Status::NO_ACTION};
        if (action[paramQueueEnd & paramQueueMask] == Action::READ)
        {
          operationStatus = sendParamReadDatagram(paramBody[paramQueueEnd & paramQueueMask]);
        }
        else if (action[paramQueueEnd & paramQueueMask] == Action::WRITE)
        {
          operationStatus = sendParamWriteDatagram(paramBody[paramQueueEnd & paramQueueMask]);
        }
        callback[paramQueueEnd & paramQueueMask]->statusCb(operationStatus);

        datagramBurstCount++;
        paramQueueBeg++;
        paramQueueEndControlLock.lock();
        sdkMessageQueueActualSize = paramQueueEnd - paramQueueBeg;
      }
      paramQueueEndControlLock.unlock();
    }
  }
}


























