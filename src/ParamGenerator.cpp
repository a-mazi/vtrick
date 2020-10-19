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
#include <ParamGenerator.h>
#include <Log.h>

ParamGenerator::ParamGenerator(const ParamDefinition& paramDefinition_) :
  paramDefinition{paramDefinition_},
  controllerId{ControllerId::UNKNOWN}
{
}

bool ParamGenerator::isControllerIdAvailable(ControllerId controllerId) const
{
  if (paramDefinition.count(controllerId) > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool ParamGenerator::isParamIdAvailable(ParamId paramId) const
{
  if (isControllerIdAvailable(controllerId))
  {
    if (paramDefinition.at(controllerId).count(paramId) > 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool ParamGenerator::setControllerId(ControllerId controllerId_)
{
  if (isControllerIdAvailable(controllerId_))
  {
    controllerId = controllerId_;
    return true;
  }
  else
  {
    LOGE("ParamGenerator::setControllerId: No parameters defined for controller Id %u!\n",
         static_cast<uint16_t>(controllerId_));
    return false;
  }
}

ParamBodyPtr ParamGenerator::generate(ParamId paramId) const
{
  ParamBodyPtr paramBodyPtr{};
  if (isParamIdAvailable(paramId))
  {
    paramBodyPtr = std::make_shared<ParamBody>(paramDefinition.at(controllerId).at(paramId));
  }
  else
  {
    LOGE("ParamGenerator::generate: Parameter Id %04X for controller Id %u is not available in the parameter definitions!\n",
         static_cast<uint16_t>(paramId), static_cast<uint16_t>(controllerId));
  }
  return paramBodyPtr;
}
