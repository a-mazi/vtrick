#ifndef PARAMREADER_H_
#define PARAMREADER_H_

#include <ParamBody.h>
#include <ParamReadWriteCallback.h>

class ParamReader
{
public:
  virtual void readParam(ParamBody* paramBody, ParamReadWriteCallback* callback) = 0;
};

#endif // PARAMREADER_H_
