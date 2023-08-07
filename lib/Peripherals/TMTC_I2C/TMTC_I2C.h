/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 23/02/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Libreria I2C para telemetria y telecomando para SAMV71
*===========================================================================*/

/*==================[Definiciones]================================*/
  #ifndef _TMTC_I2C_H
  #define _TMTC_I2C_H

/*=====================[ Inclusiones ]============================*/
  #include <zephyr/kernel.h>
  #include <zephyr/device.h>
  #include <zephyr/devicetree.h>
  #include <zephyr/drivers/i2c.h>
  #include <stdio.h>
  #include <string.h>

/*=================[Prototipos de funciones]======================*/
uint8_t TMTC_I2C_init(void);
uint8_t TMTC_I2C_sendMsg(const uint8_t *tx_buf, uint8_t n_bytes, uint16_t direccion, uint16_t port);
uint8_t TMTC_I2C_readMsg(uint8_t *rx_buf, size_t bufferSize, size_t *readBytes, uint16_t direccion);
uint8_t TMTC_I2C_writeandreadMMsg(uint8_t *tx_buf, uint8_t *rx_buf, uint8_t n_bytes, uint16_t direccion);

/*=====================[Variables]================================*/
 
/*=====================[Definiciones]================================*/

 #endif