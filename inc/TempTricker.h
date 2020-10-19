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
#include <ParamGetter.h>
#include <ParamSetter.h>

class TempTricker
{
public:
  TempTricker() = delete;
  TempTricker(ParamGetterPtr&& paramGetter, ParamSetterPtr&& paramSetter);

  void start();
  void stop();

private:
  enum class HeatingState;

  ParamGetterPtr paramGetter;
  ParamSetterPtr paramSetter;

  std::mutex processingControl;
  std::atomic_bool doProcessing;
  std::thread mainLoopThread;

  HeatingState heatingState;
  static constexpr int stateCheckInterval = 5000; // in milliseconds
  static constexpr int heatingStabilizeTime = 90; // in seconds
  static constexpr float trickingTemp = 37; // degrees Celsius

  float currentBurnerPower;

  bool isHeatingIdle();
  void trickHeattingSettings();
  float trickParam(ParamId paramToTrick);
  void restoreParam(ParamId paramToTrick, float savedValue);
  void mainLoop();
};

enum class TempTricker::HeatingState
{
  idle,
  starting,
  running,
};
