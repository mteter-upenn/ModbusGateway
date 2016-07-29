bool findRegister(uint16_t &adj_strt_reg, uint8_t &reg_flags, uint8_t meter)
{
  uint16_t strt_blocks = 0;
  uint8_t num_blocks = 0;
  uint16_t first_reg = 0;
  uint16_t last_reg = 0;
  uint8_t i;
  uint16_t lclmtr_strt;

//  Serial.print(F("findReg: "));
//  Serial.println(meter, DEC);
//  Serial.print(F("reg_strt: "));
//  Serial.println(g_u16_regBlkStart, DEC);
  
  if ((meter > EEPROM.read(g_u16_regBlkStart + 2)) || (meter == 0)){  // check if higher than possible number of meters
    return false;  // no registers in eeprom
  }
  
  lclmtr_strt = word(EEPROM.read(g_u16_regBlkStart + 4 * meter - 1), EEPROM.read(g_u16_regBlkStart + 4 * meter));
  
  strt_blocks = word(EEPROM.read(lclmtr_strt), EEPROM.read(lclmtr_strt + 1));
  num_blocks = EEPROM.read(lclmtr_strt + 2);

//  Serial.print(F("lcl_strt: "));
//  Serial.println(lclmtr_strt, DEC);
//  Serial.print(F("blks: "));
//  Serial.println(strt_blocks, DEC);
//  Serial.print(F("num: "));
//  Serial.println(num_blocks, DEC);

  for (i = 0; i < num_blocks; i++)
  {
    first_reg = word(EEPROM.read(((5 * i) + strt_blocks)), EEPROM.read(((5 * i) + strt_blocks + 1)));
    last_reg = word(EEPROM.read(((5 * i) + strt_blocks + 2)), EEPROM.read(((5 * i) + strt_blocks + 3)));

//    Serial.print("eeprom first: ");
//    Serial.println(first_reg);
//    Serial.print("eeprom last: ");
//    Serial.println(last_reg);
//    Serial.print("reg: ");
//    Serial.println(adj_strt_reg);
    
    if ((adj_strt_reg >= first_reg) && (adj_strt_reg <= last_reg))
    {
//      Serial.println("flags from this address");
//      Serial.println(((5 * i) + 9));
//      Serial.println(EEPROM.read(((5 * i) + 9)));
      reg_flags = EEPROM.read(((5 * i) + strt_blocks + 4));
      return true;
    }
  }
  return false;
}


bool isMeterEth(uint8_t dev_id, uint8_t &mtr_typ, uint8_t &act_dev){
  uint8_t num_mtrs;
  uint8_t i, j;

  num_mtrs = EEPROM.read(g_u16_mtrBlkStart);
  
  for(i = 0; i < num_mtrs; i++){
    if (dev_id == g_u8a_slaveVids[i]){
      act_dev = g_u8a_slaveIds[i];
      if (EEPROM.read(i * 9 + 4 + g_u16_mtrBlkStart) == 0){
        mtr_typ = EEPROM.read(i * 9 + 1 + g_u16_mtrBlkStart);
        return false;  // no ip addr associated with meter
      }
      else{
        for (j = 0; j < 4; j++){
          g_u8a_clientIP[j] = EEPROM.read(i * 9 + j + 4 + g_u16_mtrBlkStart);
//          Serial.println(g_u8a_clientIP[j]);
        }
        mtr_typ = EEPROM.read(i * 9 + 1 + g_u16_mtrBlkStart);
        return true;  // found ip address associated with meter
      }
    }
  }
  mtr_typ = 0;  // meters without mapped registers are type 0
  return false;  
}

