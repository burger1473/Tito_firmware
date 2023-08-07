/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 13/01/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Libreria CAN bus en modo fd por interrupcion
 *              Funcionamiento: paso 1: Se configura el periferico can con mcan_fd_interrupt_config() 
 *                              paso 2: Para enviar mensaje can: Llamar mcan_fd_interrupt_enviar(), esta funcion configura el frame a enviar con la informacion indicada y cambia el estado del periferico a APP_STATE_MCAN_TRANSMIT (normalmente esta en APP_STATE_MCAN_USER_INPUT)
 *                                      Para recibir mensaje can: No hacer nada
 *                              paso 3: Cuando se finaliza la transmicion o recepcion, se llama automaticamente a la funcion Callback() correspondeinte la cual cambia el estado de la maquina de estados de can a APP_STATE_MCAN_RECEIVE o  APP_STATE_MCAN_TRANSMIT segun la operacion que se concreto
 *                                      y el estado de can pasa a APP_STATE_MCAN_XFER_SUCCESSFUL si todo salio bien o a APP_STATE_MCAN_XFER_ERROR si algo salio mal. En caso de que sera recepcion, el callback toma el dato recibido y lo almacena en variables globales a esta libreria.
 *                              paso 4: Verificar con una tarea el estado de la maquina de estados de mcan con resultado() y cuando sea APP_STATE_MCAN_XFER_SUCCESSFUL procesar el dato recibido con  mcan_fd_interrupt_recibir().
 *                              Nota: No todos los mensajes can recibidos llaman al callback y guardan el dato. En este caso se estan usando filtros para mensajes SDO de CanOpen, es decir para id que comienza con 0x6XX o 0x5XX. se pueden agregar otros valores en mcan_fd_interrupt_config()
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
  #include "mcan_fd_interrupt.h"
  #include <zephyr/kernel.h>
  #include <zephyr/device.h>
  #include <zephyr/devicetree.h>
  #include <zephyr/drivers/can.h>
  #include <zephyr/sys/byteorder.h>
  #include <zephyr/drivers/uart.h>


  //#define mCAN_MODEE_FD  (CAN_MODE_LOOPBACK|CAN_MODE_FD)  //Modo FD con LOOPBACK
  #define mCAN_MODEE_FD  CAN_MODE_FD  //Modo FD
  //#define mCAN_MODEE  (CAN_MODE_LOOPBACK|CAN_MODE_NORMAL)   //Modo normal con LOOPBACK
  #define mCAN_MODEE  CAN_MODE_NORMAL   //Modo normal

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
    static uint32_t xferContext = 0;        //Contexto global de can
    volatile static APP_STATES state = APP_STATE_MCAN_USER_INPUT; //Variable para guardar el estado de la aplicaciÃ³n
    
    static uint32_t rx_messageID = 0;      //Variable para almacenar el id del mensaje can recibido
    static uint8_t rx_message[64] = {0};   //Variable para almacenar el mensaje can recibido  
    static uint8_t rx_messageLength = 0;   //Variable para almacenar el tamaño del mensaje can recibido

    const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));   //Indico el periferico can utilizado

    struct can_frame Mensaje_frame = {      //Creo estructura para almacenar los datos del mensaje a enviar.
		.flags = CAN_FRAME_IDE
	};
    
/*========================[Prototipos]=================================*/    
 
/*=====================[Implementaciones]==============================*/

/*========================================================================
  Funcion: rx_irq_callback
  Descripcion: Callback al cual se llama cuando finalizo una recepcion que pasa el filtro
  Parametro de entrada:
                        const struct device *dev:   Periferico can desde el cual se llamo al callback
                        truct can_frame *frame:     Frame de can que contiene la informacion del mensaje recibido
                        void *user_data             Parametro que indica el usuario para cada filtro añadido
  No retorna nada
  ========================================================================*/
void rx_irq_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{
    rx_messageLength=can_dlc_to_bytes(frame->dlc);                                      //Obtengo el tamaño del mensaje can recibido
    rx_messageID=frame->id;                                                             //Obtengo el id del mensaje can recibido
    
    for (uint8_t loop_count = 0; loop_count < rx_messageLength; loop_count++){          //Obtengo mensaje can recibido
        rx_message[loop_count] = frame->data[loop_count];
    }

    xferContext  = APP_STATE_MCAN_RECEIVE;                                              //Indico que la operacion realizada es recepcion
    state= APP_STATE_MCAN_XFER_SUCCESSFUL;                                              //Indico que se finalizo la recepcion correctamente
   
    #if debug == 1                                                                      //Envio al uart el id y el tamaño del mensaje para debugear
    char destino[35]="                                   ";
    sprintf(destino, " Message - ID : 0x%x Length : 0x%x ", (unsigned int)rx_messageID, (unsigned int)rx_messageLength);
    Uart1_println(destino);
    #endif
}

/*========================================================================
  Funcion: tx_irq_callback
  Descripcion: Callback al cual se llama cuando finalizo una transmision
  Parametro de entrada:
                        const struct device *dev:   Periferico can desde el cual se llamo al callback
                        int error:                  Error que sucede si no se puede enviar. Es cero si se envio correctamente
                        void *arg:                  Argumento de la funcion
  No retorna nada
  ========================================================================*/
void tx_irq_callback(const struct device *dev, int error, void *arg)
{	
    ARG_UNUSED(dev);

	if (error != 0) {                           //Si existio un error al enviar el mensaje por can
        state = APP_STATE_MCAN_XFER_ERROR;      //Cambio estado a mensaje transmitido erroneamente
        #if debug == 1
        Uart1_println("Error envio can");       //printf("Callback! error-code: %d\nSender: %s\n", error, sender);
        #endif
	}else{                                      //Si se transmitio correctamente
        state = APP_STATE_MCAN_XFER_SUCCESSFUL; //Cambio estado a  transmitido correctamente
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
    
    if(state == APP_STATE_MCAN_XFER_SUCCESSFUL && (APP_STATES)xferContext == APP_STATE_MCAN_RECEIVE){
        k_sched_lock();                                       //Seccion critica para evitar que se ejecute cambio de contexto alterando el proceso de guardado de la variable
        *rx_messageID2=rx_messageID;
        for(uint8_t i=0; i<rx_messageLength; i++){
            rx_message2[i]=rx_message[i];
        }
        *rx_messageLength2=rx_messageLength;
        k_sched_unlock();                                      //Salgo de seccion critica
        return true;                                           //Retorno falso si se recibio mensaje
    }else{
        return false;                                          //Retorno falso si no se recibio mensaje
    }
}

/*========================================================================
  Funcion: mcan_fd_interrupt_config
  Descripcion: Establece la configuracion del periferico can y configura los filtros de recepcion
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void mcan_fd_interrupt_config(void){
	int ret;                                       //Variable para indicar el retorno de funciones

	if (!device_is_ready(can_dev)) {               //Si el dispositivo can no esta listo
        #if debug == 1
        Uart1_println("CAN: Device not ready.");   //printf("CAN: Device %s not ready.\n", can_dev->name);
        #endif
	}

    can_set_bitrate(can_dev, 0x7A120);             //Configuro el bitrate de mensaje normal
    can_set_bitrate_data(can_dev, 0x1E8480);       //Caonfiguro el bitrate del mensaje FD
    ret = can_set_mode(can_dev, mCAN_MODEE_FD);        //Activo modo can en modo mensaje normal
	#if debug == 1
    if (ret != 0) {
        Uart1_println("Error setting CAN mode");  //printf("Error setting CAN mode [%d]", ret);
	}
    #endif

	ret = can_start(can_dev);                     //Inicio el periferico CAN
    #if debug == 1
	if (ret != 0) {
        Uart1_println("Error starting CAN controller"); //printf("Error starting CAN controller [%d]", ret);
	}
    #endif

    const struct can_filter filter1 = {         //Estructura que contiene el filtro a aplicara para los mensajes can recibidos, los que cumplen con el filtro llaman al callback rx
		.flags = CAN_FILTER_DATA,               //Filtro normal sin FD
		.id = 0x600,                            //Base de ID a filtrar
		.mask = 0x600                           //Mascara de los bits que deben corresponder para pasar el filtro. 1 debe contener 0 ignora bit
	};

    ret = can_add_rx_filter(can_dev, &rx_irq_callback, "TSDO", &filter1);  //Agrego que contiene la estructura anterior e indico callback al que debe llamar cuando el mensaje recibido pase el filtro.

    #if debug == 1
	if (ret == -ENOSPC) {                       //Si no se puedo agregar el filtro
        Uart1_println("Error, no filter available!");
	}
    #endif

    const struct can_filter filter2 = {         //Estructura que contiene el filtro a aplicara para los mensajes can recibidos, los que cumplen con el filtro llaman al callback rx
		.flags = CAN_FILTER_DATA,               //Filtro normal sin FD
		.id = 0x580,                            //Base de ID a filtrar
		.mask = 0x500                           //Mascara de los bits que deben corresponder para pasar el filtro. 1 debe contener 0 ignora bit
	};

    ret = can_add_rx_filter(can_dev, &rx_irq_callback, "RSDO", &filter2);   //Agrego que contiene la estructura anterior e indico callback al que debe llamar cuando el mensaje recibido pase el filtro.

	#if debug == 1
	if (ret == -ENOSPC) {                       //Si no se puedo agregar el filtro
        Uart1_println("Error, no filter available!");
	}
    Uart1_println("Finished init");
    #endif
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
        if(MCAN_MODE_L==MCAN_MODE_FD_EXTENDED || MCAN_MODE_L==MCAN_MODE_EXTENDED){      //Si es mensaje  es id extendido
            Mensaje_frame.flags|=CAN_FRAME_IDE;
        }

        if(MCAN_MODE_L==MCAN_MODE_NORMAL){                                              //Si el mensaje es id normal
            Mensaje_frame.flags=0;
        }

        if(MCAN_MODE_L==MCAN_MODE_FD_STANDARD || MCAN_MODE_L==MCAN_MODE_FD_EXTENDED){                   //Si es mensaje es FD
            Mensaje_frame.flags=0;
            Mensaje_frame.flags|=CAN_FRAME_FDF;
        }

        Mensaje_frame.id=messageID;
        
        for (uint8_t loop_count = 0; loop_count < messageLength; loop_count++){
            Mensaje_frame.data[loop_count] = message[loop_count];
        }

        Mensaje_frame.dlc=can_bytes_to_dlc(messageLength);
        /* This sending call is none blocking. */
        uint8_t res = can_send(can_dev, &Mensaje_frame, K_FOREVER, tx_irq_callback, "TX IRQ");
        /* This sending call is blocking until the message is sent. */
        //can_send(can_dev, &Mensaje_frame, K_MSEC(100), NULL, NULL);
        if (res != 0)
        {
            return false;       //Retorno falso si no se pudo enviar
        }
        state = APP_STATE_MCAN_TRANSMIT;
        xferContext = APP_STATE_MCAN_TRANSMIT;
        return true;
    }else{
        return false;       //Retorno falso si no se pudo enviar
    }
}


/*========================================================================
  Funcion: mcan_resultado()
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
uint8_t mcan_resultado(void){
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


/*
char *state_to_str(enum can_state state)
{
	switch (state) {
	case CAN_STATE_ERROR_ACTIVE:
		return "error-active";
	case CAN_STATE_ERROR_WARNING:
		return "error-warning";
	case CAN_STATE_ERROR_PASSIVE:
		return "error-passive";
	case CAN_STATE_BUS_OFF:
		return "bus-off";
	case CAN_STATE_STOPPED:
		return "stopped";
	default:
		return "unknown";
	}
}



void state_change_work_handler(struct k_work *work)
{
	printf("State Change ISR\nstate: %s\n"
	       "rx error count: %d\n"
	       "tx error count: %d\n",
		state_to_str(current_state),
		current_err_cnt.rx_err_cnt, current_err_cnt.tx_err_cnt);

#ifndef CONFIG_CAN_AUTO_BUS_OFF_RECOVERY
	if (current_state == CAN_STATE_BUS_OFF) {
		printf("Recover from bus-off\n");

		if (can_recover(can_dev, K_MSEC(100)) != 0) {
			printf("Recovery timed out\n");
		}
	}
#endif //CONFIG_CAN_AUTO_BUS_OFF_RECOVERY
}
*/
