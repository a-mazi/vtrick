#ifndef PARAMWRITER_H_
#define PARAMWRITER_H_

#include <ParamBody.h>
#include <ParamReadWriteCallback.h>

class ParamWriter
{
public:
  virtual void writeParam(ParamBody* paramBody, ParamReadWriteCallback* callback) = 0;
};

#endif // PARAMWRITER_H_
