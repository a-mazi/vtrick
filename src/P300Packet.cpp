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
#include <P300Packet.h>
#include <IsPowerOfTwo.h>
#include <Log.h>
#include <cassert>

P300Packet::P300Packet()
{
  setType(Type::empty);
}

void P300Packet::prepareInitRequest()
{
  setType(P300Packet::Type::initRequest);
  setDataSize(0);
  setChecksum(0, calculateChecksum());
}

void P300Packet::prepareReadRequest(uint16_t paramAddress, uint8_t paramSize)
{
  setType(Type::userData);
  uint8_t dataSize = sizeof(Direction) + sizeof(Operation) + sizeof(paramAddress) + sizeof(paramSize);
  setDataSize(dataSize);
  setDirection(Direction::request);
  setOperation(Operation::dataRead);
  setParamAddress(swapEndian(paramAddress, sizeof(paramAddress)));
  setParamSize(paramSize);
  setChecksum(dataSize, calculateChecksum());
}

void P300Packet::prepareWriteRequest(uint16_t paramAddress, uint8_t paramSize, int32_t paramValue)
{
  setType(Type::userData);
  uint8_t dataSize = sizeof(Direction) + sizeof(Operation) + sizeof(paramAddress) + sizeof(paramSize) + paramSize;
  setDataSize(dataSize);
  setDirection(Direction::request);
  setOperation(Operation::dataWrite);
  setParamAddress(swapEndian(paramAddress, sizeof(paramAddress)));
  setParamSize(paramSize);
  setParamValue(paramSize, paramValue);
  setChecksum(dataSize, calculateChecksum());
}

IoStatus P300Packet::send(std::function<IoStatus(const void*, size_t)> sink) const
{
  auto ioStatus = IoStatus::error;
  size_t packetSize = sizeof(Type) + sizeof(uint8_t) + getDataSize() + sizeof(uint8_t);
  if (packetSize > maxBufferSize)
  {
    LOGE("P300Packet::send: Packet size larger than buffer!\n");
    return IoStatus::error;
  }
  ioStatus = sink(buffer, packetSize);
  return ioStatus;
}

IoStatus P300Packet::receive(std::function<IoStatus(void*, size_t)> source)
{
  auto ioStatus = IoStatus::error;

  ioStatus = receiveHeader(source);

  if ((ioStatus == IoStatus::ok) && (getType() == Type::userData))
  {
    ioStatus = source(buffer + dataSizeOffset, sizeof(uint8_t));

    if (ioStatus != IoStatus::ok)
    {
      return ioStatus;
    }
    if (getDataSize() > (maxBufferSize - sizeof(Type) - sizeof(uint8_t) - sizeof(uint8_t)))
    {
      LOGE("P300Packet::receive: Packet size larger than buffer!\n");
      return IoStatus::error;
    }

    size_t remainingPacketSize = getDataSize() + sizeof(uint8_t);
    ioStatus = source(buffer + directionOffset, remainingPacketSize);
  }
  return ioStatus;
}

P300Packet::Type P300Packet::getType() const
{
  return static_cast<Type>(buffer[typeOffset]);
}

P300Packet::Direction P300Packet::getDirection() const
{
  return static_cast<Direction>(buffer[directionOffset]);
}

uint16_t P300Packet::getParamAddress() const
{
  return static_cast<uint16_t>(readLittleEndian(buffer + paramAddressOffset, sizeof(uint16_t)));
}

uint8_t P300Packet::getParamSize() const
{
  return buffer[paramSizeOffset];
}

int32_t P300Packet::getParamValue() const
{
  return static_cast<int32_t>(readLittleEndian(buffer + paramValueOffset, getParamSize()));
}

uint8_t P300Packet::getChecksum() const
{
  size_t checksumOffset = directionOffset + getDataSize();
  return buffer[checksumOffset];
}

uint8_t P300Packet::calculateChecksum() const
{
  uint32_t checksum = 0;
  size_t checksumDataSize = sizeof(uint8_t) + getDataSize();
  for (size_t index = dataSizeOffset; index <= checksumDataSize; index++)
  {
    checksum += buffer[index];
  }
  return checksum;
}

IoStatus P300Packet::receiveHeader(std::function<IoStatus(void*, size_t)> source)
{
  for (size_t tryCount = 0; tryCount < maxBufferSize; tryCount++)
  {
    auto ioStatus = source(buffer + typeOffset, sizeof(Type));

    if (ioStatus == IoStatus::ok)
    {
      auto packetType = getType();
      if ((packetType == P300Packet::Type::initPrompt)  ||
          (packetType == P300Packet::Type::initRequest) ||
          (packetType == P300Packet::Type::userData)    ||
          (packetType == P300Packet::Type::acknowledge) ||
          (packetType == P300Packet::Type::error)       )
      {
        return ioStatus;
      }
      else
      {
        continue;
      }
    }
    else
    {
      return ioStatus;
    }
  }
  LOGE("P300Packet::receiveHeader: After %lu tries packet header couldn't be received!\n", maxBufferSize);
  return IoStatus::error;
}

uint8_t P300Packet::getDataSize() const
{
  return buffer[dataSizeOffset];
}

void P300Packet::setType(Type type)
{
  buffer[typeOffset] = static_cast<uint8_t>(type);
}

void P300Packet::setDataSize(uint8_t dataSize)
{
  buffer[dataSizeOffset] = dataSize;
}

void P300Packet::setDirection(Direction direction)
{
  buffer[directionOffset] = static_cast<uint8_t>(direction);
}

void P300Packet::setOperation(Operation operation)
{
  buffer[operationOffset] = static_cast<uint8_t>(operation);
}

void P300Packet::setParamAddress(uint16_t paramAddress)
{
  *reinterpret_cast<uint16_t*>(buffer + paramAddressOffset) = paramAddress;
}

void P300Packet::setParamSize(uint8_t paramSize)
{
  buffer[paramSizeOffset] = paramSize;
}

void P300Packet::setParamValue(uint8_t paramSize, int32_t paramValue)
{
  writeLittleEndian(buffer + paramValueOffset, paramSize, static_cast<uint32_t>(paramValue));
}

void P300Packet::setChecksum(uint8_t dataSize, uint8_t checksum)
{
  size_t checksumOffset = directionOffset + dataSize;
  buffer[checksumOffset] = checksum;
}

uint32_t P300Packet::swapEndian(uint32_t value, size_t valueSize)
{
  assert(valueSize <= sizeof(uint32_t));
  assert(isPowerOfTwo(valueSize));
  if (valueSize == 1)
  {
    return value;
  }
  else
  {
    size_t   halfSize       = valueSize / 2;
    uint32_t halfSizeInBits = halfSize * 8;
    uint32_t lsHalfMask     = (1UL << halfSizeInBits) - 1;
    uint32_t msHalfMask     = lsHalfMask << halfSizeInBits;

    uint32_t lsHalf = swapEndian( value & lsHalfMask,                    halfSize);
    uint32_t msHalf = swapEndian((value & msHalfMask) >> halfSizeInBits, halfSize);

    return (lsHalf << halfSizeInBits) + msHalf;
  }
}

void P300Packet::writeLittleEndian(uint8_t* buffer, size_t valueSize, uint32_t value)
{
  assert(valueSize <= sizeof(uint32_t));
  assert(isPowerOfTwo(valueSize));
  if (valueSize == 1)
  {
    *buffer = static_cast<uint8_t>(value);
  }
  else
  {
    size_t   halfSize       = valueSize / 2;
    uint32_t halfSizeInBits = halfSize * 8;
    uint32_t lsHalfMask     = (1UL << halfSizeInBits) - 1;
    uint32_t msHalfMask     = lsHalfMask << halfSizeInBits;

    writeLittleEndian(buffer,             value & lsHalfMask,                    halfSize);
    writeLittleEndian(buffer + halfSize, (value & msHalfMask) >> halfSizeInBits, halfSize);
  }
}

uint32_t P300Packet::readLittleEndian(const uint8_t* buffer, size_t valueSize)
{
  assert(valueSize <= sizeof(uint32_t));
  assert(isPowerOfTwo(valueSize));
  if (valueSize == 1)
  {
    return static_cast<uint32_t>(*buffer);
  }
  else
  {
    size_t   halfSize       = valueSize / 2;
    uint32_t halfSizeInBits = halfSize * 8;

    uint32_t lsHalf = readLittleEndian(buffer,            halfSize);
    uint32_t msHalf = readLittleEndian(buffer + halfSize, halfSize);

    return (msHalf << halfSizeInBits) + lsHalf;
  }
}
