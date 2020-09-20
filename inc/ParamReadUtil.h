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
#include <condition_variable>
#include <ParamGenerator.h>
#include <ParamReader.h>

class ParamReadUtil : public ParamReadWriteCallback
{
public:
  struct Result;

  ParamReadUtil() = delete;
  ParamReadUtil(const ParamGeneratorPtr& paramGenerator, const ParamReaderPtr& paramReader);
  ~ParamReadUtil() = default;

  void statusCb(IoStatus status) final;

  Result get(ParamId paramId);

private:
  ParamGeneratorPtr paramGenerator;
  ParamReaderPtr paramReader;
  std::condition_variable paramReady;
  std::mutex paramReadyControl;
  IoStatus status;
};

struct ParamReadUtil::Result
{
  IoStatus ioStatus;
  float    value;
};
