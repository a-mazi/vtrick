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
#include <ParamTraits.h>

int32_t ParamTraits::multiplyByFactor(float value, float factor)
{
  return static_cast<int32_t>(value * factor);
}

float ParamTraits::divideByFactor(int32_t value, float factor)
{
  return static_cast<float>(value) / factor;
}

int32_t ParamTraits::pumpValueToRaw(float value, float factor)
{
  return static_cast<int32_t>(value * factor) << sizeof(char);
}

float ParamTraits::pumpValueFromRaw(int32_t value, float factor)
{
  int32_t byteMask = (1L << 8) - 1;
  return static_cast<float>((value >> 8) & byteMask) / factor;
}

float ParamTraits::divideLSBbyFactor(int32_t value, float factor)
{
  int32_t byteMask = (1L << 8) - 1;
  return static_cast<float>(value & byteMask) / factor;
}
