/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 23/02/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Libreria IMU MPU6050 para SAMV71
*===========================================================================*/

/*==================[Definiciones]================================*/
  #ifndef _MPU6050_H
  #define _MPU6050_H

/*=====================[ Inclusiones ]============================*/
  #include <zephyr/kernel.h>
  #include <zephyr/device.h>
  #include <zephyr/devicetree.h>
  #include <zephyr/drivers/sensor.h>
  #include <stdio.h>

/*=================[Prototipos de funciones]======================*/
uint8_t MPU6050_init(void);
uint8_t MPU6050_getdata(float *temp, float *accell, float *gyroo);

/*=====================[Variables]================================*/
 
/*=====================[Definiciones]================================*/

 #endif