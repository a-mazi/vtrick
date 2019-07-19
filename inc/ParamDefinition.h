#ifndef PARAMDEFINITION_H_
#define PARAMDEFINITION_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <ControllerId.h>
#include <ParamId.h>

struct ParamTraits
{
  bool     isReadable;
  bool     isWriteable;
  uint32_t address;
  size_t   size;
  float    factor;
  int      min;
  int      max;
};

using ParamDefinition = std::map<ControllerId, std::map<ParamId, ParamTraits>>;

extern const ParamDefinition paramDefinition;

#endif // PARAMDEFINITION_H_
