#include "LTC1454.hpp"

LTC1454::LTC1454(uint8_t cs_pin, uint8_t clr_pin = 0U)
    : m_csPin(cs_pin), 
    m_clrPin(clr_pin),
    m_initialized(false),
    m_vRef(0.0f),
    m_gain(0u),
    m_spiSettings()
{
    m_txBuffer.tx_dacB = 0;
    m_txBuffer.tx_dacA = 0;
}

LTC1454::~LTC1454()
{
    if (m_initialized) SPI.end();
}

void LTC1454::init(float vref = 2.048f, uint8_t gain = 2u)
{
    if (!m_initialized)
    {
        SPI.begin();

        pinMode(m_csPin, OUTPUT);
        digitalWrite(m_csPin, HIGH);

        if (m_clrPin) {
            pinMode(m_clrPin, OUTPUT);
            digitalWrite(m_clrPin, HIGH);
        }

        setVRef(vref);
        setGain(gain);

        m_initialized = true;
    }

    clear();
}

void LTC1454::update() const
{
    if (m_initialized)
    {
        SPI.beginTransaction(m_spiSettings);
          digitalWrite(m_csPin, LOW);
          SPI.transfer(m_txBuffer.tx_8bits[2]);
          SPI.transfer(m_txBuffer.tx_8bits[1]);
          SPI.transfer(m_txBuffer.tx_8bits[0]);
          digitalWrite(m_csPin, HIGH);
        SPI.endTransaction();
    }
}

void LTC1454::clear()
{
    if (m_initialized)
    {
        m_txBuffer.tx_dacB = 0;
        m_txBuffer.tx_dacA = 0;

        if (m_clrPin) {
            digitalWrite(m_clrPin, LOW);
            delay(10);
            digitalWrite(m_clrPin, HIGH);
        } else {
            update();
        }
    }
}

uint16_t LTC1454::get(Channel channel) const
{
    switch (channel) {
        case Channel::DAC_A: return m_txBuffer.tx_dacA;
        case Channel::DAC_B: return m_txBuffer.tx_dacB;
    }

    return 0;
}

void LTC1454::set(uint16_t set_all_value)
{
    m_txBuffer.tx_dacB = constrain(set_all_value, 0, Fullscale);
    m_txBuffer.tx_dacA = constrain(set_all_value, 0, Fullscale);
}

void LTC1454::set(Channel channel, uint16_t set_value)
{
    switch (channel)
    {
        case Channel::DAC_A: m_txBuffer.tx_dacA = constrain(set_value, 0, Fullscale); break;
        case Channel::DAC_B: m_txBuffer.tx_dacB = constrain(set_value, 0, Fullscale); break;
    }
}