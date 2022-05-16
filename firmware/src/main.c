/*=============================================================================
 * Author: Spacebeetech - Navegación
 * Date: 16/05/2021 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: MPLABX - Harmony
 *
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
  #include <stddef.h>                     // Defines NULL
  #include <stdbool.h>                    // Defines true
  #include <stdlib.h>                     // Defines EXIT_FAILURE
  #include "definitions.h"                // SYS function prototypes


/*=====================[Implementaciones]==============================*/

/*========================================================================
  Funcion: uartReadEventHandler
  Descripcion: Callback que se llama al ocurrir una recepcio de datos por uart, esta funcion libera el semaforo que desbloquea la tarea uart
              para poder obtener el dato que se encuentra en el buffer del driver uart.
  Parametro de entrada: 
                          USART_EVENT event: tipo de evento del callback
                          uintptr_t context.
  No retorna nada
  ========================================================================*/
int main ( void )
{
    SYS_Initialize ( NULL );    //Inicializa todos los modulos generado en el Harmony

    while ( true )
    {
        SYS_Tasks ( );        //Mantenga las máquinas de estado de todos los módulos MPLAB Harmony sondeados.
                              //Si se utiliza FreeRTOS, en esta funcion se crean las tareas y se llama al planificador para que este realize los cambio de contextos.
                              //Si se utiliza FreeRTOS, esta funcion nunca retorna ya que esta misma llama al planificador y este ultimo intercambia el contexto de ejecucion entre las tareas creadas previamente y las nuevas tareas que se creen durante su ejecucion.
                              // Cuando se inicia el planificador de FreeRTOS no se retorna nunca al menos que el micro se quede sin ram. Por lo tanto lo que sigue de aca en adelante no se ejecutara nunca.
        /* La ejecución no debería venir aquí durante el funcionamiento normal si no se usa FreeRtos*/
    }
    return ( EXIT_FAILURE );
}

