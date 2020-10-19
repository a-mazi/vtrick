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
#include <TempTricker.h>
#include <cassert>
#include <Log.h>

TempTricker::TempTricker(ParamGetterPtr&& paramGetter_, ParamSetterPtr&& paramSetter_) :
  paramGetter{std::move(paramGetter_)},
  paramSetter{std::move(paramSetter_)},
  doProcessing{false},
  heatingState{HeatingState::running},
  currentBurnerPower{0}
{
  assert(paramGetter);
  assert(paramSetter);
}

void TempTricker::start()
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (!mainLoopThread.joinable())
  {
    doProcessing.store(true);
    mainLoopThread = std::thread{&TempTricker::mainLoop, this};
  }
}

void TempTricker::stop()
{
  std::lock_guard<std::mutex> processingControlLock{processingControl};
  if (mainLoopThread.joinable())
  {
    doProcessing.store(false);
    mainLoopThread.join();
  }
}

bool TempTricker::isHeatingIdle()
{
  auto valvePosition = paramGetter->get(ParamId::SWITCHING_VALVE_POSITION);
  if (valvePosition.ioStatus != IoStatus::ok)
  {
    LOGE("TempTricker::isHeatingIdle: Cannot read switching valve position due to %s!\n",
         ioStatusName.at(valvePosition.ioStatus).c_str());
    return true;
  }
  else
  {
    if (valvePosition.value != ValvePosition::heating)
    {
      return true;
    }
    else
    {
      auto burnerPower = paramGetter->get(ParamId::BURNER_CURRENT_POWER);
      if (burnerPower.ioStatus != IoStatus::ok)
      {
        LOGE("TempTricker::isHeatingIdle: Cannot read current burner power due to %s!\n",
             ioStatusName.at(valvePosition.ioStatus).c_str());
        return true;
      }
      else
      {
        currentBurnerPower = burnerPower.value;
        return (burnerPower.value == 0);
      }
    }
  }
}

void TempTricker::trickHeattingSettings()
{
  LOGI("TempTricker::trickHeattingSettings: Tricking day time settings\n");
  float dayTemp = trickParam(ParamId::TEMP_ROOM_DAY_NORMAL_TARGET);
  LOGI("TempTricker::trickHeattingSettings: Tricking night time settings\n");
  float nightTemp = trickParam(ParamId::TEMP_ROOM_NIGHT_REDUCED_TARGET);

  LOGI("TempTricker::trickHeattingSettings: Sleep for %d seconds\n", heatingStabilizeTime);
  std::this_thread::sleep_for(std::chrono::seconds(heatingStabilizeTime));

  LOGI("TempTricker::trickHeattingSettings: Restore day time settings\n");
  restoreParam(ParamId::TEMP_ROOM_DAY_NORMAL_TARGET, dayTemp);
  LOGI("TempTricker::trickHeattingSettings: Restore night time settings\n");
  restoreParam(ParamId::TEMP_ROOM_NIGHT_REDUCED_TARGET, nightTemp);
}

float TempTricker::trickParam(ParamId param)
{
  auto saved = paramGetter->get(param);
  if (saved.ioStatus != IoStatus::ok)
  {
    LOGE("TempTricker::trickParam: Cannot read param to save due to %s!\n", ioStatusName.at(saved.ioStatus).c_str());
    return 0;
  }
  LOGI("TempTricker::trickParam: Save param %d value %2.1f\n", static_cast<int>(param), saved.value);

  auto status = paramSetter->set(param, trickingTemp);
  if (status != IoStatus::ok)
  {
    LOGE("TempTricker::trickParam: Cannot set param to tricked value due to %s!\n", ioStatusName.at(status).c_str());
    return 0;
  }
  LOGI("TempTricker::trickParam: Set param %d to tricking value %2.1f\n", static_cast<int>(param), trickingTemp);

  return saved.value;
}

void TempTricker::restoreParam(ParamId param, float savedValue)
{
  auto status = paramSetter->set(param, savedValue);
  if (status != IoStatus::ok)
  {
    LOGE("TempTricker::restoreParam: Cannot restore saved param value due to %s!\n", ioStatusName.at(status).c_str());
    return;
  }
  LOGI("TempTricker::restoreParam: Restore param %d saved value %2.1f\n", static_cast<int>(param), savedValue);
}

void TempTricker::mainLoop()
{
  int checkingCounts = 0;
  static constexpr int reportingCounts = 12;

  while (doProcessing.load())
  {
    switch (heatingState)
    {
      case HeatingState::idle:
        LOGD("TempTricker::mainLoop: Heating is idle\n");
        if ((checkingCounts++ % reportingCounts) == 0)
        {
          LOGI("TempTricker::mainLoop: Heating is idle\n");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(stateCheckInterval));
        if (!isHeatingIdle())
        {
          heatingState = HeatingState::starting;
        }
        break;

      case HeatingState::starting:
        LOGI("TempTricker::mainLoop: Heating is starting - tricking heating settings\n");
        trickHeattingSettings();
        LOGI("TempTricker::mainLoop: Tricking done\n");
        heatingState = HeatingState::running;
        break;

      case HeatingState::running:
        LOGD("TempTricker::mainLoop: Heating is running, burner power: %2.1f%%\n", currentBurnerPower);
        if ((checkingCounts++ % reportingCounts) == 0)
        {
          LOGI("TempTricker::mainLoop: Heating is running, burner power: %2.1f%%\n", currentBurnerPower);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(stateCheckInterval));
        if (isHeatingIdle())
        {
          heatingState = HeatingState::idle;
        }
        break;

      default:
        doProcessing.store(false);
        break;
    }
  }
}
