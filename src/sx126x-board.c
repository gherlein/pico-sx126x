/*!
 * \file      sx1262mbxdas-board.c
 *
 * \brief     Target board SX1262MBXDAS shield driver implementation
 *
 * \remark    This target board is only available with the SX126xDVK1xAS
 *            development kit.
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <stdlib.h>
#include "radio-config.h"
#include "radio.h"
#include "sx126x-board.h"

#if defined(USE_RADIO_DEBUG)
/*!
 * \brief Writes new Tx debug pin state
 *
 * \param [IN] state Debug pin state
 */
static void SX126xDbgPinTxWrite(uint8_t state);

/*!
 * \brief Writes new Rx debug pin state
 *
 * \param [IN] state Debug pin state
 */
static void SX126xDbgPinRxWrite(uint8_t state);
#endif

/*!
 * \brief Holds the internal operating mode of the radio
 */
static RadioOperatingModes_t OperatingMode;

/*!
 * Antenna switch GPIO pins objects
 */
// Gpio_t AntPow;
//  Gpio_t DeviceSel;

// this should be in the radio_config.h file ONLY
// #define RADIO_ANT_SWITCH_POWER 22
/*!
 * Debug GPIO pins objects
 */
#if defined(USE_RADIO_DEBUG)
Gpio_t DbgPinTx;
Gpio_t DbgPinRx;
#endif

static inline void cs_select(void)
{
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0); // Active low
    asm volatile("nop \n nop \n nop");
    // sleep_ms(1);
}

static inline void cs_deselect(void)
{
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
    // sleep_ms(1);
}

void SX126xIoInit(spi_inst_t *spi)
{
    SX126x.spi = spi;

    // RADIO_BUSY is an input
    gpio_init(RADIO_BUSY);
    gpio_set_dir(RADIO_NSS, GPIO_IN);

    // Reset is active-low
    gpio_init(RADIO_RESET);
    gpio_set_dir(RADIO_RESET, GPIO_OUT);
    gpio_put(RADIO_RESET, 1);

    // Chip select NSS is active-low, so we'll initialise it to a driven-high state
    gpio_init(RADIO_NSS);
    gpio_set_dir(RADIO_NSS, GPIO_OUT);
    gpio_put(RADIO_NSS, 1);
#ifdef ORIG
    GpioInit(&SX126x.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    GpioInit(&SX126x.BUSY, RADIO_BUSY, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX126x.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
// GpioInit( &DeviceSel, RADIO_DEVICE_SEL, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
#endif
}

void SX126xIoIrqInit(DioIrqHandler dioIrq)
{

#ifdef ORIG
    GpioSetInterrupt(&SX126x.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, dioIrq);
#endif
}

void SX126xIoDeInit(void)
{
#ifdef ORIG
    GpioInit(&SX126x.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    GpioInit(&SX126x.BUSY, RADIO_BUSY, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&SX126x.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
#endif
}

void SX126xIoDbgInit(void)
{
#ifdef ORIG
#if defined(USE_RADIO_DEBUG)
    GpioInit(&DbgPinTx, RADIO_DBG_PIN_TX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&DbgPinRx, RADIO_DBG_PIN_RX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
#endif
#endif
}

void SX126xIoTcxoInit(void)
{

    CalibrationParams_t calibParam;

    SX126xSetDio3AsTcxoCtrl(TCXO_CTRL_1_7V, SX126xGetBoardTcxoWakeupTime() << 6); // convert from ms to SX126x time base
    calibParam.Value = 0x7F;
    SX126xCalibrate(calibParam);
}

uint32_t SX126xGetBoardTcxoWakeupTime(void)
{
    return BOARD_TCXO_WAKEUP_TIME;
}

void SX126xIoRfSwitchInit(void)
{
    SX126xSetDio2AsRfSwitchCtrl(true);
}

RadioOperatingModes_t SX126xGetOperatingMode(void)
{
    return OperatingMode;
}

void SX126xSetOperatingMode(RadioOperatingModes_t mode)
{
    OperatingMode = mode;
#if defined(USE_RADIO_DEBUG)
    switch (mode)
    {
    case MODE_TX:
        SX126xDbgPinTxWrite(1);
        SX126xDbgPinRxWrite(0);
        break;
    case MODE_RX:
    case MODE_RX_DC:
        SX126xDbgPinTxWrite(0);
        SX126xDbgPinRxWrite(1);
        break;
    default:
        SX126xDbgPinTxWrite(0);
        SX126xDbgPinRxWrite(0);
        break;
    }
#endif
}

void SX126xReset(void)
{
    gpio_put(RADIO_RESET, 0);
    sleep_ms(2);
    gpio_put(RADIO_RESET, 1);
#ifdef ORIG
    DelayMs(10);
    GpioInit(&SX126x.Reset, RADIO_RESET, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    DelayMs(20);
    GpioInit(&SX126x.Reset, RADIO_RESET, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0); // internal pull-up
    DelayMs(10);
#endif
}

void SX126xWaitOnBusy(void)
{
    while (gpio_get(RADIO_BUSY))
    {
        sleep_us(10);
    }

#ifdef ORIG

    while (GpioRead(&SX126x.BUSY) == 1)
        ;
#endif
}

void SX126xWakeup(void)
{
#ifdef ORIG

    CRITICAL_SECTION_BEGIN();

    GpioWrite(&SX126x.Spi.Nss, 0);

    SpiInOut(&SX126x.Spi, RADIO_GET_STATUS);
    SpiInOut(&SX126x.Spi, 0x00);

    GpioWrite(&SX126x.Spi.Nss, 1);

    // Wait for chip to be ready.
    SX126xWaitOnBusy();

    // Update operating mode context variable
    SX126xSetOperatingMode(MODE_STDBY_RC);

    CRITICAL_SECTION_END();
#endif
}

void SX126xWriteCommand(RadioCommands_t command, uint8_t *buffer, uint16_t size)
{
    SX126xCheckDeviceReady();
    cs_select();
    while (!spi_is_writable(spi_default))
    {
        sleep_us(100);
    }
    spi_write_blocking(SX126x.spi, (uint8_t *)&command, 1);
    for (uint16_t i = 0; i < size; i++)
    {
        spi_write_blocking(SX126x.spi, buffer, size);
    }
    cs_deselect();
    SX126xWaitOnBusy();
}

uint8_t SX126xReadCommand(RadioCommands_t command, uint8_t *buffer, uint16_t size)
{
    uint8_t status = 0;
    SX126xCheckDeviceReady();
    cs_select();
    while (!spi_is_readable(spi_default))
    {
        sleep_us(100);
    }
    spi_write_blocking(SX126x.spi, (uint8_t *)&command, 1);
    for (uint16_t i = 0; i < size; i++)
    {
        status = spi_write_read_blocking(SX126x.spi, 0, buffer, size);
    }
    cs_deselect();
    SX126xWaitOnBusy();
    return status;
}

void SX126xWriteRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    SX126xCheckDeviceReady();

#ifdef ORIG

    GpioWrite(&SX126x.Spi.Nss, 0);

    SpiInOut(&SX126x.Spi, RADIO_WRITE_REGISTER);
    SpiInOut(&SX126x.Spi, (address & 0xFF00) >> 8);
    SpiInOut(&SX126x.Spi, address & 0x00FF);

    for (uint16_t i = 0; i < size; i++)
    {
        SpiInOut(&SX126x.Spi, buffer[i]);
    }

    GpioWrite(&SX126x.Spi.Nss, 1);

#endif
    SX126xWaitOnBusy();
}

void SX126xWriteRegister(uint16_t address, uint8_t value)
{
    SX126xWriteRegisters(address, &value, 1);
}

void SX126xReadRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    SX126xCheckDeviceReady();

#ifdef ORIG

    GpioWrite(&SX126x.Spi.Nss, 0);

    SpiInOut(&SX126x.Spi, RADIO_READ_REGISTER);
    SpiInOut(&SX126x.Spi, (address & 0xFF00) >> 8);
    SpiInOut(&SX126x.Spi, address & 0x00FF);
    SpiInOut(&SX126x.Spi, 0);
    for (uint16_t i = 0; i < size; i++)
    {
        buffer[i] = SpiInOut(&SX126x.Spi, 0);
    }
    GpioWrite(&SX126x.Spi.Nss, 1);

#endif
    SX126xWaitOnBusy();
}

uint8_t SX126xReadRegister(uint16_t address)
{
    uint8_t data;
    SX126xReadRegisters(address, &data, 1);
    return data;
}

void SX126xWriteBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    SX126xCheckDeviceReady();

#ifdef ORIG

    GpioWrite(&SX126x.Spi.Nss, 0);

    SpiInOut(&SX126x.Spi, RADIO_WRITE_BUFFER);
    SpiInOut(&SX126x.Spi, offset);
    for (uint16_t i = 0; i < size; i++)
    {
        SpiInOut(&SX126x.Spi, buffer[i]);
    }
    GpioWrite(&SX126x.Spi.Nss, 1);
#endif
    SX126xWaitOnBusy();
}

void SX126xReadBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    SX126xCheckDeviceReady();
#ifdef ORIG

    GpioWrite(&SX126x.Spi.Nss, 0);

    SpiInOut(&SX126x.Spi, RADIO_READ_BUFFER);
    SpiInOut(&SX126x.Spi, offset);
    SpiInOut(&SX126x.Spi, 0);
    for (uint16_t i = 0; i < size; i++)
    {
        buffer[i] = SpiInOut(&SX126x.Spi, 0);
    }
    GpioWrite(&SX126x.Spi.Nss, 1);
#endif
    SX126xWaitOnBusy();
}

void SX126xSetRfTxPower(int8_t power)
{
    SX126xSetTxParams(power, RADIO_RAMP_40_US);
}

uint8_t SX126xGetDeviceId(void)
{
    // if( GpioRead( &DeviceSel ) == 1 )
    // {
    //     return SX1261;
    // }
    // else
    // {
    //     return SX1262;
    // }

    return SX1262;
}

void SX126xAntSwOn(void)
{
    // GpioInit(&AntPow, RADIO_ANT_SWITCH_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
}

void SX126xAntSwOff(void)
{
    // GpioInit(&AntPow, RADIO_ANT_SWITCH_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
}

bool SX126xCheckRfFrequency(uint32_t frequency)
{
    // Implement check. Currently all frequencies are supported
    return true;
}

#if defined(USE_RADIO_DEBUG)
static void SX126xDbgPinTxWrite(uint8_t state)
{
    // GpioWrite(&DbgPinTx, state);
}

static void SX126xDbgPinRxWrite(uint8_t state)
{
    // GpioWrite(&DbgPinRx, state);
}
#endif
