#ifndef PARAMBODY_H_
#define PARAMBODY_H_

#include <cstddef>
#include <cstdint>
#include <memory>

class ParamBody
{
public:
  ParamBody(uint32_t paramAddress, size_t dataSize);
  uint32_t getParamAddress();
  size_t   getDataSize();
  std::shared_ptr<char> getDataBuffer();

private:
  uint32_t paramAddress;
  size_t   dataSize;
  std::shared_ptr<char> dataBuffer;
};

#endif // PARAMBODY_H_
