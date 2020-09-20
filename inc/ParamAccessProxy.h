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
#include <mutex>
#include <thread>
#include <queue>
#include <chrono>
#include <map>
#include <ParamReader.h>
#include <ParamWriter.h>
#include <ParamReadWriteCallback.h>

class ParamAccessProxy : public ParamReader, public ParamWriter, public ParamReadWriteCallback
{
public:
  struct ParamQueue;
  struct ParamHistory;

  ParamAccessProxy() = delete;
  ParamAccessProxy(const ParamReaderPtr& paramReader, const ParamWriterPtr& paramWriter,
                   int64_t retentionTime = 1000 /* milliseconds */);
  ~ParamAccessProxy() = default;

  void read(std::shared_ptr<ParamBody>& paramBody, ParamReadWriteCallback* callback) final;
  void write(std::shared_ptr<ParamBody>& paramBody, ParamReadWriteCallback* callback) final;
  void statusCb(IoStatus status) final;

private:
  ParamReaderPtr paramReader;
  ParamWriterPtr paramWriter;

  std::mutex synchronizer;
  std::queue<ParamQueue> paramQueue;
  std::map<uint16_t, ParamHistory> paramMemory;

  int64_t retentionTime;
};

struct ParamAccessProxy::ParamQueue
{
  std::shared_ptr<ParamBody> paramBody;
  ParamReadWriteCallback* callback;
};

struct ParamAccessProxy::ParamHistory
{
  ParamBody paramBody;
  std::chrono::high_resolution_clock::time_point updateTimes;
};
