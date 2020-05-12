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
#include <cstring>
#include <memory>

#include <KettlePort.h>

#include <gtest/gtest.h>
#include <ut/SerialMock.h>

using namespace ::testing;

struct KettlePort_Fixture : public Test
{
  void SetUp() override
  {
    serialMock = std::make_shared<SerialMock>();
    kettlePort = std::make_unique<KettlePort>();
    kettlePort->attachSerial(serialMock);
  }

  std::shared_ptr<SerialMock> serialMock;
  std::unique_ptr<KettlePort> kettlePort;
};

TEST_F(KettlePort_Fixture, readOk_Test)
{
  static constexpr size_t testDataSize = 13;
  static constexpr char const* testData = "asdertopMna#";
  static constexpr size_t bufferSize = 15;
  char buffer[bufferSize];

  EXPECT_CALL(*serialMock, readSerial(_, testDataSize)).WillOnce([&](void* buffer, size_t dataSize)
  {
    memcpy(buffer, testData, dataSize);
    return dataSize;
  });

  auto readStatus = kettlePort->read(buffer, testDataSize);

  ASSERT_EQ(readStatus, IoStatus::ok);
  ASSERT_EQ(strcmp(buffer, testData), 0);
  Mock::VerifyAndClearExpectations(serialMock.get());
}

TEST_F(KettlePort_Fixture, readNoDataThenOk_Test)
{
  static constexpr size_t testDataSize = 13;
  static constexpr char const* testData = "asdertopMna#";
  static constexpr size_t bufferSize = 15;
  char buffer[bufferSize];

  {
    InSequence seq;

    EXPECT_CALL(*serialMock, readSerial(_, testDataSize)).WillOnce([&](void* buffer, size_t dataSize)
    {
      return 0;
    });

    EXPECT_CALL(*serialMock, readSerial(_, testDataSize)).WillOnce([&](void* buffer, size_t dataSize)
    {
      memcpy(buffer, testData, dataSize);
      return dataSize;
    });
  }

  auto readStatus = kettlePort->read(buffer, testDataSize, 101);

  ASSERT_EQ(readStatus, IoStatus::ok);
  ASSERT_EQ(strcmp(buffer, testData), 0);
  Mock::VerifyAndClearExpectations(serialMock.get());
}
