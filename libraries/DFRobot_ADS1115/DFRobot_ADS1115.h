#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#define ADS1115_IIC_ADDRESS0         (0x48)
#define ADS1115_IIC_ADDRESS1         (0x49)
#define ADS1115_CONVERSIONDELAY         (100)
#define ADS1115_REG_POINTER_MASK        (0x03)
#define ADS1115_REG_POINTER_CONVERT     (0x00)
#define ADS1115_REG_POINTER_CONFIG      (0x01)
#define ADS1115_REG_POINTER_LOWTHRESH   (0x02)
#define ADS1115_REG_POINTER_HITHRESH    (0x03)

#define ADS1115_REG_CONFIG_OS_MASK      (0x8000)    // Operational status/single-shot conversion start
#define ADS1115_REG_CONFIG_OS_NOEFFECT  (0x0000)    // Write: Bit = 0 No effect
#define ADS1115_REG_CONFIG_OS_SINGLE    (0x8000)    // Write: Bit = 1 Begin a single conversion
#define ADS1115_REG_CONFIG_OS_BUSY      (0x0000)    // Read: Bit = 0 Device is performing a conversion
#define ADS1115_REG_CONFIG_OS_NOTBUSY   (0x8000)    // Read: Bit = 1 Device is not performing a conversion

#define ADS1115_REG_CONFIG_MUX_MASK     (0x7000)    // Input multiplexer configuration
#define ADS1115_REG_CONFIG_MUX_DIFF_0_1 (0x0000)    // Differential P = AIN0, N = AIN1 (default)
#define ADS1115_REG_CONFIG_MUX_DIFF_0_3 (0x1000)    // Differential P = AIN0, N = AIN3
#define ADS1115_REG_CONFIG_MUX_DIFF_1_3 (0x2000)    // Differential P = AIN1, N = AIN3
#define ADS1115_REG_CONFIG_MUX_DIFF_2_3 (0x3000)    // Differential P = AIN2, N = AIN3
#define ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4000)    // Single-ended P = AIN0, N = GND
#define ADS1115_REG_CONFIG_MUX_SINGLE_1 (0x5000)    // Single-ended P = AIN1, N = GND
#define ADS1115_REG_CONFIG_MUX_SINGLE_2 (0x6000)    // Single-ended P = AIN2, N = GND
#define ADS1115_REG_CONFIG_MUX_SINGLE_3 (0x7000)    // Single-ended P = AIN3, N = GND

#define ADS1115_REG_CONFIG_PGA_MASK     (0x0E00)    // Programmable gain amplifier configuration
#define ADS1115_REG_CONFIG_PGA_6_144V   (0x0000)    // +/-6.144V range = Gain 2/3
#define ADS1115_REG_CONFIG_PGA_4_096V   (0x0200)    // +/-4.096V range = Gain 1
#define ADS1115_REG_CONFIG_PGA_2_048V   (0x0400)    // +/-2.048V range = Gain 2 (default)
#define ADS1115_REG_CONFIG_PGA_1_024V   (0x0600)    // +/-1.024V range = Gain 4
#define ADS1115_REG_CONFIG_PGA_0_512V   (0x0800)    // +/-0.512V range = Gain 8
#define ADS1115_REG_CONFIG_PGA_0_256V   (0x0A00)    // +/-0.256V range = Gain 16

#define ADS1115_REG_CONFIG_MODE_MASK    (0x0100)    // Device operating mode
#define ADS1115_REG_CONFIG_MODE_CONTIN  (0x0000)    // Continuous conversion mode
#define ADS1115_REG_CONFIG_MODE_SINGLE  (0x0100)    // Power-down single-shot mode (default)

#define ADS1115_REG_CONFIG_DR_MASK      (0x00E0)    // Data rate
#define ADS1115_REG_CONFIG_DR_8SPS      (0x0000)    // 8 samples per second
#define ADS1115_REG_CONFIG_DR_16SPS     (0x0020)    // 16 samples per second
#define ADS1115_REG_CONFIG_DR_32SPS     (0x0040)    // 32 samples per second
#define ADS1115_REG_CONFIG_DR_64SPS     (0x0060)    // 64 samples per second
#define ADS1115_REG_CONFIG_DR_128SPS    (0x0080)    // 128 samples per second (default)
#define ADS1115_REG_CONFIG_DR_250SPS    (0x00A0)    // 250 samples per second
#define ADS1115_REG_CONFIG_DR_475SPS    (0x00C0)    // 475 samples per second
#define ADS1115_REG_CONFIG_DR_860SPS    (0x00E0)    // 860 samples per second

#define ADS1115_REG_CONFIG_CMODE_MASK   (0x0010)    // Comparator mode
#define ADS1115_REG_CONFIG_CMODE_TRAD   (0x0000)    // Traditional comparator with hysteresis (default)
#define ADS1115_REG_CONFIG_CMODE_WINDOW (0x0010)    // Window comparator

#define ADS1115_REG_CONFIG_CPOL_MASK    (0x0008)    // Comparator polarity
#define ADS1115_REG_CONFIG_CPOL_ACTVLOW (0x0000)    // ALERT/RDY pin is low when active (default)
#define ADS1115_REG_CONFIG_CPOL_ACTVHI  (0x0008)    // ALERT/RDY pin is high when active

#define ADS1115_REG_CONFIG_CLAT_MASK    (0x0004)    // Latching comparator
#define ADS1115_REG_CONFIG_CLAT_NONLAT  (0x0000)    // Non-latching comparator (default)
#define ADS1115_REG_CONFIG_CLAT_LATCH   (0x0004)    // Latching comparator

#define ADS1115_REG_CONFIG_CQUE_MASK    (0x0003)    // Comparator queue and disable
#define ADS1115_REG_CONFIG_CQUE_1CONV   (0x0000)    // Assert ALERT/RDY after one conversions
#define ADS1115_REG_CONFIG_CQUE_2CONV   (0x0001)    // Assert ALERT/RDY after two conversions
#define ADS1115_REG_CONFIG_CQUE_4CONV   (0x0002)    // Assert ALERT/RDY after four conversions
#define ADS1115_REG_CONFIG_CQUE_NONE    (0x0003)    // Disable the comparator and put ALERT/RDY in high state (default)

typedef enum
{
    eOSMODE_SINGLE       = ADS1115_REG_CONFIG_OS_SINGLE,
    eOSMODE_BUSY         = ADS1115_REG_CONFIG_OS_BUSY,
    eOSMODE_NOTBUSY      = ADS1115_REG_CONFIG_OS_NOTBUSY
    
} eADSOSMode_t;

typedef enum
{
    eGAIN_TWOTHIRDS      = ADS1115_REG_CONFIG_PGA_6_144V,
    eGAIN_ONE            = ADS1115_REG_CONFIG_PGA_4_096V,
    eGAIN_TWO            = ADS1115_REG_CONFIG_PGA_2_048V,
    eGAIN_FOUR           = ADS1115_REG_CONFIG_PGA_1_024V,
    eGAIN_EIGHT          = ADS1115_REG_CONFIG_PGA_0_512V,
    eGAIN_SIXTEEN        = ADS1115_REG_CONFIG_PGA_0_256V
} eADSGain_t;

typedef enum
{
    eMODE_CONTIN         = ADS1115_REG_CONFIG_MODE_CONTIN,
    eMODE_SINGLE         = ADS1115_REG_CONFIG_MODE_SINGLE
} eADSMode_t;

typedef enum
{
    eRATE_8              = ADS1115_REG_CONFIG_DR_8SPS,
    eRATE_16             = ADS1115_REG_CONFIG_DR_16SPS,
    eRATE_32             = ADS1115_REG_CONFIG_DR_32SPS,
    eRATE_64             = ADS1115_REG_CONFIG_DR_64SPS,
    eRATE_128            = ADS1115_REG_CONFIG_DR_128SPS,
    eRATE_250            = ADS1115_REG_CONFIG_DR_250SPS,
    eRATE_475            = ADS1115_REG_CONFIG_DR_475SPS,
    eRATE_860            = ADS1115_REG_CONFIG_DR_860SPS
} eADSRate_t;

typedef enum
{
    eCOMPMODE_TRAD       = ADS1115_REG_CONFIG_CMODE_TRAD,
    eCOMPMODE_WINDOW     = ADS1115_REG_CONFIG_CMODE_WINDOW
} eADSCompMode_t;

typedef enum
{
    eCOMPPOL_LOW         = ADS1115_REG_CONFIG_CPOL_ACTVLOW,
    eCOMPPOL_HIGH        = ADS1115_REG_CONFIG_CPOL_ACTVHI
} eADSCompPol_t;

typedef enum
{
    eCOMPLAT_NONLAT      = ADS1115_REG_CONFIG_CLAT_NONLAT,
    eCOMPLAT_LATCH       = ADS1115_REG_CONFIG_CLAT_LATCH
} eADSCompLat_t;

typedef enum
{
    eCOMPQUE_ONE         = ADS1115_REG_CONFIG_CQUE_1CONV,
    eCOMPQUE_TWO         = ADS1115_REG_CONFIG_CQUE_2CONV,
    eCOMPQUE_FOUR        = ADS1115_REG_CONFIG_CQUE_4CONV,
    eCOMPQUE_NONE        = ADS1115_REG_CONFIG_CQUE_NONE
} eADSCompQue_t;

class DFRobot_ADS1115
{
    protected:
        // Instance-specific properties
        uint8_t ads_conversionDelay;
        int16_t ads_lowthreshold;
        int16_t ads_highthreshold;
        eADSOSMode_t ads_osmode;
        eADSGain_t ads_gain;
        eADSMode_t ads_mode;
        eADSRate_t ads_rate;
        eADSCompMode_t ads_compmode;
        eADSCompPol_t ads_comppol;
        eADSCompLat_t ads_complat;
        eADSCompQue_t ads_compque;

    public:
        uint8_t ads_i2cAddress;
        void begin(uint8_t i2cAddress);
        void init(void);
        int16_t readVoltage(uint8_t channel);
        int16_t ComparatorVoltage(uint8_t channel);
        int16_t getLastConversionResults();
        void setOSMode(eADSOSMode_t osmode);
        eADSOSMode_t getOSMode(void);
        void setGain(eADSGain_t gain);
        eADSGain_t getGain(void);
        void setMode(eADSMode_t mode);
        eADSMode_t getMode(void);
        void setRate(eADSRate_t rate);
        eADSRate_t getRate(void);
        void setCompMode(eADSCompMode_t compmode);
        eADSCompMode_t getCompMode(void);
        void setCompPol(eADSCompPol_t comppol);
        eADSCompPol_t getCompPol(void);
        void setCompLat(eADSCompLat_t complat);
        eADSCompLat_t getCompLat(void);
        void setCompQue(eADSCompQue_t compque);
        eADSCompQue_t getCompQue(void);
        void setLowThreshold(int16_t threshold);
        int16_t   getLowThreshold();
        void setHighThreshold(int16_t threshold);
        int16_t   getHighThreshold();

    private:
};
