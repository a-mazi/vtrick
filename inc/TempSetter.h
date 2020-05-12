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
#ifndef TEMPSETTER_H_
#define TEMPSETTER_H_

#include <memory>
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
  TempSetter(std::shared_ptr<const ParamGenerator> paramGenerator, std::shared_ptr<ParamWriter> paramWriter);
  ~TempSetter() = default;

  void start() final;
  void stop() final;
  void statusCb(IoStatus status) final;

private:
  std::shared_ptr<const ParamGenerator> paramGenerator;
  std::shared_ptr<ParamWriter> paramWriter;
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

#endif // TEMPSETTER_H_
