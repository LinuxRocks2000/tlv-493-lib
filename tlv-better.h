// custom magnetometer code
// BYOI - Bring Your Own I2C
// reverse engineered from the old Infineon library (I did not copy any of their code. this is not because I have ethics, but because I have standards.)
#pragma once
#include <Arduino.h>
#include <Wire.h>


namespace tlv493d {
    struct Register { // the registers are bitbanged most egregiously. this structure describes how to isolate a single register.
        uint8_t byteOffset; // offset into the relevant register set
        uint8_t bitmask; // bitmask
        uint8_t shift; // bitshift
    };

    enum Reg {
        R_BX1 = 0,
        R_BY1,
        R_BZ1,
        R_TEMP1,
        R_FRAMECOUNT,
        R_CHANNEL,
        R_BX2,
        R_BY2,
        R_POWERDOWN,
        R_BZ2,
        R_TEMP2,
        R_MAG1,
        R_MAG2,
        R_MAG3,
        W_PARITY,
        W_ADDR,
        W_MAG1,
        W_INT,
        W_FAST,
        W_LOWPOWER,
        W_MAG2,
        W_TEMP_EN,
        W_LP_PERIOD,
        W_PARITY_EN,
        W_MAG3
    };

    const Register registers[] = { // in order of appearance in memory
        // translating this from the Infineon library was hell
        // they're really bad at writing code, which is kinda surprising
        { 0, 0xFF, 0 },		// R_BX1
        { 1, 0xFF, 0 },		// R_BY1
        { 2, 0xFF, 0 },		// R_BZ1
        { 3, 0xF0, 4 },		// R_TEMP1
        { 3, 0x0C, 2 },		// R_FRAMECOUNT
        { 3, 0x03, 0 },		// R_CHANNEL
        { 4, 0xF0, 4 },		// R_BX2
        { 4, 0x0F, 0 },		// R_BY2
        { 5, 0x10, 4 },		// R_POWERDOWN
        { 5, 0x0F, 0 },		// R_BZ2
        { 6, 0xFF, 0 },		// R_TEMP2
        { 7, 0x18, 3 },		// R_MAG1        magic
        { 8, 0xFF, 0 },		// R_MAG2
        { 9, 0x1F, 0 },		// R_MAG3
        { 11, 0x80, 7 },	// W_PARITY 
        { 11, 0x60, 5 },	// W_ADDR
        { 11, 0x18, 3 },	// W_MAG1
        { 11, 0x04, 2 },	// W_INT
        { 11, 0x02, 1 },	// W_FAST
        { 11, 0x01, 0 },	// W_LOWPOWER
        { 12, 0xFF, 0 },	// W_MAG2
        { 13, 0x80, 7 },	// W_TEMP_EN
        { 13, 0x40, 6 },	// W_LP_PERIOD
        { 13, 0x20, 5 },	// W_PARITY_EN
        { 13, 0x1F, 0 }		// W_MAG3
    };

    struct MagReading {
        float x;
        float y;
        float z;
        float total;
    };

    struct Sensor {
        const int STANDARD_ADDRESS = 0x5E;

        const float B_MUL = 0.098; // multiplier for magnetic readings

        uint8_t address; // it makes me so incredibly happy that the Infineon guys misspelled this as "adress" in their library

        uint8_t registerBits[14]; // first 10 are read-only, last 4 are write-only

        TwoWire* bus;

        enum Mode {
            Powerdown,
            Fast,
            LowPower,
            UltraLowPower,
            MasterControlled
        } m_mode;

        void begin(TwoWire* b); // setup this sensor

        void begin(); // setup this sensor, with reasonable defaults

        void setReg(Reg reg, uint8_t value); // set the value of a register

        uint8_t getReg(Reg reg); // get the value of a register

        bool write(); // write config to the chip
            // returns false on failure

        bool read(); // read data from chip
            // returns false on failure

        void modeSet(Mode mode); // set the mode (doesn't write)

        int16_t concatRegs(Reg one, Reg two, bool whichFour); // concatenate the values of two registers to get a 16-bit signed int (it only actually outputs 12 bits, mind)

        int getDelay(); // get the measurement delay

        MagReading readMag(); // read the magnetometer values
    };
};