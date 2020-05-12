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
#ifndef PARAMTRAITS_H_
#define PARAMTRAITS_H_

#include <cstddef>
#include <cstdint>
#include <functional>

struct ParamTraits
{
  bool     isReadable;
  bool     isWriteable;
  uint16_t address;
  uint8_t  size;
  float    factor;
  float    min;
  float    max;
  std::function<int32_t(float, float)> toRaw;
  std::function<float(int32_t, float)> fromRaw;

  static int32_t multiplyByFactor    (float value, float factor);
  static float   divideByFactor    (int32_t value, float factor);
  static int32_t pumpValueToRaw      (float value, float factor);
  static float   pumpValueFromRaw  (int32_t value, float factor);
  static float   divideLSBbyFactor(int32_t value, float factor);
};

#endif // PARAMTRAITS_H_
