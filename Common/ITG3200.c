/**
 * @file        ITG3200.c
 * @author      Luis Maduro
 * @author      Jeff Rowberg
 * @version     1.0
 * @date        2011
 * @copyright	MIT license
 */
#include "ITG3200.h"

unsigned char ITG3200Buffer[6];

/**
 * Power on and prepare for general usage.
 * This will activate the gyroscope, so be sure to adjust the power settings
 * after you call this method if you want it to enter standby mode, or another
 * less demanding mode of operation. This also sets the gyroscope to use the
 * X-axis gyro for a clock source. Note that it doesn't have any delays in the
 * routine, which means you might want to add ~50ms to be safe if you happen
 * to need to read gyro data immediately after initialization. The data will
 * flow in either case, but the first reports may have higher error offsets.
 */
void ITG3200Initialize(void)
{
    ITG3200SetFullScaleRange(ITG3200_FULLSCALE_2000);
    ITG3200SetClockSource(ITG3200_CLOCK_PLL_XGYRO);
}

/**
 * Get Device ID.
 * This register is used to verify the identity of the device (0b110100).
 * @return Device ID (should be 0x34, 52 dec, 64 oct)
 * @see ITG3200_RA_WHO_AM_I
 * @see ITG3200_RA_DEVID_BIT
 * @see ITG3200_RA_DEVID_LENGTH
 */
unsigned char ITG3200GetDeviceID(void)
{
    return I2CDeviceReadBits(ITG3200_RA_WHO_AM_I,
                             ITG3200_DEVID_BIT,
                             ITG3200_DEVID_LENGTH);
}

/**
 * Set Device ID.
 * Write a new ID into the WHO_AM_I register (no idea why this should ever be
 * necessary though).
 * @param id New device ID to set.
 * @see ITG3200GetDeviceID()
 * @see ITG3200_RA_WHO_AM_I
 * @see ITG3200_RA_DEVID_BIT
 * @see ITG3200_RA_DEVID_LENGTH
 */
void ITG3200SetDeviceID(unsigned char id)
{
    I2CDeviceWriteBits(ITG3200_RA_WHO_AM_I,
                       ITG3200_DEVID_BIT,
                       ITG3200_DEVID_LENGTH,
                       id);
}

/**
 * Get sample rate.
 * This register determines the sample rate of the ITG-3200 gyros. The gyros'
 * outputs are sampled internally at either 1kHz or 8kHz, determined by the
 * DLPF_CFG setting (see register 22). This sampling is then filtered digitally
 * and delivered into the sensor registers after the number of cycles determined
 * by this register. The sample rate is given by the following formula:
 *
 * F_sample = F_internal / (divider+1), where F_internal is either 1kHz or 8kHz
 *
 * As an example, if the internal sampling is at 1kHz, then setting this
 * register to 7 would give the following:
 *
 * F_sample = 1kHz / (7 + 1) = 125Hz, or 8ms per sample
 *
 * @return Current sample rate
 * @see ITG3200SetDLPFBandwidth()
 * @see ITG3200GetDLPFBandwidth()
 * @see ITG3200SetRate()
 * @see ITG3200_RA_SMPLRT_DIV
 */
unsigned char ITG3200GetRate(void)
{
    return I2CDeviceReadByte(ITG3200_RA_SMPLRT_DIV);
}

/**
 * Set sample rate.
 * @param rate New sample rate
 * @see ITG3200GetRate()
 * @see ITG3200SetDLPFBandwidth()
 * @see ITG3200GetDLPFBandwidth()
 * @see ITG3200_RA_SMPLRT_DIV
 */
void ITG3200SetRate(unsigned char rate)
{
    I2CDeviceWriteByte(ITG3200_RA_SMPLRT_DIV, rate);
}

/**
 * Set full-scale range.
 * The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
 * as described in the table below. The power-on-reset value of FS_SEL is 00h.
 * Set to 03h for proper operation.
 *
 * 0 = Reserved
 * 1 = Reserved
 * 2 = Reserved
 * 3 = +/- 2000 degrees/sec
 *
 * @return Current full-scale range setting
 * @see ITG3200_FULLSCALE_2000
 * @see ITG3200_RA_DLPF_FS
 * @see ITG3200_DF_FS_SEL_BIT
 * @see ITG3200_DF_FS_SEL_LENGTH
 */
unsigned char ITG3200GetFullScaleRange(void)
{
    return I2CDeviceReadBits(ITG3200_RA_DLPF_FS,
                             ITG3200_DF_FS_SEL_BIT,
                             ITG3200_DF_FS_SEL_LENGTH);
}

/**
 * Set full-scale range setting.
 * @param range New full-scale range value
 * @see ITG3200_FULLSCALE_2000
 * @see ITG3200_RA_DLPF_FS
 * @see ITG3200_DF_FS_SEL_BIT
 * @see ITG3200_DF_FS_SEL_LENGTH
 */
void ITG3200SetFullScaleRange(unsigned char range)
{
    I2CDeviceWriteBits(ITG3200_RA_DLPF_FS,
                       ITG3200_DF_FS_SEL_BIT,
                       ITG3200_DF_FS_SEL_LENGTH,
                       range);
}

/**
 * Get digital low-pass filter bandwidth.
 * The DLPF_CFG parameter sets the digital low pass filter configuration. It
 * also determines the internal sampling rate used by the device as shown in
 * the table below.
 *
 * DLPF_CFG | Low-Pass Filter Bandwidth | Internal Sample Rate
 * ---------+---------------------------+---------------------
 * 0        | 256Hz                     | 8kHz
 * 1        | 188Hz                     | 1kHz
 * 2        | 98Hz                      | 1kHz
 * 3        | 42Hz                      | 1kHz
 * 4        | 20Hz                      | 1kHz
 * 5        | 10Hz                      | 1kHz
 * 6        | 5Hz                       | 1kHz
 * 7        | Reserved                  | Reserved
 *
 * @return DLFP bandwidth setting
 * @see ITG3200SetDLPFBandwidth()
 * @see ITG3200_RA_DLPF_FS
 * @see ITG3200_DF_DLPF_CFG_BIT
 * @see ITG3200_DF_DLPF_CFG_LENGTH
 */
unsigned char ITG3200GetDLPFBandwidth(void)
{
    return I2CDeviceReadBits(ITG3200_RA_DLPF_FS,
                             ITG3200_DF_DLPF_CFG_BIT,
                             ITG3200_DF_DLPF_CFG_LENGTH);
}

/**
 * Set digital low-pass filter bandwidth.
 * @param bandwidth New DLFP bandwidth setting
 * @see ITG3200GetDLPFBandwidth()
 * @see ITG3200_DLPF_BW_256
 * @see ITG3200_RA_DLPF_FS
 * @see ITG3200_DF_DLPF_CFG_BIT
 * @see ITG3200_DF_DLPF_CFG_LENGTH
 */
void ITG3200SetDLPFBandwidth(unsigned char bandwidth)
{
    I2CDeviceWriteBits(ITG3200_RA_DLPF_FS,
                       ITG3200_DF_DLPF_CFG_BIT,
                       ITG3200_DF_DLPF_CFG_LENGTH,
                       bandwidth);
}

/**
 * Get interrupt logic level mode.
 * Will be set 0 for active-high, 1 for active-low.
 * @return Current interrupt mode (0=active-high, 1=active-low)
 * @see ITG3200SetInterruptMode()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_ACTL_BIT
 */
boolean ITG3200GetInterruptMode(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_CFG,
                            ITG3200_INTCFG_ACTL_BIT);
}

/**
 * Set interrupt logic level mode.
 * @param mode New interrupt mode (0=active-high, 1=active-low)
 * @see ITG3200GetInterruptMode()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_ACTL_BIT
 */
void ITG3200SetInterruptMode(boolean mode)
{
    I2CDeviceWriteBit(ITG3200_RA_INT_CFG,
                      ITG3200_INTCFG_ACTL_BIT,
                      mode);
}

/**
 * Get interrupt drive mode.
 * Will be set 0 for push-pull, 1 for open-drain.
 * @return Current interrupt drive mode (0=push-pull, 1=open-drain)
 * @see ITG3200SetInterruptDrive()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_OPEN_BIT
 */
boolean ITG3200getInterruptDrive(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_CFG,
                            ITG3200_INTCFG_OPEN_BIT);
}

/**
 * Set interrupt drive mode.
 * @param drive New interrupt drive mode (0=push-pull, 1=open-drain)
 * @see ITG3200getInterruptDrive()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_OPEN_BIT
 */
void ITG3200SetInterruptDrive(boolean drive)
{
    I2CDeviceWriteBit(ITG3200_RA_INT_CFG,
                      ITG3200_INTCFG_OPEN_BIT,
                      drive);
}

/**
 * Get interrupt latch mode.
 * Will be set 0 for 50us-pulse, 1 for latch-until-int-cleared.
 * @return Current latch mode (0=50us-pulse, 1=latch-until-int-cleared)
 * @see ITG3200SetInterruptLatch()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_LATCH_INT_EN_BIT
 */
boolean ITG3200GetInterruptLatch(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_CFG,
                            ITG3200_INTCFG_LATCH_INT_EN_BIT);
}

/**
 * Set interrupt latch mode.
 * @param latch New latch mode (0=50us-pulse, 1=latch-until-int-cleared)
 * @see ITG3200GetInterruptLatch()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_LATCH_INT_EN_BIT
 */
void ITG3200SetInterruptLatch(boolean latch)
{
    I2CDeviceWriteBit(ITG3200_RA_INT_CFG,
                      ITG3200_INTCFG_LATCH_INT_EN_BIT,
                      latch);
}

/**
 * Get interrupt latch clear mode.
 * Will be set 0 for status-read-only, 1 for any-register-read.
 * @return Current latch clear mode (0=status-read-only, 1=any-register-read)
 * @see ITG3200SetInterruptLatchClear()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_INT_ANYRD_2CLEAR_BIT
 */
boolean ITG3200GetInterruptLatchClear(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_CFG,
                            ITG3200_INTCFG_INT_ANYRD_2CLEAR_BIT);
}

/**
 * Set interrupt latch clear mode.
 * @param clear New latch clear mode (0=status-read-only, 1=any-register-read)
 * @see ITG3200GetInterruptLatchClear()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_INT_ANYRD_2CLEAR_BIT
 */
void ITG3200SetInterruptLatchClear(boolean clear)
{
    I2CDeviceWriteBit(ITG3200_RA_INT_CFG,
                      ITG3200_INTCFG_INT_ANYRD_2CLEAR_BIT,
                      clear);
}

/**
 * Get "device ready" interrupt enabled setting.
 * Will be set 0 for disabled, 1 for enabled.
 * @return Current interrupt enabled setting
 * @see ITG3200SetIntDeviceReadyEnabled()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_ITG_RDY_EN_BIT
 */
boolean ITG3200GetIntDeviceReadyEnabled(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_CFG,
                            ITG3200_INTCFG_ITG_RDY_EN_BIT);
}

/**
 * Set "device ready" interrupt enabled setting.
 * @param enabled New interrupt enabled setting
 * @see ITG3200GetIntDeviceReadyEnabled()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_ITG_RDY_EN_BIT
 */
void ITG3200SetIntDeviceReadyEnabled(boolean enabled)
{
    I2CDeviceWriteBit(ITG3200_RA_INT_CFG,
                      ITG3200_INTCFG_ITG_RDY_EN_BIT,
                      enabled);
}

/**
 * Get "data ready" interrupt enabled setting.
 * Will be set 0 for disabled, 1 for enabled.
 * @return Current interrupt enabled setting
 * @see ITG3200SetIntDataReadyEnabled()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_RAW_RDY_EN_BIT
 */
boolean ITG3200GetIntDataReadyEnabled(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_CFG,
                            ITG3200_INTCFG_RAW_RDY_EN_BIT);
}

/**
 * Set "data ready" interrupt enabled setting.
 * @param enabled New interrupt enabled setting
 * @see ITG3200GetIntDataReadyEnabled()
 * @see ITG3200_RA_INT_CFG
 * @see ITG3200_INTCFG_RAW_RDY_EN_BIT
 */
void ITG3200SetIntDataReadyEnabled(boolean enabled)
{
    I2CDeviceWriteBit(ITG3200_RA_INT_CFG,
                      ITG3200_INTCFG_RAW_RDY_EN_BIT,
                      enabled);
}

/**
 * Get Device Ready interrupt status.
 * The ITG_RDY interrupt indicates that the PLL is ready and gyroscopic data can
 * be read.
 * @return Device Ready interrupt status
 * @see ITG3200_RA_INT_STATUS
 * @see ITG3200_INTSTAT_RAW_DATA_READY_BIT
 */
boolean ITG3200GetIntDeviceReadyStatus(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_STATUS,
                            ITG3200_INTSTAT_ITG_RDY_BIT);
}

/**
 * Get Data Ready interrupt status.
 * In normal use, the RAW_DATA_RDY interrupt is used to determine when new
 * sensor data is available in and of the sensor registers (27 to 32).
 * @return Data Ready interrupt status
 * @see ITG3200_RA_INT_STATUS
 * @see ITG3200_INTSTAT_RAW_DATA_READY_BIT
 */
boolean ITG3200GetIntDataReadyStatus(void)
{
    return I2CDeviceReadBit(ITG3200_RA_INT_STATUS,
                            ITG3200_INTSTAT_RAW_DATA_READY_BIT);
}

/**
 * Get current internal temperature.
 * @return Temperature reading in 16-bit 2's complement format
 * @see ITG3200_RA_TEMP_OUT_H
 */
int ITG3200GetTemperature(void)
{
    I2CDeviceReadBytes(ITG3200_RA_TEMP_OUT_H, 2, ITG3200Buffer);
    return (((int) ITG3200Buffer[0]) << 8) | ITG3200Buffer[1];
}

/**
 * Get 3-axis gyroscope readings.
 * @param x 16-bit signed integer container for X-axis rotation
 * @param y 16-bit signed integer container for Y-axis rotation
 * @param z 16-bit signed integer container for Z-axis rotation
 * @see ITG3200_RA_GYRO_XOUT_H
 */
void ITG3200GetRotation(int* x, int* y, int* z)
{
    I2CDeviceReadBytes(ITG3200_RA_GYRO_XOUT_H, 6, ITG3200Buffer);
    *x = (((int) ITG3200Buffer[0]) << 8) | ITG3200Buffer[1];
    *y = (((int) ITG3200Buffer[2]) << 8) | ITG3200Buffer[3];
    *z = (((int) ITG3200Buffer[4]) << 8) | ITG3200Buffer[5];
}

/**
 * Get X-axis gyroscope reading.
 * @return X-axis rotation measurement in 16-bit 2's complement format
 * @see ITG3200_RA_GYRO_XOUT_H
 */
int ITG3200GetRotationX(void)
{
    I2CDeviceReadBytes(ITG3200_RA_GYRO_XOUT_H, 2, ITG3200Buffer);
    return (((int) ITG3200Buffer[0]) << 8) | ITG3200Buffer[1];
}

/**
 * Get Y-axis gyroscope reading.
 * @return Y-axis rotation measurement in 16-bit 2's complement format
 * @see ITG3200_RA_GYRO_YOUT_H
 */
int ITG3200GetRotationY(void)
{
    I2CDeviceReadBytes(ITG3200_RA_GYRO_YOUT_H, 2, ITG3200Buffer);
    return (((int) ITG3200Buffer[0]) << 8) | ITG3200Buffer[1];
}

/**
 * Get Z-axis gyroscope reading.
 * @return Z-axis rotation measurement in 16-bit 2's complement format
 * @see ITG3200_RA_GYRO_ZOUT_H
 */
int ITG3200GetRotationZ(void)
{
    I2CDeviceReadBytes(ITG3200_RA_GYRO_ZOUT_H, 2, ITG3200Buffer);
    return (((int) ITG3200Buffer[0]) << 8) | ITG3200Buffer[1];
}

/**
 * Trigger a full device reset.
 * A small delay of ~50ms may be desirable after triggering a reset.
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_H_RESET_BIT
 */
void ITG3200Reset(void)
{
    I2CDeviceWriteBit(ITG3200_RA_PWR_MGM,
                      ITG3200_PWR_H_RESET_BIT, true);
}

/**
 * Get sleep mode status.
 * Setting the SLEEP bit in the register puts the device into very low power
 * sleep mode. In this mode, only the serial interface and internal registers
 * remain active, allowing for a very low standby current. Clearing this bit
 * puts the device back into normal mode. To save power, the individual standby
 * selections for each of the gyros should be used if any gyro axis is not used
 * by the application.
 * @return Current sleep mode enabled status
 * @see ITG3200SetSleepEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_SLEEP_BIT
 */
boolean ITG3200GetSleepEnabled(void)
{
    return I2CDeviceReadBit(ITG3200_RA_PWR_MGM,
                            ITG3200_PWR_SLEEP_BIT);
}

/**
 * Set sleep mode status.
 * @param enabled New sleep mode enabled status
 * @see ITG3200GetSleepEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_SLEEP_BIT
 */
void ITG3200SetSleepEnabled(boolean enabled)
{
    I2CDeviceWriteBit(ITG3200_RA_PWR_MGM,
                      ITG3200_PWR_SLEEP_BIT,
                      enabled);
}

/**
 * Get X-axis standby enabled status.
 * If enabled, the X-axis will not gather or report data (or use power).
 * @return Current X-axis standby enabled status
 * @see ITG3200SetStandbyXEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_STBY_XG_BIT
 */
boolean ITG3200GetStandbyXEnabled(void)
{
    return I2CDeviceReadBit(ITG3200_RA_PWR_MGM,
                            ITG3200_PWR_STBY_XG_BIT);
}

/**
 * Set X-axis standby enabled status.
 * @param enabled X-axis standby enabled status
 * @see ITG3200GetStandbyXEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_STBY_XG_BIT
 */
void ITG3200SetStandbyXEnabled(boolean enabled)
{
    I2CDeviceWriteBit(ITG3200_RA_PWR_MGM,
                      ITG3200_PWR_STBY_XG_BIT,
                      enabled);
}

/**
 * Get Y-axis standby enabled status.
 * If enabled, the Y-axis will not gather or report data (or use power).
 * @return Current Y-axis standby enabled status
 * @see ITG3200SetStandbyYEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_STBY_YG_BIT
 */
boolean ITG3200GetStandbyYEnabled(void)
{
    return I2CDeviceReadBit(ITG3200_RA_PWR_MGM,
                            ITG3200_PWR_STBY_YG_BIT);
}

/**
 * Set Y-axis standby enabled status.
 * @param enabled Y-axis standby enabled status
 * @see ITG3200GetStandbyYEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_STBY_YG_BIT
 */
void ITG3200SetStandbyYEnabled(boolean enabled)
{
    I2CDeviceWriteBit(ITG3200_RA_PWR_MGM,
                      ITG3200_PWR_STBY_YG_BIT,
                      enabled);
}

/**
 * Get Z-axis standby enabled status.
 * If enabled, the Z-axis will not gather or report data (or use power).
 * @return Current Z-axis standby enabled status
 * @see ITG3200SetStandbyZEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_STBY_ZG_BIT
 */
boolean ITG3200GetStandbyZEnabled(void)
{
    return I2CDeviceReadBit(ITG3200_RA_PWR_MGM,
                            ITG3200_PWR_STBY_ZG_BIT);
}

/**
 * Set Z-axis standby enabled status.
 * @param enabled Z-axis standby enabled status
 * @see ITG3200GetStandbyZEnabled()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_STBY_ZG_BIT
 */
void ITG3200SetStandbyZEnabled(boolean enabled)
{
    I2CDeviceWriteBit(ITG3200_RA_PWR_MGM,
                      ITG3200_PWR_STBY_ZG_BIT,
                      enabled);
}

/**
 * Get clock source setting.
 * @return Current clock source setting
 * @see ITG3200SetClockSource()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_CLK_SEL_BIT
 * @see ITG3200_PWR_CLK_SEL_LENGTH
 */
unsigned char ITG3200GetClockSource(void)
{
    return I2CDeviceReadBits(ITG3200_RA_PWR_MGM,
                             ITG3200_PWR_CLK_SEL_BIT,
                             ITG3200_PWR_CLK_SEL_LENGTH);
}

/** Set clock source setting.
 * On power up, the ITG-3200 defaults to the internal oscillator. It is highly
 * recommended that the device is configured to use one of the gyros (or an
 * external clock) as the clock reference, due to the improved stability.
 *
 * The CLK_SEL setting determines the device clock source as follows:
 *
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Reserved
 *
 * @param source New clock source setting
 * @see ITG3200GetClockSource()
 * @see ITG3200_RA_PWR_MGM
 * @see ITG3200_PWR_CLK_SEL_BIT
 * @see ITG3200_PWR_CLK_SEL_LENGTH
 */
void ITG3200SetClockSource(unsigned char source)
{
    I2CDeviceWriteBits(ITG3200_RA_PWR_MGM,
                       ITG3200_PWR_CLK_SEL_BIT,
                       ITG3200_PWR_CLK_SEL_LENGTH,
                       source);
}

/**
 * @}
 */
