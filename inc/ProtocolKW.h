#ifndef PROTOCOLKW_H_
#define PROTOCOLKW_H_

#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <fstream>

#include <ParamReader.h>
#include <ParamWriter.h>

class ProtocolKW : public ParamReader, public ParamWriter
{
public:
  ProtocolKW();
  ~ProtocolKW();

  void start(std::string serialPortName);
  void stop();

  void readParam(ParamBody* paramBody, ParamReadWriteCallback* callback) override;
  void writeParam(ParamBody* paramBody, ParamReadWriteCallback* callback) override;

private:
  enum class Action;

  std::fstream serialPortFile;

  std::mutex processingControl;
  std::atomic_bool doProcessing;
  std::unique_ptr<std::thread> mainLoopThread;

  static constexpr unsigned int paramQueueSize = 4; //This number must be equal to power of 2
  static constexpr unsigned int paramQueueMask = paramQueueSize - 1;
  unsigned int                  paramQueueBeg;
  unsigned int                  paramQueueEnd;
  std::mutex                    paramQueueEndControl;
  std::mutex                    sameDataAccessControl;
  Action                        action[paramQueueSize];
  ParamBody*                    paramBody[paramQueueSize];
  ParamReadWriteCallback*       callback[paramQueueSize];

  static constexpr char synchroByte    = 0x05;
  static constexpr char synchroAckByte = 0x01;
  static constexpr char readTransissionTypeByte  = 0xF7;
  static constexpr char writeTransissionTypeByte = 0xF4;
  static constexpr int  synchoEventWaitTime = 3; // in seconds
  static constexpr int  synchoEventAckWaitTime = 50; // in milliseconds, this value is a guess
  static constexpr int  maxDatagramBurstCount = 5;

  static constexpr size_t maxDatagramBufferSize = 6;
  char datagramBuffer[maxDatagramBufferSize];

  void addToQueue(Action action, ParamBody* paramBody, ParamReadWriteCallback* callback);
  bool isSynchroEventDetected();
  void ackSynchroEvent();
  ParamReadWriteCallback::Status sendParamReadDatagram(ParamBody* paramBody);
  ParamReadWriteCallback::Status sendParamWriteDatagram(ParamBody* paramBody);
  void mainLoop();
};

enum class ProtocolKW::Action
{
  READ,
  WRITE,
};

#endif // PROTOCOLKW_H_
