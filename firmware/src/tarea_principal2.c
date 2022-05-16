/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

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
    This structure should be initialized by the tarea_principal_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

tarea_principal_DATA tarea_principalData;

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
    void tarea_principal_Initialize ( void )

  Remarks:
    See prototype in tarea_principal.h.
 */

void tarea_principal_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    tarea_principalData.state = tarea_principal_STATE_INIT;

    /* Create a mutex type semaphore. */
    uartMutexLock = xSemaphoreCreateMutex();

    if( uartMutexLock == NULL)
    {
        /* There was insufficient FreeRTOS heap available for the semaphore to
        be created successfully. */
        USART1_Write((uint8_t*)"Could not create mutex lock\r\n", strlen("Could not create mutex lock\r\n"));
    }
}

/******************************************************************************
  Function:
    void tarea_principal_Tasks ( void )

  Remarks:
    See prototype in tarea_principal.h.
 */

void tarea_principal_Tasks ( void )
{
    TickType_t timeNow;

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
            readByte = ' ';
            portEXIT_CRITICAL();
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
