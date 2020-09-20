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
#include <ParamBody.h>
#include <Log.h>

ParamBody::ParamBody(const ParamTraits& paramTraits_):
  value{0}, paramTraits{paramTraits_}
{
}

uint16_t ParamBody::getParamAddress() const
{
  return paramTraits.address;
}

uint8_t ParamBody::getParamSize() const
{
  return paramTraits.size;
}

int32_t ParamBody::getRawValue() const
{
  return paramTraits.toRaw(getValue(), paramTraits.factor);
}

float ParamBody::getValue() const
{
  return value;
}

void ParamBody::setRawValue(int32_t rawValue_)
{
  return setValue(paramTraits.fromRaw(rawValue_, paramTraits.factor));
}

void ParamBody::setValue(float value_)
{
  if (value_ > paramTraits.max)
  {
    LOGW("ParamBody::setValue: Parameter 0x%04X value greater than its upper limit! %3.3f > %3.3f\n",
         getParamAddress(), value_, paramTraits.max);
  }
  else if (value_ < paramTraits.min)
  {
    LOGW("ParamBody::setValue: Parameter 0x%04X value smaller than its lower limit! %3.3f < %3.3f\n",
         getParamAddress(), value_, paramTraits.min);
  }
  value = value_;
}

bool ParamBody::isReadable() const
{
  return paramTraits.isReadable;
}

bool ParamBody::isWriteable() const
{
  return paramTraits.isWriteable;
}
