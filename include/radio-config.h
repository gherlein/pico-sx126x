#ifndef __RADIO_CONFIG_H__
#define __RADIO_CONFIG_H__

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
#define RADIO_RESET 20

#define RADIO_MOSI 19
#define RADIO_MISO 16
#define RADIO_SCLK 18

#define RADIO_NSS 17
#define RADIO_BUSY 26
#define RADIO_DIO_1 28

#ifdef __cplusplus
}
#endif

#endif // __RADIO_CONFIG_H__
