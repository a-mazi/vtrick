#ifndef CONTROLLERID_H_
#define CONTROLLERID_H_

enum class ControllerId
{
  UNKNOWN                    ,
  V333MW1                    ,
  V100GC1                    ,
  VDensHO1                   ,
  V200GW1                    ,
  V200KW1                    ,
  V200KW2       = V200KW1    ,
  V200KO1B                   ,
  V200KO2B      = V200KO1B   ,
  VScotHO1                   ,
  VBC550                     ,
  VBC550P       = VBC550     ,
  VBC550S       = VBC550     ,
  VPlusHO1                   ,
  V200WO1                    ,
  VBC700        = V200WO1    ,
  VBC700_BW_WW  = V200WO1    ,
  VBC700_AW     = V200WO1    ,
  VBC702_AW     = V200WO1    ,
  Vitocal200S                ,
  Vitotronic200 = Vitocal200S,
};

#endif // CONTROLLERID_H_
