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
#include <ParamSetter.h>
#include <cassert>
#include <Log.h>

ParamSetter::ParamSetter(const ParamGeneratorPtr& paramGenerator_, const ParamWriterPtr& paramWriter_) :
  paramGenerator{paramGenerator_},
  paramWriter{paramWriter_},
  status{IoStatus::error}
{
  assert(paramGenerator);
  assert(paramWriter);
}

void ParamSetter::statusCb(IoStatus status_)
{
  {
    std::lock_guard<std::mutex> paramReadyLock{paramReadyControl};
    status = status_;
    LOGD("ParamSetter::statusCb: Callback received, code = %d\n", static_cast<int>(status_));
  }
  paramReady.notify_all();
}

IoStatus ParamSetter::set(ParamId paramId, float value)
{
  auto param = paramGenerator->generate(paramId);
  if (!param)
  {
    LOGE("ParamSetter::set: Cannot generate param with ID: %d\n", static_cast<int>(paramId));
    return IoStatus::error;
  }

  param->setValue(value);

  std::unique_lock<std::mutex> paramReadyLock{paramReadyControl};
  paramWriter->write(param, this);
  paramReady.wait(paramReadyLock);

  return status;
}
