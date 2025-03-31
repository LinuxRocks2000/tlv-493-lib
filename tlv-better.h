// custom magnetometer code
// BYOI - Bring Your Own I2C
// reverse engineered from the old Infineon library (I did not copy any of their code. this is not because I have ethics, but because I have standards.)


namespace tlv493 {
  const struct Register { // the registers are bitbanged most egregiously. this structure describes how to isolate a single register.
    uint8_t byteOffset; // offset into the relevant register set
    uint8_t bitmask; // bitmask
    uint8_t shift; // bitshift
  } regMasks[] = {
    { REGMASK_READ, 0, 0xFF, 0 },		// R_BX1
    { REGMASK_READ, 4, 0xF0, 4 },		// R_BX2
    { REGMASK_READ, 1, 0xFF, 0 },		// R_BY1
    { REGMASK_READ, 4, 0x0F, 0 },		// R_BY2
    { REGMASK_READ, 2, 0xFF, 0 },		// R_BZ1
    { REGMASK_READ, 5, 0x0F, 0 },		// R_BZ2
    { REGMASK_READ, 3, 0xF0, 4 },		// R_TEMP1
    { REGMASK_READ, 6, 0xFF, 0 },		// R_TEMP2
    { REGMASK_READ, 3, 0x0C, 2 },		// R_FRAMECOUNTER
    { REGMASK_READ, 3, 0x03, 0 },		// R_CHANNEL
    { REGMASK_READ, 5, 0x10, 4 },		// R_POWERDOWNFLAG
    { REGMASK_READ, 7, 0x18, 3 },		// R_RES1
    { REGMASK_READ, 8, 0xFF, 0 },		// R_RES2
    { REGMASK_READ, 9, 0x1F, 0 },		// R_RES3
    { REGMASK_WRITE, 1, 0x80, 7 },		// W_PARITY
    { REGMASK_WRITE, 1, 0x60, 5 },		// W_ADDR
    { REGMASK_WRITE, 1, 0x04, 2 },		// W_INT
    { REGMASK_WRITE, 1, 0x02, 1 },		// W_FAST
    { REGMASK_WRITE, 1, 0x01, 0 },		// W_LOWPOWER
    { REGMASK_WRITE, 3, 0x80, 7 },		// W_TEMP_EN
    { REGMASK_WRITE, 3, 0x40, 6 },		// W_LOWPOWER
    { REGMASK_WRITE, 3, 0x20, 5 },		// W_POWERDOWN
    { REGMASK_WRITE, 1, 0x18, 3 },		// W_RES1
    { REGMASK_WRITE, 2, 0xFF, 0 },		// W_RES2
    { REGMASK_WRITE, 3, 0x1F, 0 }		// W_RES3
  };

  struct Tlv493 {
    const int STANDARD_ADDRESS = 0x5E;
    
    uint8_t address; // it makes me so incredibly happy that the Infineon guys misspelled this as "adress" in their library

    uint8_t config[4]; // the writable part of the registers

    uint8_t data[10]; // the readable part of the registers

    TwoWire* bus;

    void begin(TwoWire* b) {
      bus = b;
      address = STANDARD_ADDRESS;
      for (int i = 0; i < 4; i ++) { // reset config registers to 0
        config[i] = 0;
      }
      readOut();

    }

    void setConfig(uint8_t i, uint8_t value) {
      config[i] = value;
    }

    uint8_t getData(uint8_t i) {
      return (data[i] & caveat[i].mask) 
    }

    void begin() {
      begin(&Wire);
    }

    void write() { // write config to the chip

    }
  };
};