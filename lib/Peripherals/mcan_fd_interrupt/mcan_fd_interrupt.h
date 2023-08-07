/*=============================================================================
 * Author: Spacebeetech - Navegación
 * Date: 13/02/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripción: Libreria CAN bus en modo fd por interrupcion
 *===========================================================================*/

/*==================[Definiciones]================================*/
  #ifndef MCAN_FD_INTERRUPT_H_
  #define MCAN_FD_INTERRUPT_H_
  
  #define debug 0
  

/*=====================[ Inclusiones ]============================*/
  #include <zephyr/kernel.h>
  #include <zephyr/device.h>
  #include <zephyr/devicetree.h>
  #include <zephyr/drivers/can.h>
  #include <zephyr/sys/byteorder.h>
  #include <zephyr/drivers/uart.h>
  

/*=====================[Variables]================================*/
typedef enum
    {
        CAN_LIBRE,               //No se esta esperando una transmision o recepcion
        CAN_RECEPCION_OK,        //Se recibio correctamente un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_recibir()
        CAN_TRANSMICION_OK,      //Se transmitio correctamente un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_enviar()
        CAN_RECEPCION_ERROR,     //Error al recibir un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_recibir()
        CAN_TRANSMICION_ERROR,   //Error al transmitir un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_enviar()
    } CAN_ESTADO;                //Enumaracion de los estados posibles

typedef enum
    {
        MCAN_MODE_NORMAL,              
        MCAN_MODE_FD_STANDARD,      
        MCAN_MODE_FD_EXTENDED,
        MCAN_MODE_EXTENDED    
    } MCAN_MODE;                //Enumaracion de los estados de transmicion de MCAN


/*=================[Prototipos de funciones]======================*/
 void mcan_fd_interrupt_config(void);
 bool mcan_fd_interrupt_enviar(uint32_t messageID , uint8_t *message, uint8_t messageLength, MCAN_MODE MCAN_MODE_L);
 bool mcan_fd_interrupt_recibir(uint32_t *rx_messageID2, uint8_t *rx_message2, uint8_t *rx_messageLength2);
 uint8_t mcan_resultado(void);
 void mcan_fd_interrupt_habilitar(void);


#endif /* _TAREA_PRINCIPAL_H */

