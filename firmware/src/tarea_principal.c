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
#include "../lib/mcan_fd_interrupt/mcan_fd_interrupt.h"

/*=====================[Variables]================================*/
  TAREA_PRINCIPAL_DATA tarea_principalData;       //Estructura que contiene la informacion de la tarea como por ejemplo, el estado de esta
  
  TaskHandle_t xTAREA_Can1;                       //Puntero hacia la tarea can1
  TaskHandle_t xTAREA_Can2;                       //Puntero hacia la tarea can2
  
  
  //uint8_t Can1MessageRAM[MCAN1_MESSAGE_RAM_CONFIG_SIZE] __attribute__((aligned (32)))__attribute__((space(data), section (".ram_nocache")));
  uint8_t Mcan1MessageRAM[MCAN1_MESSAGE_RAM_CONFIG_SIZE] __attribute__((aligned (32)))__attribute__((space(data), section (".ram_nocache")));
  SemaphoreHandle_t canMutexLock;                 //Mutex de semaforo utilizado para proteger el recurso compartido de CAN con otras tareas
  
  
  

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
    
    canMutexLock = xSemaphoreCreateMutex();                //Creo semaforo para proteger el recurso compartido de CAN con otras tareas
    if(canMutexLock == NULL)                               //Si no se creo el semaforo
    {
        /* No habia suficiente almacenamiento dinamico de FreeRTOS disponible para que el semaforo se creara correctamente. */
        USART1_Write((uint8_t*)"No se pudo crear el bloqueo mutex2\r\n", strlen("No se pudo crear el bloqueo mutex2\r\n"));  //Escribo por uart
    }
}

/*========================================================================
  Funcion: TAREA_PRINCIPAL_Tasks
  Descripcion: Tarea para interpretar dato recibido por uart y enviar trama can
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void TAREA_PRINCIPAL_Tasks ( void )
{
    mcan_fd_interrupt_config(Mcan1MessageRAM);               //Configuro memoria ram de mensaje can
    
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
  
  xSemaphoreTake(canMutexLock, portMAX_DELAY);                         //Tomo semaforo para proteger el bus can ya que es un recurso compartico con otras tareas
  static uint8_t message[4] = {0}; message[0]='T'; message[1]='I'; message[2]='T'; message[3]='O';
  Enable_testmode(0);
  bool retorno = mcan_fd_interrupt_enviar((uint32_t) 0x45A, message, 4, MCAN_MODE_NORMAL); //Envio trama por can bus
  if ( retorno == false)
  {
    Uart1_print("\r\nError");
  }             
  //mcan_fd_interrupt_habilitar();                                     //Libero la maquina de estado del mcan para que otra tarea o funcion pueda enviar o recibir por can
  xSemaphoreGive(canMutexLock);                                        //Libero semaforo
  
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

