/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 30/056/2022 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: MPLABX - Harmony
 *
 * Descripcion: Libreria CANopen utilizando preiferico MCAN con libreria mcan_fd_interrupt.c
*===========================================================================*/

/*==================[Definiciones]================================*/
  #ifndef _CANopen_H
  #define _CANopen_H

/*=====================[ Inclusiones ]============================*/
  #include <stddef.h>                     //Define NULL
  #include <stdbool.h>                    //Define true
  #include <stdlib.h>                     //Define EXIT_FAILURE
  #include "definitions.h"                //Prototipos de funciones SYS
  #include "../lib/mcan_fd_interrupt/mcan_fd_interrupt.h"
  #include "Dictionary.h"
  
/*=================[Prototipos de funciones]======================*/
 uint8_t CANopen_init(void);
 void CANopen_STOP(void);


/*=====================[Variables]================================*/
  uint8_t CANopen_nodeid = 1;

/*=====================[Definiciones]================================*/
  #define CAN_send_timeout     10                        //Tiempo de espera en ms para verificar el envio correcto de mensaje por can
  
  #define CANopen_SDO_mode_client 0
  #define CANopen_SDO_mode_server 1
  #define CANopen_SDO_timeout     10                        //Tiempo de espera en ms para recibir respuesta de servidor
  #define CANopen_SDO_command_writing_request_4byte     0x23 //(rx) Solicitud de escritura de 4 bytes
  #define CANopen_SDO_command_writing_request_2byte     0x2B //(rx) Solicitud de escritura de 2 bytes
  #define CANopen_SDO_command_writing_request_1byte     0x2F //(rx) Solicitud de escritura de 1 byte
  #define CANopen_SDO_command_write_Confirmation        0x60 //(tx) Confirmacion de escritura a cliente (maestro) desde servidor (esclavo)
  #define CANopen_SDO_command_read_request              0x40 //(rx) Solicitud de lectura (upload) - solicitud de parametro a servidor (esclavo)
  #define CANopen_SDO_command_response_parameter_4byte  0x43 //(tx) Respuesta envio de parametro a cliente (4 bytes)
  #define CANopen_SDO_command_response_parameter_2byte  0x4B //(tx) Respuesta envio de parametro a cliente (2 bytes)
  #define CANopen_SDO_command_response_parameter_1byte  0x4F //(tx) Respuesta envio de parametro a cliente (1 byte)
  #define CANopen_SDO_command_transfer_aborted          0x80 //(tx) Transferencia de datos abortada, envio mensaje de error a cliente (No sopoerado en esta libreria)

  #endif

