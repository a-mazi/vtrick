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
#include <ParamGetter.h>

class TempMonitor
{
public:
  TempMonitor() = delete;
  TempMonitor(ParamGetterPtr&& paramGetter);

  void start();
  void stop();

private:
  ParamGetterPtr paramGetter;

  std::mutex processingControl;
  std::atomic_bool doProcessing;
  std::thread mainLoopThread;

  static constexpr int tempReadInterval = 3000; // in milliseconds

  void checkParamValue(const char* paramString, ParamGetter::Result result);
  void mainLoop();
};
