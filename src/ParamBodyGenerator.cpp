#include "ParamBodyGenerator.h"

ParamBodyGenerator::ParamBodyGenerator(const ParamDefinition& paramDefinition_) :
                                       paramDefinition{paramDefinition_},
                                       controllerId{ControllerId::UNKNOWN}
{
}

void ParamBodyGenerator::setController(ControllerId controllerId_)
{
  controllerId = controllerId_;
}

ParamBody ParamBodyGenerator::generateParamBody(ParamId paramId)
{
  uint32_t paramAddress = 0;
  size_t dataSize = 0;

  if (paramDefinition.count(controllerId) > 0)
  {
    if (paramDefinition.at(controllerId).count(paramId) > 0)
    {
      paramAddress = paramDefinition.at(controllerId).at(paramId).address;
      dataSize = paramDefinition.at(controllerId).at(paramId).size;
    }
  }

  return ParamBody{paramAddress, dataSize};
}
