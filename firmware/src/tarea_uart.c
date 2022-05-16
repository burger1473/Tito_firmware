/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    tarea_uart.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "tarea_uart.h"
#include "definitions.h"
#include "portmacro.h"
#include <string.h>
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the TAREA_UART_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TAREA_UART_DATA tarea_uartData;
static SemaphoreHandle_t dataRxSemaphore;
extern SemaphoreHandle_t uartMutexLock;
uint8_t readByte_global=' ';
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void uartReadEventHandler(USART_EVENT event, uintptr_t context )
{
    if (event == USART_EVENT_READ_THRESHOLD_REACHED)
    {
        BaseType_t xHigherPriorityTaskWoken;

        /* Unblock the task by releasing the semaphore. */
        xSemaphoreGiveFromISR( dataRxSemaphore, &xHigherPriorityTaskWoken );

        /* If xHigherPriorityTaskWoken was set to true you
        we should yield */

        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}
/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void TAREA_UART_Initialize ( void )

  Remarks:
    See prototype in tarea_uart.h.
 */

void TAREA_UART_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    tarea_uartData.state = TAREA_UART_STATE_INIT;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void TAREA_UART_Tasks ( void )

  Remarks:
    See prototype in tarea_uart.h.
 */

void TAREA_UART_Tasks ( void )
{
    uint8_t readByte;
    bool status = false;
    //TickType_t timeNow;

    USART1_ReadCallbackRegister(uartReadEventHandler, 0);
    USART1_ReadThresholdSet(1);
    USART1_ReadNotificationEnable(true, false);

    dataRxSemaphore = xSemaphoreCreateBinary();

    if (dataRxSemaphore != NULL)
    {
        status = true;
    }

    while (status == true)
    {
        /* Block until a character is received on the terminal */
        if( xSemaphoreTake( dataRxSemaphore, portMAX_DELAY ) == pdTRUE )
        {
            /*
            xSemaphoreTake(uartMutexLock, portMAX_DELAY);
            USART1_Write((uint8_t*)"Inicio tarea uart\r\n", 19);
            xSemaphoreGive(uartMutexLock);
            */
            /* Toggle an LED if character received is 'L' or 'l' */
            while (USART1_Read(&readByte, 1) == true)
            {
                portENTER_CRITICAL(); //seccion critica para evitar que se ejecute cambio de contexto
                readByte_global=readByte;
                portEXIT_CRITICAL();
            }

            /* Work done by task3 for 50 ticks 
            timeNow = xTaskGetTickCount();
            while ((xTaskGetTickCount() - timeNow) < 50);
            */

            /*
            xSemaphoreTake(uartMutexLock, portMAX_DELAY);
            USART1_Write((uint8_t*)"Fin tarea uart\r\n", 16);
            xSemaphoreGive(uartMutexLock);
            */
        }
    }
}


/*******************************************************************************
 End of File
 */
