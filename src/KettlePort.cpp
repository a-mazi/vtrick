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
#include <KettlePort.h>
#include <Log.h>
#include <cassert>
#include <cstring>
#include <chrono>
#include <thread>

// LCOV_EXCL_START
KettlePort::KettlePort() :
  serial{},
  internalBuffer{},
  internalBufferSize{0}
{
}

void KettlePort::attachSerial(const SerialPortPtr& serial_)
{
  serial = serial_;
}

void KettlePort::detachSerial()
{
  serial.reset();
}

void KettlePort::open()
{
  if (serial)
  {
    serial->openSerial();
  }
}

void KettlePort::close()
{
  if (serial)
  {
    serial->closeSerial();
  }
}
// LCOV_EXCL_STOP

IoStatus KettlePort::read(void* buffer, size_t dataSize, int timeoutmsec)
{
  size_t dataPointer{0};
  int readTimeElapsedmsec{0};

  if (internalBufferSize > 0)
  {
    if (internalBufferSize <= dataSize)
    {
      memcpy(buffer, internalBuffer.get(), internalBufferSize);
      dataPointer = internalBufferSize;

      internalBufferSize = 0;
      internalBuffer.reset();
    }
    else
    {
      memcpy(buffer, internalBuffer.get(), dataSize);
      dataPointer = dataSize;

      auto remainingInternalDataSize = internalBufferSize - dataSize;
      auto remainingInternalData = std::shared_ptr<char[]>(new char[remainingInternalDataSize]);
      memcpy(remainingInternalData.get(), internalBuffer.get() + dataSize, remainingInternalDataSize);
      internalBuffer = remainingInternalData;
      internalBufferSize = remainingInternalDataSize;

      return IoStatus::ok;
    }
  }

  while ((dataSize - dataPointer) > 0)
  {
    auto dataRead = serial->readSerial(static_cast<char*>(buffer) + dataPointer, dataSize - dataPointer);
    if (dataRead < 0)
    {
      LOGE("KettlePort::read: Serial device returned error!\n");
      return IoStatus::error;
    }
    dataPointer += dataRead;

    assert(dataPointer <= dataSize);
    if (dataPointer == dataSize)
    {
      return IoStatus::ok;
    }
    else
    {
      if (readTimeElapsedmsec <= (timeoutmsec - readRetryIntervalmsec))
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(readRetryIntervalmsec));
        readTimeElapsedmsec += readRetryIntervalmsec;
      }
      else
      {
        break;
      }
    }
  }

  if (dataPointer != 0)
  {
    internalBuffer = std::shared_ptr<char[]>(new char[dataPointer]);
    memcpy(internalBuffer.get(), buffer, dataPointer);
  }

  return IoStatus::timeout;
}

// LCOV_EXCL_START
IoStatus KettlePort::write(const void* buffer, size_t dataSize)
{
  if (serial)
  {
    auto dataWrite = serial->writeSerial(buffer, dataSize);
    if (static_cast<size_t>(dataWrite) != dataSize)
    {
      LOGE("KettlePort::write: Wrote only %ld bytes of %ld requested!\n", dataWrite, dataSize);
      return IoStatus::error;
    }
  }
  return IoStatus::ok;
}
// LCOV_EXCL_STOP
