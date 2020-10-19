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
#pragma once

#include <memory>
#include <functional>
#include <IoStatus.h>

class P300Packet
{
public:
  enum class Type : uint8_t;
  enum class Direction : uint8_t;
  enum class Operation : uint8_t;

  P300Packet();

  void prepareInitRequest();
  void prepareReadRequest(uint16_t paramAddress, uint8_t paramSize);
  void prepareWriteRequest(uint16_t paramAddress, uint8_t paramSize, int32_t paramValue);

  IoStatus send(std::function<IoStatus(const void*, size_t)> sink) const;
  IoStatus receive(std::function<IoStatus(void*, size_t)> source);

  Type      getType() const;
  Direction getDirection() const;
  uint16_t  getParamAddress() const;
  uint8_t   getParamSize() const;
  int32_t   getParamValue() const;
  uint8_t   getChecksum() const;
  uint8_t   calculateChecksum() const;

private:
  static constexpr size_t maxBufferSize = 12;
  static constexpr size_t typeOffset         = 0;
  static constexpr size_t dataSizeOffset     = 1;
  static constexpr size_t directionOffset    = 2;
  static constexpr size_t operationOffset    = 3;
  static constexpr size_t paramAddressOffset = 4;
  static constexpr size_t paramSizeOffset    = 6;
  static constexpr size_t paramValueOffset   = 7;

  uint8_t buffer[maxBufferSize];

  IoStatus receiveHeader(std::function<IoStatus(void*, size_t)> source);

  uint8_t getDataSize() const;
  void setType(Type type);
  void setDataSize(uint8_t dataSize);
  void setDirection(Direction direction);
  void setOperation(Operation operation);
  void setParamAddress(uint16_t paramAddress);
  void setParamSize(uint8_t paramSize);
  void setParamValue(uint8_t paramSize, int32_t paramValue);
  void setChecksum(uint8_t dataSize, uint8_t checksum);

  static uint32_t swapEndian(uint32_t value,  size_t valueSize);
  static void     writeLittleEndian(uint8_t* buffer, size_t valueSize, uint32_t value);
  static uint32_t readLittleEndian(const uint8_t* buffer, size_t valueSize);
};

using P300PacketPtr = std::shared_ptr<P300Packet>;
using ConstP300PacketPtr = std::shared_ptr<const P300Packet>;

enum class P300Packet::Type : uint8_t
{
  empty        = 0x00,
  initPrompt   = 0x05,
  initRequest  = 0x16,
  userData     = 0x41,
  acknowledge  = 0x06,
  error        = 0x15,
};

enum class P300Packet::Direction : uint8_t
{
  request      = 0x00,
  response     = 0x01,
  error        = 0x03,
};

enum class P300Packet::Operation : uint8_t
{
  dataRead     = 0x01,
  dataWrite    = 0x02,
  functionCall = 0x07,
};
