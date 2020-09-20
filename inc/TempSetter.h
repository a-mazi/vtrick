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

#include <thread>
#include <atomic>
#include <condition_variable>
#include <Manipulator.h>
#include <ParamWriter.h>
#include <ParamGenerator.h>

class TempSetter : public Manipulator, public ParamReadWriteCallback
{
public:
  TempSetter() = delete;
  TempSetter(const ParamGeneratorPtr& paramGenerator, const ParamWriterPtr& paramWriter);
  ~TempSetter() = default;

  void start() final;
  void stop() final;
  void statusCb(IoStatus status) final;

private:
  ParamGeneratorPtr paramGenerator;
  ParamWriterPtr paramWriter;
  std::condition_variable paramReady;
  std::mutex paramReadyControl;
  IoStatus status;

  std::mutex processingControl;
  std::atomic_bool doProcessing;
  std::thread mainLoopThread;

  static constexpr int tempWriteInterval = 3000; // in milliseconds

  void checkStatus();
  void mainLoop();
};
