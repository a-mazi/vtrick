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

#include <gmock/gmock.h>
#include <SerialPort.h>

class SerialMock : public SerialPort
{
public:
  MOCK_METHOD(void, openSerial, (), (final));
  MOCK_METHOD(void, closeSerial, (), (final));
  MOCK_METHOD(ssize_t, readSerial, (void*, size_t), (final));
  MOCK_METHOD(ssize_t, writeSerial, (const void*, size_t), (final));
};
