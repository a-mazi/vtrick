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
#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#include <string>

class SerialPort
{
public:
  SerialPort();
  SerialPort(std::string serialPortName);
  ~SerialPort() = default;

  virtual void openSerial();
  virtual void closeSerial();
  virtual ssize_t readSerial(void* buffer, size_t dataSize);
  virtual ssize_t writeSerial(const void* buffer, size_t dataSize);

private:
  std::string serialPortName;
  int         serialPortFD;
};

#endif // SERIALPORT_H_
