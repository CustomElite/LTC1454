/*
LTC1454 Serial Interface
SPI Mode = 0
Max Freq = 4.0MHz

Data is loaded as one 24-bit word, DAC A first, then DAC B. The MSB is loaded first for each DAC.

                      BYTE #1                                      
|| A11 | A10 | A09 | A08 | A07 | A06 | A05 | A04 ||

                      BYTE #2
|| A03 | A02 | A01 | A00 | B11 | B10 | B09 | B08 || 

                      BYTE #3
|| B07 | B06 | B05 | B04 | B03 | B02 | B01 | B00 ||

AXX :: DAC A (12) Data Bits
BXX :: DAC B (12) Data Bits
*/
#ifndef LTC1454_hpp
#define LTC1454_hpp

#include <Arduino.h>
#include <SPI.h>

#define LIMIT(val, min, max)   (val < min) ? min : (val > max) ? max : val

class LTC1454 
{
public:
    enum class Channel
    {
        DAC_A = 0U,
        DAC_B
    };

    static const uint16_t Fullscale = 4095u;

public:
    // Constructor
    explicit LTC1454(uint8_t cs_pin, uint8_t clr_pin = 0u);

    ~LTC1454();

    // Initializes the device
    void init(float vref = 2.048f, uint8_t gain = 2u);                            

    // Updates the output DACs with values set via the 'set' method
    void update() const;

    // Toggles clear pin if set, or sets all channels to 0 while preserving the current set values
    void clear();

    // Returns set value of selected channel
    uint16_t get(Channel channel) const;

    // Sets all outputs to 'set_all_value'
    void set(uint16_t set_all_value);

    // Sets selected channel to 'set_value'
    void set(Channel channel, uint16_t set_value);

    // Converts a digital value(uint16_t) to a analog voltage(float)
    inline float toVoltage(uint16_t code) const
    {
        return (float)(code * ((m_vRef / (Fullscale + 1)) * m_gain));
    }

    // Converts a analog voltage(float) to a digital value(uint16_t)
    inline uint16_t toCode(float voltage) const
    {
        return (uint16_t)((voltage / ((m_vRef / (Fullscale + 1)) * m_gain)) + 0.5F);
    }

    inline void setClockFreq(uint32_t clk_freq)
    {
        m_spiSettings = SPISettings(clk_freq, MSBFIRST, SPI_MODE0);
    }

    inline void setVRef(float vref)
    {
        m_vRef = LIMIT(vref, 0.0f, 2.7f);
    }

    inline void setGain(uint8_t gain)
    {
        m_gain = LIMIT(gain, 1u, 2u);
    }

private:
    const uint8_t m_csPin, m_clrPin;
    bool m_initialized;
    float m_vRef;
    uint8_t m_gain;
    SPISettings m_spiSettings;

    union 
    {
        struct { uint32_t tx_dacB : 12, tx_dacA : 12; };    
        uint8_t tx_8bits[4];
    } m_txBuffer;
};

#endif  // LTC1454_hpp