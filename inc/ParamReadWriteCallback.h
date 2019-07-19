#ifndef PARAMREADWRITECALLBACK_H_
#define PARAMREADWRITECALLBACK_H_


class ParamReadWriteCallback
{
public:
  enum class Status;

  virtual void statusCb(Status status);
};

enum class ParamReadWriteCallback::Status
{
  OK,
  PARAM_DATA_SIZE_TO_BIG,
  NO_ACTION,
};


#endif // PARAMREADWRITECALLBACK_H_
