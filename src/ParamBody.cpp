#include <ParamBody.h>

ParamBody::ParamBody(uint32_t paramAddress_, size_t dataSize_) :
                    paramAddress{paramAddress_}, dataSize{dataSize_}
{
  if (dataSize_ > 0)
  {
    dataBuffer = std::make_shared<char>(dataSize_);
  }
  else
  {
    dataBuffer = nullptr;
  }
}

uint32_t ParamBody::getParamAddress()
{
  return paramAddress;
}

size_t ParamBody::getDataSize()
{
  return dataSize;
}

std::shared_ptr<char> ParamBody::getDataBuffer()
{
  return dataBuffer;
}
