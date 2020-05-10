/******************************************************************************
 * SFE_VL6180.cpp
 * Library for VL6180 time of flight range finder.
 * Casey Kuhns @ SparkFun Electronics
 * 10/29/2014
 * https://github.com/sparkfun/
 *
 * The VL6180 by ST micro is a time of flight range finder that
 * uses pulsed IR light to determine distances from object at close
 * range.  The average range of a sensor is between 0-200mm
 *
 * In this file are the functions in the VL6180 class
 *
 * Resources:
 * This library uses the Arduino Wire.h to complete I2C transactions.
 *
 * Development environment specifics:
 *  IDE: Arduino 1.0.5
 *  Hardware Platform: Arduino Pro 3.3V/8MHz
 *  VL6180 Breakout Version: 1.0
 *
 *
 * This code is beerware. If you see me (or any other SparkFun employee) at the
 * local pub, and you've found our code helpful, please buy us a round!
 *
 * Distributed as-is; no warranty is given.
 ******************************************************************************/

#include "VL6180.h"

//
// Constructors
//
VL6180::VL6180(PinName sda, PinName scl)  : i2c(sda, scl)
{
    VL6180_i2cAddress = VL6180_DEF_ADDR;
}

VL6180::VL6180(PinName sda, PinName scl, int i2cAddress)  : i2c(sda, scl)
{
    VL6180_i2cAddress = i2cAddress;
    VL6180_error_no = 0;
}
//
// destructor
//
VL6180::~VL6180()
{
}

int8_t VL6180::VL6180_Init(void)
{
    uint8_t data; //for temp data storage

    data = VL6180_getRegister(VL6180_SYSTEM_FRESH_OUT_OF_RESET);

    if(data != 1) return VL6180_FAILURE_RESET;

    //Required by datasheet
    //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
    VL6180_setRegister(0x0207, 0x01);
    VL6180_setRegister(0x0208, 0x01);
    VL6180_setRegister(0x0096, 0x00);
    VL6180_setRegister(0x0097, 0xfd);
    VL6180_setRegister(0x00e3, 0x00);
    VL6180_setRegister(0x00e4, 0x04);
    VL6180_setRegister(0x00e5, 0x02);
    VL6180_setRegister(0x00e6, 0x01);
    VL6180_setRegister(0x00e7, 0x03);
    VL6180_setRegister(0x00f5, 0x02);
    VL6180_setRegister(0x00d9, 0x05);
    VL6180_setRegister(0x00db, 0xce);
    VL6180_setRegister(0x00dc, 0x03);
    VL6180_setRegister(0x00dd, 0xf8);
    VL6180_setRegister(0x009f, 0x00);
    VL6180_setRegister(0x00a3, 0x3c);
    VL6180_setRegister(0x00b7, 0x00);
    VL6180_setRegister(0x00bb, 0x3c);
    VL6180_setRegister(0x00b2, 0x09);
    VL6180_setRegister(0x00ca, 0x09);
    VL6180_setRegister(0x0198, 0x01);
    VL6180_setRegister(0x01b0, 0x17);
    VL6180_setRegister(0x01ad, 0x00);
    VL6180_setRegister(0x00ff, 0x05);
    VL6180_setRegister(0x0100, 0x05);
    VL6180_setRegister(0x0199, 0x05);
    VL6180_setRegister(0x01a6, 0x1b);
    VL6180_setRegister(0x01ac, 0x3e);
    VL6180_setRegister(0x01a7, 0x1f);
    VL6180_setRegister(0x0030, 0x00);

    return 0;
}

void VL6180::VL6180_DefautSettings(void)
{
    //Recommended settings from datasheet
    //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf

    //Enable Interrupts on Conversion Complete (any source)
    VL6180_setRegister(VL6180_SYSTEM_INTERRUPT_CONFIG_GPIO, (4 << 3)|(4) ); // Set GPIO1 high when sample complete


    VL6180_setRegister(VL6180_SYSTEM_MODE_GPIO1, 0x10); // Set GPIO1 high when sample complete
    VL6180_setRegister(VL6180_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30); //Set Avg sample period
    VL6180_setRegister(VL6180_SYSALS_ANALOGUE_GAIN, 0x46); // Set the ALS gain
    VL6180_setRegister(VL6180_SYSRANGE_VHV_REPEAT_RATE, 0xFF); // Set auto calibration period (Max = 255)/(OFF = 0)
    VL6180_setRegister(VL6180_SYSALS_INTEGRATION_PERIOD, 0x63); // Set ALS integration time to 100ms
    VL6180_setRegister(VL6180_SYSRANGE_VHV_RECALIBRATE, 0x01); // perform a single temperature calibration
    //Optional settings from datasheet
    //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
    VL6180_setRegister(VL6180_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x09); // Set default ranging inter-measurement period to 100ms
    VL6180_setRegister(VL6180_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A); // Set default ALS inter-measurement period to 100ms
    VL6180_setRegister(VL6180_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24); // Configures interrupt on â€˜New Sample Ready threshold eventâ€™
    //Additional settings defaults from community
    VL6180_setRegister(VL6180_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32);
    VL6180_setRegister(VL6180_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);
    VL6180_setRegister16bit(VL6180_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x7B );
    VL6180_setRegister16bit(VL6180_SYSALS_INTEGRATION_PERIOD, 0x64);

    VL6180_setRegister(VL6180_READOUT_AVERAGING_SAMPLE_PERIOD,0x30);
    VL6180_setRegister(VL6180_SYSALS_ANALOGUE_GAIN,0x40);
    VL6180_setRegister(VL6180_FIRMWARE_RESULT_SCALER,0x01);
}
void VL6180::getIdentification(struct VL6180Identification *temp)
{

    temp->idModel =  VL6180_getRegister(VL6180_IDENTIFICATION_MODEL_ID);
    temp->idModelRevMajor = VL6180_getRegister(VL6180_IDENTIFICATION_MODEL_REV_MAJOR);
    temp->idModelRevMinor = VL6180_getRegister(VL6180_IDENTIFICATION_MODEL_REV_MINOR);
    temp->idModuleRevMajor = VL6180_getRegister(VL6180_IDENTIFICATION_MODULE_REV_MAJOR);
    temp->idModuleRevMinor = VL6180_getRegister(VL6180_IDENTIFICATION_MODULE_REV_MINOR);

    temp->idDate = VL6180_getRegister16bit(VL6180_IDENTIFICATION_DATE);
    temp->idTime = VL6180_getRegister16bit(VL6180_IDENTIFICATION_TIME);
}


uint8_t VL6180::changeAddress(uint8_t old_address, uint8_t new_address)
{

    //NOTICE:  IT APPEARS THAT CHANGING THE ADDRESS IS NOT STORED IN NON-VOLATILE MEMORY
    // POWER CYCLING THE DEVICE REVERTS ADDRESS BACK TO 0X29

    if( old_address == new_address) return old_address;
    if( new_address > 127) return old_address;

    VL6180_setRegister(VL6180_I2C_SLAVE_DEVICE_ADDRESS, new_address);

    return VL6180_getRegister(VL6180_I2C_SLAVE_DEVICE_ADDRESS);
}



uint8_t VL6180::getDistance()
{
    VL6180_setRegister(VL6180_SYSRANGE_START, 0x01); //Start Single shot mode
    wait(0.01);    // 10mS
    return VL6180_getRegister(VL6180_RESULT_RANGE_VAL);
//    VL6180_setRegister(VL6180_SYSTEM_INTERRUPT_CLEAR, 0x07);
//    return distance;
}

float VL6180::getAmbientLight(VL6180_als_gain VL6180_ALS_GAIN)
{
    //First load in Gain we are using, do it everytime incase someone changes it on us.
    //Note: Upper nibble shoudl be set to 0x4 i.e. for ALS gain of 1.0 write 0x46
    VL6180_setRegister(VL6180_SYSALS_ANALOGUE_GAIN, (0x40 | VL6180_ALS_GAIN)); // Set the ALS gain

    //Start ALS Measurement
    VL6180_setRegister(VL6180_SYSALS_START, 0x01);

    wait(0.1); //100Ms

    VL6180_setRegister(VL6180_SYSTEM_INTERRUPT_CLEAR, 0x07);

    //Retrieve the Raw ALS value from the sensoe
    unsigned int alsRaw = VL6180_getRegister16bit(VL6180_RESULT_ALS_VAL);

    //Get Integration Period for calculation, we do this everytime incase someone changes it on us.
    unsigned int alsIntegrationPeriodRaw = VL6180_getRegister16bit(VL6180_SYSALS_INTEGRATION_PERIOD);

    float alsIntegrationPeriod = 100.0 / alsIntegrationPeriodRaw ;

    //Calculate actual LUX from Appnotes

    float alsGain = 0.0;

    switch (VL6180_ALS_GAIN) {
        case GAIN_20:
            alsGain = 20.0;
            break;
        case GAIN_10:
            alsGain = 10.32;
            break;
        case GAIN_5:
            alsGain = 5.21;
            break;
        case GAIN_2_5:
            alsGain = 2.60;
            break;
        case GAIN_1_67:
            alsGain = 1.72;
            break;
        case GAIN_1_25:
            alsGain = 1.28;
            break;
        case GAIN_1:
            alsGain = 1.01;
            break;
        case GAIN_40:
            alsGain = 40.0;
            break;
    }

//Calculate LUX from formula in AppNotes

    float alsCalculated = (float)0.32 * ((float)alsRaw / alsGain) * alsIntegrationPeriod;

    return alsCalculated;
}

// --- Private Functions --- //

uint8_t VL6180::VL6180_getRegister(uint16_t reg_address)
{
    char data[2];

    data[0] = (reg_address >> 8) & 0xFF; //MSB of register address
    data[1] = reg_address & 0xFF; //LSB of register address
    VL6180_error_no = i2c.write(VL6180_i2cAddress, data, 2);
    VL6180_error_no = i2c.read(VL6180_i2cAddress, data, 1, false);
    return data[0];
}

uint16_t VL6180::VL6180_getRegister16bit(uint16_t reg_address)
{
    char data[2];

    data[0] = (reg_address >> 8) & 0xFF; //MSB of register address
    data[1] = reg_address & 0xFF; //LSB of register address
    VL6180_error_no = i2c.write(VL6180_i2cAddress, data, 2);
    VL6180_error_no = i2c.read(VL6180_i2cAddress, data, 2, false);
    return (data[0] + ((data[1] << 8) & 0xFF00));
}

void VL6180::VL6180_setRegister(uint16_t reg_address, uint8_t value)
{
    char  data[3];

    data[0] = (reg_address >> 8) & 0xFF;    //MSB of register address
    data[1] = reg_address & 0xFF;           //LSB of register address
    data[2] = value;
    VL6180_error_no = VL6180_error_no = i2c.write(VL6180_i2cAddress, data, 3);
}

void VL6180::VL6180_setRegister16bit(uint16_t reg_address, uint16_t value)
{
    char  data[4];

    data[0] = (reg_address >> 8) & 0xFF;    //MSB of register address
    data[1] = reg_address & 0xFF;           //LSB of register address
    data[2] = value & 0xFF;
    data[3] = ((value >> 8) & 0xFF);
    VL6180_error_no = VL6180_error_no = i2c.write(VL6180_i2cAddress, data, 4);
}

int VL6180::writeSingleRegister( uint16_t reg_address, uint8_t data )
{

    char data_write[3];
    data_write[0] = (reg_address >> 8) & 0xFF; //MSB of register address
    data_write[1] = reg_address & 0xFF; //LSB of register address
    data_write[2] = data & 0xFF;
    return i2c.write(VL6180_DEF_ADDR, data_write, 3);

//   char tx[2] = { address | 160, data }; //0d160 = 0b10100000
//   int ack = i2c.write( SLAVE_ADDRESS << 1, tx, 2 );
//   return ack;
}