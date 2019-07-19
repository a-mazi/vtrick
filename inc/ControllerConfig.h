#ifndef CONTROLLERCONFIG_H_
#define CONTROLLERCONFIG_H_

#include <ControllerId.h>
#include <ProtocolId.h>
#include <cstdint>
#include <map>

struct ConfigTraits
{
  ControllerId controller;
  ProtocolId   protocol;
};

using ControllerConfig = std::map<uint32_t, ConfigTraits>;

extern const ControllerConfig controllerConfig;

#endif // CONTROLLERCONFIG_H_
