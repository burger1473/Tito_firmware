/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 16/05/2022 
 * Update: 29/10/2022
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: MPLABX - Harmony
 *
 * DescripciÃ³n: Libreria CAN bus en modo fd por interrupcion
 *              Funcionamiento: paso 1: Se configura la ram del mensaje can mcan_fd_interrupt_config() y la maquina de estado de can empieza como APP_STATE_MCAN_USER_INPUT
 *                              paso 2: Llamar mcan_fd_interrupt_enviar() o mcan_fd_interrupt_recibir()
 *                                      cuando se llama a estas funciones, el estado can cambia de APP_STATE_MCAN_USER_INPUT a  APP_STATE_MCAN_IDLE (can incativo)
 *                              paso 3: Cuando se finaliza la transmicion o recepcion, se llama automaticamente a la funcion APP_MCAN_Callback() la cual cambia el estado de la maquina de estados de can
 *                                      a APP_STATE_MCAN_XFER_SUCCESSFUL si todo salio bien o a APP_STATE_MCAN_XFER_ERROR si algo salio mal
 *                              paso 4: Verificar con una tarea el estado de la maquina de estados de mcan y cuando sea APP_STATE_MCAN_XFER_SUCCESSFUL procesar el dato recibido.
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
  #include "mcan_fd_interrupt.h"
  #include <stddef.h>                       //Define NULL
  #include <stdbool.h>                      //Define true
  #include <stdlib.h>                       //Define EXIT_FAILURE
  #include <stdio.h>                        //Para sizeof
  #include "definitions.h"                  //Prototipos de funciones SYS
  #include "../Usart1_FreeRTOS/Uart1_FreeRTOS.h"

  /* Standard identifier id[28:18]*/
  #define WRITE_ID(id) (id << 18)
  #define READ_ID(id)  (id >> 18)

/*=====================[Variables]================================*/
    typedef enum
    {
        APP_STATE_MCAN_RECEIVE,             //Estado Recibiendo mensaje
        APP_STATE_MCAN_TRANSMIT,            //Estado transmitiendo mensaje
        APP_STATE_MCAN_IDLE,                //Estado mcan inactivo
        APP_STATE_MCAN_XFER_SUCCESSFUL,     //Estado mensaje recibido o transmitido correctamente
        APP_STATE_MCAN_XFER_ERROR,          //Estado mensaje recibido o transmitido erroneamente
        APP_STATE_MCAN_USER_INPUT           //Esperando al usuario para enviar o recibir mensaje
    } APP_STATES;                           //Enumaracion de los estados posibles
    
    /* Variable to save Tx/Rx transfer status and context */
    static uint32_t status = 0;
    static uint32_t xferContext = 0;        //Contexto global de can
    
    volatile static APP_STATES state = APP_STATE_MCAN_USER_INPUT; //Variable para guardar el estado de la aplicaciÃ³n
    static uint8_t txFiFo[MCAN1_TX_FIFO_BUFFER_SIZE];
    static uint8_t rxFiFo0[MCAN1_RX_FIFO0_SIZE];
    static uint8_t rxFiFo1[MCAN1_RX_FIFO1_SIZE];
    static uint8_t rxBuffer[MCAN1_RX_BUFFER_SIZE];

    static uint32_t rx_messageID = 0;
    static uint8_t rx_message[64] = {0};
    static uint8_t rx_messageLength = 0;
    static uint16_t timestamp = 0;

/*========================[Prototipos]=================================*/    

 
/*=====================[Implementaciones]==============================*/


/* Message Length to Data length code */
static uint8_t MCANLengthToDlcGet(uint8_t length)
{
    uint8_t dlc = 0;

    if (length <= 8U)
    {
        dlc = length;
    }
    else if (length <= 12U)
    {
        dlc = 0x9U;
    }
    else if (length <= 16U)
    {
        dlc = 0xAU;
    }
    else if (length <= 20U)
    {
        dlc = 0xBU;
    }
    else if (length <= 24U)
    {
        dlc = 0xCU;
    }
    else if (length <= 32U)
    {
        dlc = 0xDU;
    }
    else if (length <= 48U)
    {
        dlc = 0xEU;
    }
    else
    {
        dlc = 0xFU;
    }
    return dlc;
}

/* Data length code to Message Length */
static uint8_t MCANDlcToLengthGet(uint8_t dlc)
{
    uint8_t msgLength[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};
    return msgLength[dlc];
}

/*========================================================================
  Funcion: Save_message
  Descripcion: Esta función guarda el mensaje recibido por can (por los diferentes buffer o fifo) y lo almacena en variables globales de esta libreria
  No retorna nada
  ========================================================================*/
static void Save_message(uint8_t numberOfMessage, MCAN_RX_BUFFER *rxBuf, uint8_t rxBufLen, uint8_t rxFifoBuf)
{
    uint8_t length = 0;                                                         //Variable que contiene el tamaño actual
    //rx_messageLength = 0;                                                       //Variable para almacenar tamaño del mensaje
    #ifdef debug
        if (rxFifoBuf == 0){                                                    //Si el mensaje proviene de la fifo0
                Uart1_print(" Rx FIFO0 :");
        }
        else if (rxFifoBuf == 1){                                               //Si el mensaje proviene de la fifo1
                Uart1_print(" Rx FIFO1 :");
        }else if (rxFifoBuf == 2){
                Uart1_print(" Rx Buffer :");                                    //Si el mensaje proviene del buffer
        }
    #endif

    for (uint8_t count = 0; count < numberOfMessage; count++)                   //Recorro todos los mensajes
    {
        #ifdef debug
            Uart1_println(" New Message Received");
        #endif
        portENTER_CRITICAL();                                                   //Seccion critica para evitar que se ejecute cambio de contexto alterando el proceso de guardado de la variable
        rx_messageID = rxBuf->xtd ? rxBuf->id : READ_ID(rxBuf->id);             //Obtengo ID
        rx_messageLength = MCANDlcToLengthGet(rxBuf->dlc);                      //Obtengo tamaño mensaje
        length = rx_messageLength;                                              //Valorizo el tamaño actual con el tamaño real del mensaje
        timestamp=(unsigned int)rxBuf->rxts;                                    //Obtengo el timestamp
        portEXIT_CRITICAL();                                                    //Salgo de seccion critica
        #ifdef debug
            //Uart1_print(" Message - Timestamp : 0x%x ID : 0x%x Length : 0x%x ", (unsigned int)rxBuf->rxts, (unsigned int)rx_messageID, (unsigned int)rx_messageLength);
            Uart1_print(" Message - Timestamp : 0x");
            Uart1_print(timestamp);
            Uart1_print(" ID : 0x");
            Uart1_print((unsigned int)rx_messageID);
            Uart1_print(" Length : 0x");
            Uart1_print((unsigned int)rx_messageLength);
            Uart1_print("Message : ");
        #endif
        uint8_t posicion=0;                                                    //Variable para guardar la posicion actual del array
        while(length)                                                          //Recorro el array que contiene el mensaje
        {
            posicion=rx_messageLength - length--;                              //Posicion actual del array
            portENTER_CRITICAL();                                                   //Seccion critica para evitar que se ejecute cambio de contexto alterando el proceso de guardado de la variable
            rx_message[posicion]=rxBuf->data[posicion];                        //Guardo mensaje en variable global
            portEXIT_CRITICAL();                                                    //Salgo de seccion critica
            #ifdef debug
                Uart1_print("0x");
                Uart1_print(&rxBuf->data[posicion]);
            #endif
        }
        #ifdef debug
            Uart1_println(" ");
        #endif
        rxBuf += rxBufLen;                                                     //Recorro el buffer
    }
}


/*========================================================================
  Funcion: APP_MCAN_TxFifoCallback
  Descripcion: Esta función será llamada por MCAN PLIB cuando se complete la transferencia desde Tx FIFO
  Parametro de entrada:
                        uintptr_t context:
   No retorna nada
  ========================================================================*/
void APP_MCAN_TxFifoCallback(uintptr_t context)
{
    xferContext = context;                                              //Valorizo el contexto global
    status = MCAN1_ErrorGet();                                          //Comprueba el estado de MCAN

    if (((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_NONE) || ((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_LEC_NO_CHANGE))   //Si no hay error can o si no hay cambio can
    {
        switch ((APP_STATES)context)                                    //Segun el contexto:
        {
            case APP_STATE_MCAN_TRANSMIT:                               //Si el contexto del callback es por transmicion de can
            {
                state = APP_STATE_MCAN_XFER_SUCCESSFUL;                 //Estado mensaje recibido o transmitido correctamente
                break;                                                  //Salgo de switch
            }
            default:                                                    //En cualquier otro contexto
                break;                                                  //Salgo de switch
        }
    }
    else                                                                //Sino
    {
        state = APP_STATE_MCAN_XFER_ERROR;                              //Estado mensaje recibido o transmitido erroneamente
    }
}

/*========================================================================
  Funcion: APP_MCAN_RxBufferCallback
  Descripcion: Esta función será llamada por MCAN PLIB cuando se reciba un mensaje en Rx Buffer
  Parametro de entrada:
                        uint8_t bufferNumber
                        uintptr_t context:
   No retorna nada
  ========================================================================*/
void APP_MCAN_RxBufferCallback(uint8_t bufferNumber, uintptr_t context)
{
    xferContext = context;                                              //Valorizo el contexto global
    status = MCAN1_ErrorGet();                                          //Comprueba el estado de MCAN

    if (((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_NONE) || ((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_LEC_NO_CHANGE))   //Si no hay error can o si no hay cambio can
    {
        switch ((APP_STATES)context)                                    //Segun el contexto:
        {
            case APP_STATE_MCAN_RECEIVE:                                //Si el contexto del callback es por recepcion de can
            {
                memset(rxBuffer, 0x00, MCAN1_RX_BUFFER_ELEMENT_SIZE);   //memset(void *str, int c, size_t n) copia el caracter c (un caracter sin signo) en los primeros n caracteres de la cadena a la que apunta el argumento str .
                if (MCAN1_MessageReceive(bufferNumber, (MCAN_RX_BUFFER *)rxBuffer) == true) //Recibe un mensaje por can
                {                                                       //Si se pudo recibir
                    Save_message(1, (MCAN_RX_BUFFER *)rxBuffer, MCAN1_RX_BUFFER_ELEMENT_SIZE, 2);  //Imprimo mensaje y almaceno en variable local
                    state = APP_STATE_MCAN_XFER_SUCCESSFUL;             //Estado mensaje recibido o transmitido correctamente
                }
                else                                                    //Si no se pudo recibir
                {
                    state = APP_STATE_MCAN_XFER_ERROR;                  //Estado mensaje recibido o transmitido erroneamente
                }
                break;                                                  //Salgo del switch
            }
            default:                                                    //Si esta en otro estado
                break;                                                  //Salgo del switch
        }
    }
    else                                                                //Si se produjo un error
    {
        state = APP_STATE_MCAN_XFER_ERROR;                              //Estado mensaje recibido o transmitido erroneamente
    }
}

/*========================================================================
  Funcion: APP_MCAN_RxFifo0Callback
  Descripcion: Esta función será llamada por MCAN PLIB cuando se reciba un mensaje en Rx FIFO0
  Parametro de entrada:
                        uint8_t numberOfMessage
                        uintptr_t context:
   No retorna nada
  ========================================================================*/
void APP_MCAN_RxFifo0Callback(uint8_t numberOfMessage, uintptr_t context)
{   
    xferContext = context;                                              //Valorizo el contexto global          
    status = MCAN1_ErrorGet();                                          //Comprueba el estado de MCAN

    if (((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_NONE) || ((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_LEC_NO_CHANGE))   //Si no hay error can o si no hay cambio can
    {
        switch ((APP_STATES)context)                                    //Segun el contexto:
        {
            case APP_STATE_MCAN_RECEIVE:                                //Si el contexto del callback es por recepcion de can
            {
                memset(rxFiFo0, 0x00, (numberOfMessage * MCAN1_RX_FIFO0_ELEMENT_SIZE));    //memset(void *str, int c, size_t n) copia el caracter c (un caracter sin signo) en los primeros n caracteres de la cadena a la que apunta el argumento str .
                if (MCAN1_MessageReceiveFifo(MCAN_RX_FIFO_0, numberOfMessage, (MCAN_RX_BUFFER *)rxFiFo0) == true) //Recibe un mensaje por can
                {                                                       //Si se pudo recibir
                    Save_message(numberOfMessage, (MCAN_RX_BUFFER *)rxFiFo0, MCAN1_RX_FIFO0_ELEMENT_SIZE, 0);    //Imprimo mensaje y almaceno en variable local
                    state = APP_STATE_MCAN_XFER_SUCCESSFUL;             //Estado mensaje recibido o transmitido correctamente
                }
                else                                                    //Si no se pudo recibir
                {
                    state = APP_STATE_MCAN_XFER_ERROR;                  //Estado mensaje recibido o transmitido erroneamente
                }
                break;                                                  //Salgo del switch
            }
            default:                                                    //Si esta en otro estado
                break;                                                  //Salgo del switch
        }
    }
    else                                                                //Si se produjo un error
    {
        state = APP_STATE_MCAN_XFER_ERROR;                              //Estado mensaje recibido o transmitido erroneamente
    }
}

/*========================================================================
  Funcion: APP_MCAN_RxFifo1Callback
  Descripcion: Esta función será llamada por MCAN PLIB cuando se reciba un mensaje en Rx FIFO1
  Parametro de entrada:
                        uint8_t numberOfMessage
                        uintptr_t context:
   No retorna nada
  ========================================================================*/
void APP_MCAN_RxFifo1Callback(uint8_t numberOfMessage, uintptr_t context)
{
    xferContext = context;                                              //Valorizo el contexto global          
    status = MCAN1_ErrorGet();                                          //Comprueba el estado de MCAN

    if (((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_NONE) || ((status & MCAN_PSR_LEC_Msk) == MCAN_ERROR_LEC_NO_CHANGE)) //Si no hay error can o si no hay cambio can
    {
        switch ((APP_STATES)context)                                    //Segun el contexto:
        {
            case APP_STATE_MCAN_RECEIVE:                                //Si el contexto del callback es por recepcion de can
            {
                memset(rxFiFo1, 0x00, (numberOfMessage * MCAN1_RX_FIFO1_ELEMENT_SIZE));     //memset(void *str, int c, size_t n) copia el caracter c (un caracter sin signo) en los primeros n caracteres de la cadena a la que apunta el argumento str .
                if (MCAN1_MessageReceiveFifo(MCAN_RX_FIFO_1, numberOfMessage, (MCAN_RX_BUFFER *)rxFiFo1) == true) //Recibe un mensaje por can
                {                                                       //Si se pudo recibir
                    Save_message(numberOfMessage, (MCAN_RX_BUFFER *)rxFiFo1, MCAN1_RX_FIFO1_ELEMENT_SIZE, 1);  //Imprimo mensaje y almaceno en variable local
                    state = APP_STATE_MCAN_XFER_SUCCESSFUL;             //Estado mensaje recibido o transmitido correctamente
                }
                else                                                    //Si no se pudo recibir
                {
                    state = APP_STATE_MCAN_XFER_ERROR;                  //Estado mensaje recibido o transmitido erroneamente
                }   
                break;                                                  //Salgo del switch
            }
            default:                                                    //Si esta en otro estado
                break;                                                  //Salgo del switch
        }
    }
    else                                                                //Si se produjo un error
    {
        state = APP_STATE_MCAN_XFER_ERROR;                              //Estado mensaje recibido o transmitido erroneamente
    }
}


/*========================================================================
  Funcion: mcan_fd_interrupt_recibir
  Descripcion: Recibe mensaje por canbus
  Parametro de entrada:
                        uint32_t *rx_messageID:     Puntero de la variable donde se guardara el Id can del mensaje recibido (de 11 bits/29 bits).
                        uint8_t *rx_message:        Puntero de la variable donde guardar el mensaje
                        uint8_t *rx_messageLength:  Puntero de la variable donde guardar el tamaÃ±o del mensaje
  Retorna: dato bool indicando si se pudo transmitir el mensaje true o false.
  ========================================================================*/
bool mcan_fd_interrupt_recibir(uint32_t *rx_messageID2, uint8_t *rx_message2, uint8_t *rx_messageLength2){  
    if(state == APP_STATE_MCAN_XFER_SUCCESSFUL){
        portENTER_CRITICAL();                                  //Seccion critica para evitar que se ejecute cambio de contexto alterando el proceso de guardado de la variable
        rx_messageID2=rx_messageID;
        rx_message2=rx_message;
        rx_messageLength2=rx_messageLength;
        portEXIT_CRITICAL();                                   //Salgo de seccion critica
        return true;                                           //Retorno falso si se recibio mensaje
    }else{
        return false;                                          //Retorno falso si no se recibio mensaje
    }
}

/*========================================================================
  Funcion: mcan_fd_interrupt_config
  Descripcion: Establece la configuracion de RAM de mensajes can. Previamente se debe haber llamado a MCAN1_Initialize para la instancia de MCAN asociada.
  Parametro de entrada:
                        uint8_t *msgRAMConfigBaseAddress: Puntero a la direccion base del bufer asignada a la aplicaciÃ³n. La aplicaciÃ³n debe asignar el bÃºfer desde la memoria contigua no almacenada en cachÃ© y el tamaÃ±o del bÃºfer debe ser MCAN1_MESSAGE_RAM_CONFIG_SIZE
                                Ej: uint8_t Can1MessageRAM[MCAN1_MESSAGE_RAM_CONFIG_SIZE] __attribute__((aligned (32)))__attribute__((space(data), section (".ram_nocache")));
  No retorna nada
  ========================================================================*/
void mcan_fd_interrupt_config(uint8_t *msgRAMConfigBaseAddress){
    MCAN1_MessageRAMConfigSet(msgRAMConfigBaseAddress);     //Establece configuraciÃ³n de RAM de mensajes
    MCAN1_RxFifoCallbackRegister(MCAN_RX_FIFO_0, APP_MCAN_RxFifo0Callback, APP_STATE_MCAN_RECEIVE);  //Configuro callback recepcion por fifo0
    MCAN1_RxFifoCallbackRegister(MCAN_RX_FIFO_1, APP_MCAN_RxFifo1Callback, APP_STATE_MCAN_RECEIVE);  //Configuro callback recepcion por fifo01
    MCAN1_RxBuffersCallbackRegister(APP_MCAN_RxBufferCallback, APP_STATE_MCAN_RECEIVE);              //Configuro callback recepcion por buffer
}


/*========================================================================
  Funcion: mcan_fd_interrupt_enviar
  Descripcion: Envia mensaje por canbus
  Parametro de entrada:
                        uint32_t messageID:     Id can a donde enviar el mensaje de 11 bits/29 bits.
                        uint8_t *message:       Puntero del mensaje a enviar
                        uint8_t messageLength:  TamaÃ±o del arreglo a enviar
                        MCAN_MODE MCAN_MODE:    Modo de operacion can
                                                Mensajes FD (can FD- alta velocidad) con id estandar:                   MCAN_MODE_FD_STANDARD
                                                Mensajes FD (can FD- alta velocidad) con id extendido:                  MCAN_MODE_FD_EXTENDED
                                                Mensajes estandar normal (can clasico) hasta 8 bytes:                   MCAN_MODE_NORMAL
  Retorna: dato bool indicando si se pudo transmitir el mensaje true o false.
  Nota: los mensajes FD puedene enviar de 0 a 64 bytes, mientras que los menajes normales pueden enviar de 0 a 8 bytes.
  ========================================================================*/
bool mcan_fd_interrupt_enviar(uint32_t messageID , uint8_t *message, uint8_t messageLength, MCAN_MODE MCAN_MODE_L){
    if (state == APP_STATE_MCAN_USER_INPUT){
        MCAN_TX_BUFFER *txBuffer = NULL;
        memset(txFiFo, 0x00, MCAN1_TX_FIFO_BUFFER_ELEMENT_SIZE);
        txBuffer = (MCAN_TX_BUFFER *)txFiFo;
        txBuffer->id = WRITE_ID(messageID);
        txBuffer->dlc = MCANLengthToDlcGet(messageLength);
        if(MCAN_MODE_L==MCAN_MODE_FD_EXTENDED){                                              //Si es mensaje FD extendido
        txBuffer->xtd = 1;
        }
        if(MCAN_MODE_L==MCAN_MODE_FD_STANDARD || MCAN_MODE_L==MCAN_MODE_FD_EXTENDED){                   //Si es mensaje FD
            txBuffer->fdf = 1;
            txBuffer->brs = 1;
        }
        for (uint8_t loop_count = 0; loop_count < messageLength; loop_count++){
			txBuffer->data[loop_count] = message[loop_count];
		}
        MCAN1_TxFifoCallbackRegister( APP_MCAN_TxFifoCallback, (uintptr_t)APP_STATE_MCAN_TRANSMIT );
        state = APP_STATE_MCAN_IDLE;
        if (MCAN1_MessageTransmitFifo(1, txBuffer) == false)
        {
            return false;       //Retorno falso si no se pudo enviar
        }
        
        return true;                                                                              //Retorno verdadero si se pudo enviar
    }else{
        return false;       //Retorno falso si no se pudo enviar
    }
}


/*========================================================================
  Funcion: Resultado
  Descripcion: Indica el resultado del can bus luego de llamar mcan_fd_interrupt_enviar() o mcan_fd_interrupt_recibir() para determina si un mensaje se envio o recibio correctamente. 
               Esta pensado para llamar a esta funcion continuamente en una tarea para determinar si hay un nuevo dato recibido correctamente luego de llamar mcan_fd_interrupt_recibir().
               Esta pensado para llamar a esta funcion continuamente en una tarea para determinar si se envio correctamente el mensaje can luego de llamar a la funcion mcan_fd_interrupt_enviar().
  Sin parameto de entrada.
  Retorna: uint8_t estado: indica el estado del can bus:
                        0 = no se esta esperando una transmision o recepcion
                        1 = Se recibio correctamente un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_recibir()
                        2 = Se transmitio correctamente un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_enviar()
                        3 = Error al recibir un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_recibir()
                        4 = Error al transmitir un dato por can bus luego de llamar a la funcion mcan_fd_interrupt_enviar()
  ========================================================================*/
uint8_t Resultado(void){
    uint8_t resultado=0;                                                        //Variable para retornar
    switch (state)
    {
        case APP_STATE_MCAN_XFER_SUCCESSFUL:                                    //Si la transmicion o recepcion se realizo con exito
        {
            if ((APP_STATES)xferContext == APP_STATE_MCAN_RECEIVE)              //Si el contexto era de recepcion
            {
                resultado=1;                                                    //Se recibio correctamente
            } 
            else if ((APP_STATES)xferContext == APP_STATE_MCAN_TRANSMIT)        //Si el contexto era de transmision
            {
                resultado=2;                                                    //Se transmitio correctamente
            }
            break;
        }
        case APP_STATE_MCAN_XFER_ERROR:                                         //Si la transmicion o recepcion fue erronea
        {
            if ((APP_STATES)xferContext == APP_STATE_MCAN_RECEIVE)              //Si el contecto era de recepcion
            {
                resultado=3;                                                    //Error al recibir mensaje
            }
            else                                                                //Sino era de transmicion
            {
                resultado=4;                                                    //Error al enviar mensaje
            }
            break;
        }
        default:                                                                //En cualquier otro estado del can
            break;
    }
    return resultado;                                                           //Retorna el resultado
}

/*========================================================================
  Funcion: mcan_fd_interrupt_habilitar()
  Descripcion: Establece el estado de la maquina de estado de can en user_input para poder configurar una nueva transmicion o recepcion de datos can
               Recordar que las funciones mcan_fd_interrupt_recibir() y mcan_fd_interrupt_enviar() no se ejecutan si el estado de la maquina no es  APP_STATE_MCAN_USER_INPUT.
               Con esto se evita que si todavia no se tomo el dato que llego por can, no se sobreescriba.
  Sin parameto de entrada.
  No retorna nada
  ========================================================================*/
void mcan_fd_interrupt_habilitar(void){
    state = APP_STATE_MCAN_USER_INPUT;
}

