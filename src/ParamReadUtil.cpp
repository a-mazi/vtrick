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
#include <ParamReadUtil.h>
#include <Log.h>

ParamReadUtil::ParamReadUtil(std::shared_ptr<const ParamGenerator> paramGenerator_, std::shared_ptr<ParamReader> paramReader_) :
  paramGenerator{paramGenerator_},
  paramReader{paramReader_},
  status{IoStatus::error}
{
}

void ParamReadUtil::statusCb(IoStatus status_)
{
  {
    std::lock_guard<std::mutex> paramReadyLock{paramReadyControl};
    status = status_;
    LOGD("ParamReadUtil::statusCb: Callback received, code = %d\n", static_cast<int>(status_));
  }
  paramReady.notify_all();
}

ParamReadUtil::Result ParamReadUtil::get(ParamId paramId)
{
  status = IoStatus::error;
  Result result{IoStatus::error, 0};

  auto param = paramGenerator->generate(paramId);
  if (param == nullptr)
  {
    return result;
  }

  std::unique_lock<std::mutex> paramReadyLock{paramReadyControl};
  paramReader->read(param, this);
  paramReady.wait(paramReadyLock);

  if (status == IoStatus::ok)
  {
    result.ioStatus = status;
    result.value = param->getValue();
  }

  return result;
}
