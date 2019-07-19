#ifndef PARAMBODYGENERATOR_H_
#define PARAMBODYGENERATOR_H_

#include <ParamId.h>
#include <ParamBody.h>
#include <ParamDefinition.h>
#include <ControllerId.h>
#include <ControllerConfig.h>

class ParamBodyGenerator
{
public:
  ParamBodyGenerator(const ParamDefinition& paramDefinition);
  void setController(ControllerId controllerId);
  ParamBody generateParamBody(ParamId paramId);

private:
  const ParamDefinition& paramDefinition;
  ControllerId controllerId;
};

#endif // PARAMBODYGENERATOR_H_
