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
#include <ParamReader.h>
#include <ParamGenerator.h>

class TempMonitor : public Manipulator, public ParamReadWriteCallback
{
public:
  TempMonitor() = delete;
  TempMonitor(const ParamGeneratorPtr& paramGenerator, const ParamReaderPtr& paramReader);
  ~TempMonitor() = default;

  void start() final;
  void stop() final;
  void statusCb(IoStatus status) final;

private:
  ParamGeneratorPtr paramGenerator;
  ParamReaderPtr paramReader;
  std::condition_variable paramReady;
  std::mutex paramReadyControl;
  IoStatus status;

  std::mutex processingControl;
  std::atomic_bool doProcessing;
  std::thread mainLoopThread;

  static constexpr int tempReadInterval = 3000; // in milliseconds

  void checkParamValue(const char* paramString, float value);
  void mainLoop();
};
