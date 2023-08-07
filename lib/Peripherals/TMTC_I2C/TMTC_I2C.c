/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 23/02/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Libreria I2C para telemetria y telecomando para SAMV71
*===========================================================================*/

/*=====================[ Inclusiones ]============================*/
  #include "TMTC_I2C.h"
  #include "../Usart1/Uart1.h"

/*=================[Prototipos de funciones]======================*/

/*=====================[Variables]================================*/
uint8_t rx_buf[16] = {0};
const struct device *const I2C_DEV_NAME = DEVICE_DT_GET(DT_ALIAS(i2c_0));
uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;
struct k_mutex communication_sequence_mutex;

/*========================================================================
  Funcion: TMTC_I2C_init()
  Descripcion: Inicializa I2C para TMTC
  Sin parametro de entrada
  Rertorna: uint8_t   0 = resultado ok
                      1 = No se pudo inicializar
  ========================================================================*/
uint8_t TMTC_I2C_init(void) {
  if (!device_is_ready(I2C_DEV_NAME)) {
      return 1;
  }
  /* 1. Verify i2c_configure() */
  if (i2c_configure(I2C_DEV_NAME, i2c_cfg)) {
    return 1;
  }
  uint32_t i2c_cfg_tmp;
  /* 2. Verify i2c_get_config() */
  if (i2c_get_config(I2C_DEV_NAME, &i2c_cfg_tmp)) {
    return 1;
  }
  if (i2c_cfg != i2c_cfg_tmp) {
    return 1;
  }
  k_mutex_init(&communication_sequence_mutex);
  return 0;
}

/*========================================================================
  Funcion: TMTC_I2C_sendMsg()
  Descripcion: Envia TM al bridge por i2c
  Parametro de entrada: uint8_t *tx_buf: puntero al buffer del mensaje a enviar
                        uint8_t n_bytes: numeros de bytes a enviar
                        uint16_t direccion: direccion del dispositivo i2c al cual se quiere enviar mensaje
                        uint16_t port:      puerto udp por el ual se queire enviar
  Rertorna: uint8_t   0 = resultado ok
                      1 = No se pudo realizar
  ========================================================================*/
uint8_t TMTC_I2C_sendMsg(const uint8_t *tx_buf, uint8_t n_bytes, uint16_t direccion, uint16_t port) {
  if (k_mutex_lock(&communication_sequence_mutex, K_MSEC(100)) == 0) {    //Se toma semaforo de comunicacion para adminitrar recurso compartido entre hilos
    //Primero se envia mensaje para indicar que la siguiente recepcion es un envio de TM
    uint8_t buffer_send_and_port[3];
    buffer_send_and_port[0] = 'S';                  //'S' para indicar que es envio de TM 
    buffer_send_and_port[1] = (port & 0xFF00) >> 8; //Puerto por el cual se desea enviar
    buffer_send_and_port[2] = port & 0xFF;          //Puerto por el cual se desea enviar
    if (i2c_write(I2C_DEV_NAME, buffer_send_and_port, 3, direccion)) {  //Se envia por i2c
      Uart1_println("Aviso Envio ERROR ");
    } else {
      Uart1_println("Aviso Envio OK");
    }
    if (i2c_write(I2C_DEV_NAME, tx_buf, n_bytes, direccion)) {  //Se envia telemetria
      k_mutex_unlock(&communication_sequence_mutex);            //Se libera semaforo
      return 1; 
    }
    k_mutex_unlock(&communication_sequence_mutex);             //Se libera semaforo
    return 0;
  } else {
    return 1;
  }
}

  /*========================================================================
  Funcion: TMTC_I2C_readMsg()
  Descripcion: Lee TC o confirmacion de TM desde bridge por i2c
  Parametro de entrada: uint8_t *buf: puntero al buffer donde se desea guardar el mensaje
                        uint8_t n_bytes: numeros de bytes que se quieren recibir
                      :  uint16_t direccion: direccion del dispositivo i2c al cual se quiere enviar mensaje
  Rertorna: uint8_t   0 = resultado ok
                      1 = No se pudo realizar
  ========================================================================*/
uint8_t TMTC_I2C_readMsg(uint8_t *rx_buf, size_t bufferSize, size_t *readBytes, uint16_t direccion) {
  if (k_mutex_lock(&communication_sequence_mutex, K_MSEC(100)) == 0) { //Se toma semaforo de comunicacion para adminitrar recurso compartido entre hilos
    // Envia P para solicitar el size del ultimo paquete que esta en la cola del udpbrige
    uint8_t buffer_pedido[1] = {"P"};
    if (i2c_write(I2C_DEV_NAME, buffer_pedido, 1, 0x50)) {
      Uart1_println("Pedido ERROR ");
    } else {
      Uart1_println("Pedido  OK ");
    }
    uint8_t largo = 0;  //Variable para guardar el largo del mensaje a pedir por i2c al udpbridge
    // Lee por i2c desde udpbridge la respuesta de un solo byte que contiene el size del ultimo paquete de la cola de recepcion del udpbridge 
    uint8_t buffer[1];
    k_msleep(5);        //Espera un tiempo para que el udpbridge procese la solicitud anterior
    if (i2c_read(I2C_DEV_NAME, buffer, 1, direccion)) {   //Lee por i2c el tamaño
      k_mutex_unlock(&communication_sequence_mutex);      //Si falla, libera semaforo
      return 1;
    }
    largo = buffer[0];
    *readBytes = largo;
    if (largo > bufferSize) {                             //Si el largo a leer es mayor a lo que puede contener el buffer
      k_mutex_unlock(&communication_sequence_mutex);      //Si falla, libera semaforo
      return 1;
    }
    if (largo > 0) {                                      //Sino
      k_msleep(5);                                        //Espera un tiempo para que el udpbridge procese la solicitud anterior
      if (i2c_read(I2C_DEV_NAME, rx_buf, largo, direccion)) {
        Uart1_println("Error");
        k_mutex_unlock(&communication_sequence_mutex);    //Si falla, libera semaforo
        return 1;
      }
    } else {    //Si largo no es un numero mayor a 0, no lee nada.
      k_mutex_unlock(&communication_sequence_mutex);     //Si falla, libera semaforo
      return 1;
    }
    k_mutex_unlock(&communication_sequence_mutex);       //Se libera semaforo
    return 0;
  } else {
    return 1;
  }
}

  /*========================================================================
  Funcion: TMTC_I2C_writeandreadMsg()
  Descripcion: Lee TC o confirmacion de TM desde bridge por i2c
  Parametro de entrada: uint8_t *buf: puntero al buffer donde se desea guardar el mensaje
                      :  uint16_t direccion: direccion del dispositivo i2c al cual se quiere enviar mensaje
  Rertorna: uint8_t   0 = resultado ok
                      1 = No se pudo realizar
  ========================================================================*/
  uint8_t TMTC_I2C_writeandreadMMsg(uint8_t *tx_buf, uint8_t *rx_buf, uint8_t n_bytes, uint16_t direccion){
    struct i2c_msg msgs[2] = {
        {
            .buf = tx_buf,
            .len = strlen(tx_buf),
            .flags = I2C_MSG_WRITE,
        },
        {
            .buf = rx_buf,
            .len = n_bytes,
            .flags = I2C_MSG_READ | I2C_MSG_STOP,
        },
    };

    int ret = i2c_transfer(I2C_DEV_NAME, msgs, 2, direccion);
    if (ret != 0) {
        return 1;
    }
    return 0;
  }