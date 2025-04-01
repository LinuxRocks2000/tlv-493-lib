// TODO: split tlv-better.h's functions into this file
#include <tlv-better.h>


namespace tlv493d {
    void Sensor::begin(TwoWire* b) {
        bus = b;
        address = STANDARD_ADDRESS;
        for (int i = 0; i < 4; i ++) { // reset config registers to 0
            registerBits[i + 10] = 0;
        }
        read();

        // copy over our reserved space
        setReg(Reg::W_MAG1, getReg(Reg::R_MAG1));
        setReg(Reg::W_MAG2, getReg(Reg::R_MAG2));
        setReg(Reg::W_MAG3, getReg(Reg::R_MAG3));
        setReg(Reg::W_PARITY_EN, 1); // turn on parity detection
        // parity is clever and pretty useful

        modeSet(Mode::Powerdown); // set mode

        write(); // done!
    }

    void Sensor::begin(){
        begin(&Wire);
    }

    void Sensor::setReg(Reg reg, uint8_t value) {
        Register spec = registers[reg];
        registerBits[spec.byteOffset] &= ~spec.bitmask; // zero the bits we're writing into
        registerBits[spec.byteOffset] |= (value << spec.shift) & spec.bitmask; // shift our
        // value and write it into the appropriate position (the & bitmask isn't strictly necessary,
        // but prevents overflows)
    }

    uint8_t Sensor::getReg(Reg reg) {
        Register spec = registers[reg];
        uint8_t val = registerBits[spec.byteOffset] & spec.bitmask; // nab the bits we want
        // out of a register
        return val >> spec.shift; // rightshift our desired values
    }

    bool Sensor::write() {
        bus -> beginTransmission(address); // start sending data
        if (getReg(W_PARITY_EN)) {
            setReg(W_PARITY, 1); // parity calculation is screwy if we don't set the actual parity bit to 1 first
            uint8_t parity = 0;
            for (int i = 0; i < 4; i ++) {
                parity ^= registerBits[i + 10];
            }
            parity = parity ^ (parity >> 1);
            parity = parity ^ (parity >> 2);
            parity = parity ^ (parity >> 4); // after these operations, parity is in the lsb
            setReg(W_PARITY, parity & 1);
        }
        for (int i = 0; i < 4; i ++) {
            bus -> write(registerBits[i + 10]); // writey writey!
        }
        if (bus -> endTransmission() == 0) { // if endTransmission succeeds, we're done
            return true;
        }
        return false;
    }

    bool Sensor::read() {
        uint8_t bytesIncoming = bus -> requestFrom(address, 10); // ask nicely
        if (bytesIncoming == 10) { // if it says it'll send the data we want, begin reading
            for (int i = 0; i < 10; i ++) {
                registerBits[i] = bus -> read(); // ready ready
            }
            return true; // we're done!
        }
        return false;
    }

    void Sensor::modeSet(Mode mode) {
        switch (mode) {
            case Mode::Powerdown:
                setReg(Reg::W_FAST, 0);
                setReg(Reg::W_LOWPOWER, 0);
                setReg(Reg::W_LP_PERIOD, 0);
                break;
            case Mode::Fast:
                setReg(Reg::W_FAST, 1);
                setReg(Reg::W_LOWPOWER, 0);
                setReg(Reg::W_LP_PERIOD, 0);
                break;
            case Mode::LowPower:
                setReg(Reg::W_FAST, 0);
                setReg(Reg::W_LOWPOWER, 1);
                setReg(Reg::W_LP_PERIOD, 1);
                break;
            case Mode::UltraLowPower:
                setReg(Reg::W_FAST, 0);
                setReg(Reg::W_LOWPOWER, 1);
                setReg(Reg::W_LP_PERIOD, 0);
                break;
            case Mode::MasterControlled:
                setReg(Reg::W_FAST, 1);
                setReg(Reg::W_LOWPOWER, 1);
                setReg(Reg::W_LP_PERIOD, 1);
                break;
        };
        m_mode = mode;
    }

    int Sensor::getDelay() {
        switch (m_mode) {
            case Mode::Powerdown:
                return 1000;
                break;
            case Mode::Fast:
                return 0;
                break;
            case Mode::LowPower:
                return 10;
                break;
            case Mode::UltraLowPower:
                return 100;
                break;
            case Mode::MasterControlled:
                return 10;
                break;
        }
    }

    int16_t Sensor::concatRegs(Reg upper, Reg lower, bool whichFour) { // upperFull: which is the 4-bit value here, the lower (true) or the upper (false)?
        int16_t val = 0;
        if (whichFour) { // upper is 8 bits, lower is 4 bits
            val = getReg(upper) << 8;
            val |= (getReg(lower) & 0x0F) << 4;
        }
        else {
            val = (getReg(upper) & 0x0F) << 12;
            val |= getReg(lower) << 4;
        }
        val >>= 4;
        return val;
    }
    
    MagReading Sensor::readMag() {
        Mode oldMode = m_mode;
        if (m_mode == Powerdown) {
            modeSet(MasterControlled);
            write();
            delay(getDelay());
        }
        read();
        MagReading data;
        data.x = ((float)concatRegs(R_BX1, R_BX2, true)) * B_MUL;
        data.y = ((float)concatRegs(R_BY1, R_BY2, true)) * B_MUL;
        data.z = ((float)concatRegs(R_BZ1, R_BZ2, true)) * B_MUL;
        data.total = sqrt(data.x * data.x + data.y * data.y + data.z * data.z); // pythagorean identity... yippee!
        if (m_mode != oldMode) {
            modeSet(oldMode);
            write();
        }
        return data;
    }
};