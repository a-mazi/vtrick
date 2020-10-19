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
#include <ParamGetter.h>
#include <cassert>
#include <Log.h>

ParamGetter::ParamGetter(const ParamGeneratorPtr& paramGenerator_, const ParamReaderPtr& paramReader_) :
  paramGenerator{paramGenerator_},
  paramReader{paramReader_},
  status{IoStatus::error}
{
  assert(paramGenerator);
  assert(paramReader);
}

void ParamGetter::statusCb(IoStatus status_)
{
  {
    std::lock_guard<std::mutex> paramReadyLock{paramReadyControl};
    status = status_;
    LOGD("ParamGetter::statusCb: Callback received, code = %d\n", static_cast<int>(status_));
  }
  paramReady.notify_all();
}

ParamGetter::Result ParamGetter::get(ParamId paramId)
{
  status = IoStatus::error;
  Result result{IoStatus::error, 0};

  auto param = paramGenerator->generate(paramId);
  if (!param)
  {
    LOGE("ParamGetter::get: Cannot generate param with ID: %d\n", static_cast<int>(paramId));
    return result;
  }

  std::unique_lock<std::mutex> paramReadyLock{paramReadyControl};
  paramReader->read(param, this);
  paramReady.wait(paramReadyLock);

  result.ioStatus = status;
  if (status == IoStatus::ok)
  {
    result.value = param->getValue();
  }

  return result;
}
