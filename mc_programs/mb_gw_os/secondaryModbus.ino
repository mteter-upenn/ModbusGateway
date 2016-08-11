//bool findRegister(uint16_t u16_reqRegister, uint8_t &u8_regFlags, uint8_t u8_meterType) {
bool findRegister(uint16_t u16_reqRegister, FloatConv &fltConv, uint8_t u8_meterType) {
  uint16_t u16_blockStartInd = 0; // strt_blocks
  uint8_t u8_numBlocks = 0;
  uint16_t u16_blkFirstReg = 0;
  uint16_t u16_blkLastReg = 0;
  uint16_t u16_mtrLibStart;

//  Serial.print(F("findReg: "));
//  Serial.println(meter, DEC);
//  Serial.print(F("reg_strt: "));
//  Serial.println(g_u16_regBlkStart, DEC);
  
  if ((u8_meterType > EEPROM.read(g_u16_regBlkStart + 2)) || (u8_meterType == 0)){  // check if higher than possible number of meters
    return false;  // no registers in eeprom
  }
  
  u16_mtrLibStart = word(EEPROM.read(g_u16_regBlkStart + 4 * u8_meterType - 1), EEPROM.read(g_u16_regBlkStart + 4 * u8_meterType));
  
  u16_blockStartInd = word(EEPROM.read(u16_mtrLibStart), EEPROM.read(u16_mtrLibStart + 1));
  u8_numBlocks = EEPROM.read(u16_mtrLibStart + 2);

//  Serial.print(F("lcl_strt: "));
//  Serial.println(lclmtr_strt, DEC);
//  Serial.print(F("blks: "));
//  Serial.println(strt_blocks, DEC);
//  Serial.print(F("num: "));
//  Serial.println(num_blocks, DEC);

  for (int ii = 0; ii < u8_numBlocks; ++ii)
  {
    u16_blkFirstReg = word(EEPROM.read(((5 * ii) + u16_blockStartInd)), EEPROM.read(((5 * ii) + u16_blockStartInd + 1)));
    u16_blkLastReg = word(EEPROM.read(((5 * ii) + u16_blockStartInd + 2)), EEPROM.read(((5 * ii) + u16_blockStartInd + 3)));

//    Serial.print("eeprom first: ");
//    Serial.println(first_reg);
//    Serial.print("eeprom last: ");
//    Serial.println(last_reg);
//    Serial.print("reg: ");
//    Serial.println(adj_strt_reg);
    
    if ((u16_reqRegister >= u16_blkFirstReg) && (u16_reqRegister <= u16_blkLastReg))
    {
//      Serial.println("flags from this address");
//      Serial.println(((5 * i) + 9));
//      Serial.println(EEPROM.read(((5 * i) + 9)));
      //u8_regFlags = EEPROM.read(((5 * ii) + u16_blockStartInd + 4));
      fltConv = static_cast<FloatConv>(static_cast<int8_t>(EEPROM.read(((5 * ii) + u16_blockStartInd + 4))));
      return true;
    }
  }
  return false;
}

// checks if meter is connected via 485 (false) or ethernet (true)
// also assigns the meter type
bool isMeterEth(uint8_t u8a_clientIp[4], uint8_t u8_virtId, uint8_t &u8_meterType, uint8_t &u8_trueId) {
  uint8_t u8_numMeters;

  u8_numMeters = EEPROM.read(g_u16_mtrBlkStart);
  
  for(int ii = 0; ii < u8_numMeters; ++ii){
    if (u8_virtId == g_u8a_slaveVids[ii]){
      u8_trueId = g_u8a_slaveIds[ii];
      if (EEPROM.read(ii * 9 + 4 + g_u16_mtrBlkStart) == 0){
        u8_meterType = EEPROM.read(ii * 9 + 1 + g_u16_mtrBlkStart);
        return false;  // no ip addr associated with meter
      }
      else{
        for (int jj = 0; jj < 4; ++jj){
          u8a_clientIp[jj] = EEPROM.read(ii * 9 + jj + 4 + g_u16_mtrBlkStart);
//          Serial.println(g_u8a_clientIP[j]);
        }
        u8_meterType = EEPROM.read(ii * 9 + 1 + g_u16_mtrBlkStart);
        return true;  // found ip address associated with meter
      }
    }
  }
  u8_meterType = 0;  // meters without mapped registers are type 0
  return false;  
}

