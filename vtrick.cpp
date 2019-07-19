#include <ParamDefinition.h>
#include <ControllerConfig.h>
#include <ParamBodyGenerator.h>
#include <ProtocolKW.h>

int main(int argc, char * argv[]) {

  ProtocolKW protocolKW;
  ParamBodyGenerator paramBodyGenerator{paramDefinition};

  protocolKW.start("/dev/ttyUSB0");
  protocolKW.stop();

  return 0;
}



