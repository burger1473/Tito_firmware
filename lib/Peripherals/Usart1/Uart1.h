/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 13/02/2023
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Tarea encargada de recibir los datos por uart y procesarlos
 *===========================================================================*/

/*=======================[Definiciones]================================*/
#ifndef Uart1_H_
#define Uart1_H_

/*=====================[ Inclusiones ]============================*/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

/*=====================[Variables]================================*/

/*=================[Prototipos de funciones]======================*/
void Uart1_Config (void);
void Uart1_print (const char * mensaje);
void Uart1_println (const char * mensaje);
uint8_t Uart1_Recibir (char *dato);


#endif /* _TAREA_UART_H */