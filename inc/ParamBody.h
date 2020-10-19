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

#include <memory>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ParamTraits.h>

class ParamBody
{
public:
  ParamBody() = delete;
  ParamBody(const ParamTraits& paramTraits);

  uint16_t getParamAddress() const;
  uint8_t  getParamSize() const;

  int32_t getRawValue() const;
  float   getValue() const;
  void    setRawValue(int32_t rawValue);
  void    setValue(float value);

  bool isReadable() const;
  bool isWriteable() const;

private:
  float       value;
  ParamTraits paramTraits;
};

using ParamBodyPtr = std::shared_ptr<ParamBody>;
using ConstParamBodyPtr = std::shared_ptr<const ParamBody>;
