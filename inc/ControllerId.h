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
#ifndef CONTROLLERID_H_
#define CONTROLLERID_H_

#include <stdint.h>

enum class ControllerId
{
  UNKNOWN                    ,
  ALL                        ,
  V333MW1                    ,
  V100GC1                    ,
  VDensHO1                   ,
  V200GW1                    ,
  V200KW1                    ,
  V200KW2       = V200KW1    ,
  V200KO1B                   ,
  V200KO2B      = V200KO1B   ,
  VScotHO1                   ,
  VBC550                     ,
  VBC550P       = VBC550     ,
  VBC550S       = VBC550     ,
  VPlusHO1                   ,
  V200WO1                    ,
  VBC700        = V200WO1    ,
  VBC700_BW_WW  = V200WO1    ,
  VBC700_AW     = V200WO1    ,
  VBC702_AW     = V200WO1    ,
  Vitocal200S                ,
  Vitotronic200 = Vitocal200S,
};

#endif // CONTROLLERID_H_
