/*=============================================================================
 * Author: Spacebeetech - Navegación
 * Date: 16/05/2022 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: MPLABX - Harmony
 *
 * Descripción: Tarea encargada de recibir los datos por uart y procesarlos
 * Funcionamiento: La funcion Uart1_FreeRTOS_Config crea la TAREA_UART_Tasks y un semaforo para proteger el intercambio de datos con el periferico ya que es un recurso compartido entre diferentes tareas. 
 *                 TAREA_UART_Tasks al iniciarse configura el periferico uart como interrupcion cuando el buffer uart llega a x bytes previamente establecidos.
 *                 Ademas el iniciarse esta tarea tambien crea un semaforo binario, este semaforo sirve para bloquear o habilitar la tarea dependiendo si hay datos en el buffer para leer o no.
 *                 Luego de iniciar el semaforo y configurar la int de uart, se entra un bucle infinito. Dentro de este bucle se intenta tomar el semaforo, si el semaforo habilita, se leen los datos del buffer uart.
 *                 en caso de que no haya dato en el buffer, el semaforo no estara habilitado por la interrupcion y por ende bloqueara la tarea TAREA_UART_Tasks hasta que llegue el dato uart permitiendo que otras tareas entren en contexto y ejecución.
 *
 *                 Para recibir un dato, se debe llemar a la funcion Uart1_Recibir(). Esta funcion retorna el mensaje obtenido por la tarea explicada anteriormente.
 *                 Para enviar mensaje se puede usar Uart1_print() o Uart1_println()
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
#include "Uart1_FreeRTOS.h"
#include "definitions.h"
#include "portmacro.h"
#include <string.h>
#include <stdbool.h>                    // Defines true
#include <stdio.h>

#define RX_BUFFER_SIZE                  10

/*=================[Prototipos de funciones]======================*/
void TAREA_UART_Initialize ( void );
void TAREA_UART_Tasks ( void *pvParameters );

/*=====================[Variables]================================*/
  TaskHandle_t xTAREA_UART_Tasks;                    //Puntero hacia la TAREA_UART_Tasks
  TAREA_UART_DATA tarea_uartData;                    //Estructura que contiene la informacion de la tarea como por ejemplo, el estado de esta
  static SemaphoreHandle_t dataRxSemaphore;          //Variable Mutex para semaforo binario para bloquer tarea uart hasta que se llame a la funcion de interupcion de uart. (uartReadEventHandler)
  static SemaphoreHandle_t Semaforo_uart;            //Mutex de semaforo utilizado para proteger el recurso compartido de UART con otras tareas
  char receiveBuffer[RX_BUFFER_SIZE]={0};            //Variable donde se almacena el dato recibido

  bool errorStatus = false;                          //Indica el estado de error
  bool writeStatus = false;                          //Indica el estado de escritura (si se pudo escribir bien por uart)
  bool readStatus = false;                           //Indica si hay dato para leer
    
/*=====================[Implementaciones]==============================*/


/*========================================================================
Funcion: Uart1_config_recepcion
    Descripcion: Configura el periferico para saltar al callback luego de recibur x bytes
Parametro de entrada: uint8_t num_bytes: cantidad de bytes que se desean recibir
Retorna:     2= desborde timeout
             1= dato recibido correctamente
             0= si no se pudo configurar
========================================================================*/
uint8_t Uart1_leer_x_bytes(uint8_t num_bytes, char *dato){
  uint8_t retorno=0;
  uint8_t contador=0;
  xSemaphoreTake(Semaforo_uart, portMAX_DELAY);                         //Tomo semaforo para proteger la recepcion por uart ya que es un recurso compartido con otras tareas
  if(writeStatus == true){                                              //Si esta disponible el periferico
      writeStatus = false;                                              //Indico que deja de estar dispopnible
      USART1_Read(&receiveBuffer, num_bytes);                           //Configuro callback
      while(true){                                                      //Espero x segundos para recibir datos, en caso que no se reciba, se retorna 2
        if(readStatus == true){                                         //Si hay dato disponible para leer
            portENTER_CRITICAL();                                       //Seccion critica para evitar que se ejecute cambio de contexto alterando el proceso de guardado de la variable
            strcpy(dato,receiveBuffer);                                 //paso dato
            portEXIT_CRITICAL();                                        //Salgo de seccion critica
            retorno=1;
            //USART1_Write(&receiveBuffer, sizeof(receiveBuffer));
            for(uint8_t i=0; i<RX_BUFFER_SIZE; i++){                         //Borro datos de variable
                receiveBuffer[i]=' ';                               
            }      
            break;                                                      //Salgo del while
        }else{                                                          //Si no hay dato para leer, incrmento contador de timeout
            contador++;
            if(contador>=20){                                           //Si supero el timeout
                retorno=2;   
                strcpy(dato,receiveBuffer);
                break;                                                  //Salgo del while
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS );               //Deje que la tarea quede inactiva por un tiempo determinado dejando que se produzca el cambio de contexto a otra tarea.
      }
  }
  xSemaphoreGive(Semaforo_uart);                                     //Libero semaforo
  return retorno;
}

/*========================================================================
  Funcion: APP_ReadCallback(uintptr_t context)
  Descripcion: Callback que se llama al ocurrir una transmicion correcta por uart
  Parametro de entrada: 
                          uintptr_t context.
  No retorna nada
  ========================================================================*/
void APP_WriteCallback(uintptr_t context)
{
    writeStatus = true;
}

/*========================================================================
  Funcion: APP_ReadCallback(uintptr_t context)
  Descripcion: Callback que se llama al ocurrir la recepcion por uart de la cantidad de bytes indicados previamente
  Parametro de entrada: 
                          uintptr_t context.
  No retorna nada
  ========================================================================*/
void APP_ReadCallback(uintptr_t context)
{
    if(USART1_ErrorGet() != USART_ERROR_NONE)
    {
        errorStatus = true;
        USART1_Write((uint8_t*)"Error al leer USART", sizeof("Error al leer USART"));  //Escribo por uart
    }
    else
    {
        readStatus = true;
    }
}

/*========================================================================
  Funcion: Uart1_FreeRTOS_Config
  Descripcion: Configura los callback y crea el semaforo para controlar el acceso al recurso
  Parametro de entrada:
                        uint8_t prioridad: Priodirdad de la tarea a crear
  No retorna nada
  ========================================================================*/
void Uart1_FreeRTOS_Config(void)
{
    Semaforo_uart = xSemaphoreCreateMutex();                //Creo semaforo para proteger el recurso compartido de UART con otras tareas
    if( Semaforo_uart == NULL)                              //Si no se creo el semaforo
    {
        /* No habi­a suficiente almacenamiento dinÃ¡mico de FreeRTOS disponible para que el semaforo se creara correctamente. */
        USART1_Write((uint8_t*)"No se pudo crear el bloqueo mutex", strlen("No se pudo crear el bloqueo mutex"));
    }
    /* Register callback functions and send start message */
    USART1_WriteCallbackRegister(APP_WriteCallback, 0);
    USART1_ReadCallbackRegister(APP_ReadCallback, 0);
    for(uint8_t i=0; i<RX_BUFFER_SIZE; i++){                         //Borro datos de variable
         receiveBuffer[i]=' ';                               
    }
}

/*========================================================================
  Funcion: Uart1_print
  Descripcion: Envia una cadena de texto por el uart1
  Parametro de entrada:
                        uint8_t mensaje: Texto a enviar
  No retorna nada
  ========================================================================*/
void Uart1_print (const char * mensaje)
{
  xSemaphoreTake(Semaforo_uart, portMAX_DELAY);                      //Tomo semaforo para proteger el envio por uart ya que es un recurso compartico con otras tareas
  USART1_Write((uint8_t*)mensaje, strlen(mensaje));                  //Envio bytes por uart
  xSemaphoreGive(Semaforo_uart);                                     //Libero semaforo
}

/*========================================================================
  Funcion: Uart1_println
  Descripcion: Envia una cadena de texto por el uart1 con salto de linea y retorno de carro
  Parametro de entrada:
                        uint8_t mensaje: Texto a enviar
  No retorna nada
  ========================================================================*/
void Uart1_println (const char * mensaje)
{
  xSemaphoreTake(Semaforo_uart, portMAX_DELAY);                      //Tomo semaforo para proteger el envio por uart ya que es un recurso compartico con otras tareas
  USART1_Write((uint8_t*)mensaje, strlen(mensaje));                  //Envio bytes por uart
  USART1_Write((uint8_t*)"\r\n", strlen("\r\n"));                    //Envio salto de linea y retorno de carro
  xSemaphoreGive(Semaforo_uart);                                     //Libero semaforo
}
