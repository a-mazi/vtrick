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
#ifndef KETTLEMOCK_H_
#define KETTLEMOCK_H_

#include <gmock/gmock.h>
#include <KettlePort.h>

class KettleMock : public KettlePort
{
public:
  MOCK_METHOD(void, open, (), (final));
  MOCK_METHOD(void, close, (), (final));
  MOCK_METHOD(IoStatus, read, (void*, size_t, int), (final));
  MOCK_METHOD(IoStatus, write, (const void*, size_t), (final));
};

#endif // KETTLEMOCK_H_
