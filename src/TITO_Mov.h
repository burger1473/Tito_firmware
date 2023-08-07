/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 22/04/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripción: Realiza los movimientos correspondientes segun el estado de las maquinas de estado
 *===========================================================================*/

/*==================[Definiciones]================================*/
#ifndef TITO_Mov_H_
#define TITO_Mov_H_

/*=====================[ Inclusiones ]============================*/
#include <zephyr/kernel.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/*=====================[Variables]================================*/
typedef enum TITO_Mov_Mode {
  OFF,
  MANUAL,
  AUTO
} TITO_Mov_Mode_t;

typedef enum TITO_Mov_ProfileMode {
  NOTHING,
  HOME,
  SET_HEIGHT,
  FORWARD,
  TURN
} TITO_Mov_ProfileMode_t;

/*=================[Prototipos de funciones]======================*/
void TITO_Mov_Tasks(void *, void *, void *);
uint8_t TITO_Mov_setMode(TITO_Mov_Mode_t mode);
uint8_t TITO_Mov_setProfileMode(TITO_Mov_ProfileMode_t profileMode);
uint8_t TITO_Mov_setTargetVelocity(float targetVelocity);

#endif  // TITO_Mov_H_
