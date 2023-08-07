/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 17/02/2023
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Libreria driver EPOS4
 *===========================================================================*/

/*==================[Definiciones]================================*/
  #ifndef _epos4_H
  #define _epos4_H

/*=====================[ Inclusiones ]============================*/
  #include <zephyr/kernel.h>
  #include <zephyr/device.h>
  #include <zephyr/devicetree.h>
  #include <zephyr/drivers/can.h>
  #include <zephyr/sys/byteorder.h>
  #include <zephyr/drivers/uart.h>
  #include <stddef.h>                     //Define NULL
  #include <stdbool.h>                    //Define true
  #include <stdlib.h>                     //Define EXIT_FAILURE
  #include "../CANopen/CANopen.h"
  
/*=================[Prototipos de funciones]======================*/
    bool Epos4_Power_Disable(uint8_t EPOS4_id);
    bool Epos4_config_motor_parameters (uint8_t EPOS4_id, uint8_t *max_vel, uint8_t *nom_curr, uint8_t *max_curr);
    bool Epos4_config_parameters (uint8_t EPOS4_id, uint8_t *max_vel, uint8_t *des_ramp, uint8_t *des_ramp_fast, uint8_t *max_acel);
    bool Epos4_PROFILE_POSITION_MODE (uint8_t EPOS4_id, uint8_t *position, uint8_t *max_vel, uint8_t *ramp_acel, uint8_t *ramp_desacel);
    bool Epos4_read_actual_position(uint8_t EPOS4_id, uint8_t *resultado, uint8_t *data);
    bool Epos4_read_actual_Torque(uint8_t EPOS4_id, uint8_t *resultado, uint8_t *data);
    bool Epos4_read_actual_Velocity(uint8_t EPOS4_id, uint8_t *resultado, uint8_t *data);
    bool Epos4_config_PID (uint8_t EPOS4_id, uint8_t controlador, uint8_t *control_P, uint8_t *control_I, uint8_t *control_D);
    bool Epos4_Cyclic_Synchronous_Velocity_MODE (uint8_t EPOS4_id, uint8_t *velocity);
    
/*=====================[Variables]================================*/
  

  #endif

