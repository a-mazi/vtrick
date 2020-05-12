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
#ifndef KETTLEPORT_H_
#define KETTLEPORT_H_

#include <string>
#include <memory>
#include <IoStatus.h>
#include <SerialPort.h>

class KettlePort
{
public:
  enum class Status;

  KettlePort();
  ~KettlePort() = default;

  void attachSerial(std::shared_ptr<SerialPort> serial);
  void detachSerial();

  void open();
  void close();
  IoStatus read(void* buffer, size_t dataSize, int timeoutmsec = 0);
  IoStatus write(const void* buffer, size_t dataSize);

private:
  std::shared_ptr<SerialPort> serial;
  static constexpr int readRetryIntervalmsec = 100;
  std::shared_ptr<char[]> internalBuffer;
  size_t internalBufferSize;
};

#endif // KETTLEPORT_H_
