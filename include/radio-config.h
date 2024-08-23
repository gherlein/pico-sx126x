/*!
 * \file      board-config.h
 *
 * \brief     Board configuration
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 * Derived from code in this zip file:  https://files.waveshare.com/wiki/RP2040-LoRa/Rp2040-lora-code.zip
 * Found the code in this wiki: https://www.waveshare.com/wiki/RP2040-LoRa
 * \endcode
 *
 * \author    Greg Herlein
 *
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#define BOARD_TCXO_WAKEUP_TIME 5

/*!
 * Board MCU pins definitions
 */
#define RADIO_RESET 27

#define RADIO_MOSI 3
#define RADIO_MISO 4
#define RADIO_SCLK 2

#define RADIO_NSS 13
#define RADIO_BUSY 18
#define RADIO_DIO_1 16

#define RADIO_ANT_SWITCH_POWER 17

#ifdef __cplusplus
}
#endif

#endif // __BOARD_CONFIG_H__
