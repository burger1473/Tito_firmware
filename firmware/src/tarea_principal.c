/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 16/05/2022 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: MPLABX - Harmony
 *
 * Descripcion: Obtiene la letra recibida por uart y envia una trama por can
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
#include "tarea_principal.h"
#include "definitions.h"
#include <string.h>
#include <stdio.h>
#include "../lib/Usart1_FreeRTOS/Uart1_FreeRTOS.h"

/*=====================[Variables]================================*/
  TAREA_PRINCIPAL_DATA tarea_principalData;       //Estructura que contiene la informacion de la tarea como por ejemplo, el estado de esta
  
  TaskHandle_t xTAREA_Can1;                       //Puntero hacia la tarea can1
  TaskHandle_t xTAREA_Can2;                       //Puntero hacia la tarea can2
  

/*===================[Prototipos de funciones]=========================*/
  void TAREA_Can1(void *pvParameters );
  void TAREA_Can2(void *pvParameters );
  
/*=====================[Implementaciones]==============================*/

  
/*========================================================================
  Funcion: TAREA_PRINCIPAL_Initialize
  Descripcion: Tarea para iniciar la maquina de estado de la tarea y para crear semaforo para proteger el recurso compartido de uart
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/  
void TAREA_PRINCIPAL_Initialize ( void )
{
    tarea_principalData.state = TAREA_PRINCIPAL_STATE_INIT; //Se inicia la maquina de estado mediante su estructura. Se establece en 1
    Uart1_FreeRTOS_Config();
    //uint8_t resultado=CANopen_init();
    uint8_t resultado=1;    
    if (resultado == 0){ Uart1_println("CANopen was initialized and is in pre-operational mode"); }
    if (resultado == 1){ Uart1_println("No se pudo crear el bloqueo mutex"); }
   // if (resultado == 2){ Uart1_println("Error al mandar mensaje Boot_Up");  CANopen_STOP(); }
}

/*========================================================================
  Funcion: TAREA_PRINCIPAL_Tasks
  Descripcion: Tarea para interpretar dato recibido por uart y enviar trama can
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void TAREA_PRINCIPAL_Tasks ( void )
{
    while (1)
    {
        
        char dato[1]= {0};
        Uart1_leer_x_bytes(1, dato);
        
        if (dato[0] != ' ')                                   //Si el byte es distinto del caracter ' '
        {   
            //Uart1_print(dato);

            if (dato[0] == '1')                               //Si el dato recibido es el caracter 1
            {
                xTaskCreate((TaskFunction_t) TAREA_Can1, "TAREA_Can1", 512, NULL, 4, &xTAREA_Can1); //Creo tarea para envio trama 1 por can
            }

            if (dato[0] == '2')                               //Si el dato recibido es el caracter 2
            {
                xTaskCreate((TaskFunction_t) TAREA_Can2, "TAREA_Can2", 512, NULL, 4, &xTAREA_Can2); //Creo tarea para envio trama 2 por can
            }

        }else{
            Uart1_print("\r\nEsperando tecla...");
        }

        vTaskDelay(1500 / portTICK_PERIOD_MS );               //Deje que la tarea quede inactiva por un tiempo determinado dejando que se produzca el cambio de contexto a otra tarea.
    }
}


/*========================================================================
  Funcion: TAREA_Can1
  Descripcion: Envio la trama 1 por canbus
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void TAREA_Can1(void *pvParameters ){
  
  Uart1_print("\r\nFin tarea can 1");
  if(xTAREA_Can1 != NULL){vTaskDelete(xTAREA_Can1); xTAREA_Can1=NULL;} //Elimino esta tarea
}

/*========================================================================
  Funcion: TAREA_Can2
  Descripcion: Envio la trama 2 por canbus
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void TAREA_Can2(void *pvParameters ){
  
  Uart1_print("\r\nFin tarea can 2");
  if(xTAREA_Can2 != NULL){vTaskDelete(xTAREA_Can2); xTAREA_Can2=NULL;} //Elimino esta tarea
}

