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
#ifndef P300PROTOCOL_H_
#define P300PROTOCOL_H_

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
  P300Protocol(std::shared_ptr<P300PacketGenerator> packetGenerator, std::shared_ptr<KettlePort> kettle);
  ~P300Protocol();

  IoStatus init();
  void start();
  void stop();
  void read(std::shared_ptr<ParamBody> paramBody, ParamReadWriteCallback* callback) final;
  void write(std::shared_ptr<ParamBody> paramBody, ParamReadWriteCallback* callback) final;

private:
  enum class Action;
  struct Task;

  static constexpr int initIoTimeout = 4000; // in milliseconds
  static constexpr int initResponseChecks = 3;
  static constexpr int serialReadTimeout = 2000; // in milliseconds

  std::shared_ptr<P300PacketGenerator> packetGenerator;
  std::shared_ptr<KettlePort> kettle;

  std::mutex processingControl;
  std::atomic_bool doProcessing;
  std::thread mainLoopThread;

  std::queue<Task> taskQueue;
  std::mutex taskQueueControl;
  std::condition_variable taskReady;
  static constexpr int taskWaitTime = 500; // in milliseconds

  void addTaskToQueue(Action action, std::shared_ptr<ParamBody> paramBody, ParamReadWriteCallback* callback);

  IoStatus readParam(std::shared_ptr<ParamBody> paramBody);
  IoStatus writeParam(std::shared_ptr<ParamBody> paramBody);
  IoStatus sendReceive(const std::shared_ptr<P300Packet> request, std::shared_ptr<P300Packet> response);

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
  std::shared_ptr<ParamBody> paramBody;
  ParamReadWriteCallback* callback;
};

#endif // P300PROTOCOL_H_
