#ifndef FloatConvEnum_h
#define FloatConvEnum_h


/**
enum class for definitions of different data types meters can return
*/
enum class FloatConv {
  SKIP = -1,        //  -1
  FLOAT,            //   0
  UINT16,           //   1
  INT16,            //   2
  UINT32,           //   3
  INT32,            //   4
  MOD1K,            //   5
  MOD10K,           //   6
  MOD20K,           //   7
  MOD30K,           //   8
  UINT64,           //   9
  ENERGY,           //  10
  DOUBLE,           //  11
  FLOAT_WS = 64,    //  64
	//UINT16_WS,
	//INT16_WS,
  UINT32_WS = 67,   //  67
  INT32_WS,         //  68
  MOD1K_WS,         //  69
  MOD10K_WS,        //  70
  MOD20K_WS,        //  71
  MOD30K_WS,        //  72
  UINT64_WS,        //  73
  ENERGY_WS,        //  74
  DOUBLE_WS,        //  75
};

#endif