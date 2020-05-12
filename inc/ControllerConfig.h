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
#ifndef CONTROLLERCONFIG_H_
#define CONTROLLERCONFIG_H_

#include <ControllerId.h>
#include <ProtocolId.h>
#include <cstdint>
#include <map>

struct ConfigTraits
{
  ControllerId controllerId;
  ProtocolId   protocolId;
};

using ControllerConfig = std::map<uint16_t, ConfigTraits>;

extern const ControllerConfig controllerConfig;

#endif // CONTROLLERCONFIG_H_
