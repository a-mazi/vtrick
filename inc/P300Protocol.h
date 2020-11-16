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

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <queue>
#include <IoStatus.h>
#include <P300PacketGenerator.h>
#include <ParamReader.h>
#include <ParamWriter.h>
#include <KettlePort.h>

class P300Protocol : public ParamReader, public ParamWriter
{
public:
  P300Protocol() = delete;
  P300Protocol(const P300PacketGeneratorPtr& packetGenerator, const KettlePortPtr& kettle);
  ~P300Protocol();

  IoStatus init();
  void start();
  void stop();
  void read(const ParamBodyPtr& paramBody, const ParamReadWriteCallbackPtr& callback) final;
  void write(const ParamBodyPtr& paramBody, const ParamReadWriteCallbackPtr& callback) final;

private:
  enum class Action;
  struct Task;

  static constexpr int initIoTimeout = 4000; // in milliseconds
  static constexpr int initResponseChecks = 3;
  static constexpr int serialReadTimeout = 2000; // in milliseconds

  P300PacketGeneratorPtr packetGenerator;
  KettlePortPtr kettle;

  std::mutex processingControl;
  std::atomic_bool doProcessing;
  std::thread mainLoopThread;

  std::queue<Task> taskQueue;
  std::mutex taskQueueControl;
  std::condition_variable taskReady;
  static constexpr int taskWaitTime = 500; // in milliseconds

  void addTaskToQueue(Action action, const ParamBodyPtr& paramBody, const ParamReadWriteCallbackPtr& callback);

  IoStatus readParam(const ParamBodyPtr& paramBody);
  IoStatus writeParam(const ConstParamBodyPtr& paramBody);
  IoStatus sendReceive(const ConstP300PacketPtr& request, const P300PacketPtr& response);

  void mainLoop();
};

enum class P300Protocol::Action
{
  read,
  write,
};

struct P300Protocol::Task
{
  Action action;
  ParamBodyPtr paramBody;
  ParamReadWriteCallbackWeakPtr callbackLink;
};
