/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    tarea_principal.c

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

#include "tarea_principal.h"
#include "definitions.h"
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
    This structure should be initialized by the TAREA_PRINCIPAL_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

TAREA_PRINCIPAL_DATA tarea_principalData;
/* Mutex used to protect the shared resource - UART */
SemaphoreHandle_t uartMutexLock;
extern uint8_t readByte_global;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

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
    void TAREA_PRINCIPAL_Initialize ( void )

  Remarks:
    See prototype in tarea_principal.h.
 */

void TAREA_PRINCIPAL_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    tarea_principalData.state = TAREA_PRINCIPAL_STATE_INIT;

    /* Create a mutex type semaphore. */
    uartMutexLock = xSemaphoreCreateMutex();

    if( uartMutexLock == NULL)
    {
        /* There was insufficient FreeRTOS heap available for the semaphore to
        be created successfully. */
        USART1_Write((uint8_t*)"Could not create mutex lock\r\n", strlen("Could not create mutex lock\r\n"));
    }

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void TAREA_PRINCIPAL_Tasks ( void )

  Remarks:
    See prototype in tarea_principal.h.
 */

void TAREA_PRINCIPAL_Tasks ( void )
{

    //TickType_t timeNow;

    while (1)
    {
        /*
        xSemaphoreTake(uartMutexLock, portMAX_DELAY);
        USART1_Write((uint8_t*)"Entrada tarea principal\r\n", 25);
        xSemaphoreGive(uartMutexLock);
        */
        portENTER_CRITICAL(); //seccion critica para evitar que se ejecute cambio de contexto
        uint8_t readByte = readByte_global;
        portEXIT_CRITICAL();
        if (readByte != ' ')
        { 
            xSemaphoreTake(uartMutexLock, portMAX_DELAY);
            USART1_Write((uint8_t*)"Tecla: ", 7);
            USART1_Write(&readByte, 1);
            USART1_Write((uint8_t*)"\r\n", 2);
            xSemaphoreGive(uartMutexLock);
            if (readByte == 'L' || readByte == 'l')
            {
                LED1_Toggle();
            }

            portENTER_CRITICAL(); //seccion critica para evitar que se ejecute cambio de contexto
            readByte_global = ' ';
            portEXIT_CRITICAL();
        }else{
            xSemaphoreTake(uartMutexLock, portMAX_DELAY);
            USART1_Write((uint8_t*)"Esperando tecla...\r\n", 20);
            xSemaphoreGive(uartMutexLock);
        }
        
        /* Work done by tarea_principal for 100 ticks 
        timeNow = xTaskGetTickCount();
        while ((xTaskGetTickCount() - timeNow) < 100);
        */
        /*
        xSemaphoreTake(uartMutexLock, portMAX_DELAY);
        USART1_Write((uint8_t*)"Salida tarea principal\r\n", 24);
        xSemaphoreGive(uartMutexLock);
        */

        /* Let idle task run for some time*/
        vTaskDelay(10 / portTICK_PERIOD_MS );
    }
}


/*******************************************************************************
 End of File
 */
