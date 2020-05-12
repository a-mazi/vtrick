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
#include <memory>
#include <Log.h>
#include <SerialPort.h>
#include <KettlePort.h>
#include <ControllerConfig.h>
#include <ParamDefinition.h>
#include <ParamGenerator.h>
#include <P300Protocol.h>
#include <P300PacketGenerator.h>
#include <ParamAccessProxy.h>
#include <TempMonitor.h>
#include <TempSetter.h>
#include <ParamReadUtil.h>

bool isControllerSupported(uint16_t controllerMark, ControllerId& controllerId)
{
  if (controllerConfig.count(controllerMark) > 0)
  {
    auto controllerTraits = controllerConfig.at(controllerMark);
    controllerId = controllerTraits.controllerId;
    if (controllerTraits.protocolId == ProtocolId::P300)
    {
      return true;
    }
    else
    {
      LOGE("vtrick: Controller (Mark: %04X, Id: %u) is using unsupported protocol Id: %u!\n", controllerMark,
           static_cast<unsigned int>(controllerId), static_cast<unsigned int>(controllerTraits.protocolId));
    }
  }
  else
  {
    LOGE("vtrick: Controller Mark %04X is unsupported!\n", controllerMark);
  }
  return false;
}


int main(int argc, char * argv[]) {

#ifdef RELEASE
  LOGI("vtrick: Release %s\n", RELEASE);
#else
  LOGI("vtrick: Debug build\n");
#endif

  auto serialPort  = std::make_shared<SerialPort>("/dev/ttyUSB0");
  auto kettlePort  = std::make_shared<KettlePort>();
  kettlePort->attachSerial(serialPort);

  auto packetGenerator = std::make_shared<P300PacketGenerator>(P300PacketGenerator{});
  auto protocol300  = std::make_shared<P300Protocol>(packetGenerator, kettlePort);

  auto paramAccessProxy = std::make_shared<ParamAccessProxy>(protocol300, protocol300, 10000);

  auto paramGenerator = std::make_shared<ParamGenerator>(paramDefinition);
  if (!paramGenerator->setControllerId(ControllerId::ALL))
  {
    return 1;
  }

  kettlePort->open();
  auto ioStatus = protocol300->init();
  if (ioStatus == IoStatus::ok)
  {
    protocol300->start();
  }
  else
  {
    return 1;
  }

  LOGI("vtrick: Reading controller Mark ...\n");
  ParamReadUtil paramReadUtil{paramGenerator, paramAccessProxy};
  auto result = paramReadUtil.get(ParamId::CONTROLLER_MARK);

  uint16_t     controllerMark = static_cast<uint16_t>(result.value);
  ControllerId controllerId   = ControllerId::UNKNOWN;

  if ((result.ioStatus != IoStatus::ok) || (!isControllerSupported(controllerMark, controllerId)))
  {
    return 1;
  }
  LOGI("vtrick: Controller Mark: %04X, Id: %u\n", controllerMark, static_cast<unsigned int>(controllerId));

  if (!paramGenerator->setControllerId(controllerId))
  {
    return 1;
  }
  auto tempMonitor = std::make_shared<TempMonitor>(paramGenerator, paramAccessProxy);
  auto tempSetter = std::make_shared<TempSetter>(paramGenerator, paramAccessProxy);

  tempMonitor->start();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  tempSetter->start();
  std::this_thread::sleep_for(std::chrono::seconds(30));
  tempSetter->stop();
  tempMonitor->stop();
  protocol300->stop();
  kettlePort->close();

  return 0;
}
