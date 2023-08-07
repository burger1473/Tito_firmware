/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 12/01/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripción: Libreria para enviar y recibir datos por uart
 * Funcionamiento: La funcion Uart1_Config realiza: 
 *                              -Crea un semaforo para proteger el intercambio de datos con el periferico ya que es un recurso compartido entre diferentes tareas. 
 *                              -Configura el periferico uart como interrupcion cuando llegan datos al buffer del periferico uart. La interrupcion llama a la funcion serial_callback()
 *                           La funcion serial_callback() se llama cuando llegan datos al periferico. Esta funcion tomas los datos y lo almacena en una cola para luego ser leido por la funcion Uart1_Recibir()
 *                           La funcion Uart1_Recibir() obtiene el dato desde la cola y lo envia por medio del puntero al hilo que lo solicito
 *                           Las funciones Uart1_print() o Uart1_println()  solo realizan el envio de datos
 *
 *                 Para recibir un dato, se debe llemar a la funcion Uart1_Recibir(). Se le debe pasar el puntero de donde se quiere almacenar el dato.
 *                 Para enviar mensaje se puede usar Uart1_print() o Uart1_println()
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
#include "Uart1.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

/*=================[Definiciones]======================*/
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)                         //Cambiar esto a cualquier otro periférico UART si lo desea
#define MSG_SIZE 32

/*=====================[Variables]================================*/

K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);                                    //Cola para almacenar hasta 10 mensajes (alineados al límite de 4 bytes)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE); //Variable que contiene el periferico uart que se desea usar


static char rx_buf[MSG_SIZE];                                                 //búfer de recepción utilizado en la devolución de llamada UART ISR
static int rx_buf_pos;                                                        //búfer de recepción utilizado en la devolución de llamada UART ISR

struct k_sem Semaforo_uart;                                                   //Semaforo para proteger el periferico uart ya que es un recurso compartico con otros hilos o tareas

/*=====================[Implementaciones]==============================*/


 /*========================================================================
  Funcion: serial_callback
  Descripcion: Funcion que se llama cuando se recibe un dato. Lee caracteres de UART hasta que se detecte el final de la línea. Luego envíe los datos a la cola de mensajes.
  Parametro de entrada:
                         device *dev: dispositivo uart desde el que se llamo al callback
  No retorna nada
  ========================================================================*/
void serial_callback(const struct device *dev, void *user_data)
{
	uint8_t c;                                                    //Variable para guardar caracter leido

	if (!uart_irq_update(uart_dev)) {                             //Si esta habilitada la funcion uart irq (Esta función debe llamarse lo primero en el ISR. Llamar a uart_irq_rx_ready() , uart_irq_tx_ready() , uart_irq_tx_complete() permitido solo después de esto.)
		return;
	}

	while (uart_irq_rx_ready(uart_dev)) {                         //Si hay dato para leer

		uart_fifo_read(uart_dev, &c, 1);                            //Lee de a 1 caracter desde el buffer del driver uart

		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {           //Si llego un salto de line o retorno de carro y hay dato en el buffer
			rx_buf[rx_buf_pos] = '\0';                                //Termino el string
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);               //Coloco mensaje en la cola. Si la cola está llena, el mensaje se descarta silenciosamente
			rx_buf_pos = 0;                                           //Restablecer el búfer (fue copiado al msgq)
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {             //Si no llego el salto de linea o el retorno de carro, sigo sumando cada caracter al arreglo   
			rx_buf[rx_buf_pos++] = c;
		}
		  //los caracteres más allá del tamaño del búfer se eliminan
	}
}

/*========================================================================
  Funcion: Uart1_FreeRTOS_Config
  Descripcion: Crea el semaforo para proger el recurso compartido e inicializa la recepcion de uart por interrupcion
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void Uart1_Config (void)
{

  if (!device_is_ready(uart_dev)) {                                              //Si no se pudo iniciar el periferico uart indicado en uart_dev
		printk("UART device not found!");                                            //Envio mensaje por uart
		return;
	}

	uart_irq_callback_user_data_set(uart_dev, serial_callback, NULL);              //Configurar interrupción y devolución de llamada para recibir datos

	uart_irq_rx_enable(uart_dev);                                                  //Habilito interrupcion por recepcion mensaje en uart
  
  int resultado = k_sem_init(&Semaforo_uart, 0, K_SEM_MAX_LIMIT);                //Creo e inicializo semaforo para proteger el recurso compartido de UART con otras tareas
   
   if( resultado != 0)                                                           //Si no se creo el semaforo
    {
        Uart1_println("No se pudo crear el semaforo para proteger uart");
    }else{
        k_sem_give(&Semaforo_uart);                                              //Arranco con semaforo liberado
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
  k_sem_take(&Semaforo_uart, K_FOREVER);                               //Tomo semaforo para proteger el envio por uart ya que es un recurso compartico con otras tareas
  int msg_len = strlen(mensaje);                                       //Tamaño del buffer o arreglo
	for (int i = 0; i < msg_len; i++) {                                  //Envio bytes por uart
		uart_poll_out(uart_dev, mensaje[i]);
	}
  k_sem_give(&Semaforo_uart);                                          //Libero semaforo
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
  k_sem_take(&Semaforo_uart, K_FOREVER);                               //Tomo semaforo para proteger el envio por uart ya que es un recurso compartico con otras tareas
  int msg_len = strlen(mensaje);                                       //Tamaño del buffer o arreglo
	for (int i = 0; i < msg_len; i++) {                                  //Envio bytes por uart
		uart_poll_out(uart_dev, mensaje[i]);
	}
  uart_poll_out(uart_dev, '\n');                                       //Envio salto de linea
  uart_poll_out(uart_dev, '\r');                                       //Envio retorno de carro
  k_sem_give(&Semaforo_uart);                                          //Libero semaforo
}

/*========================================================================
  Funcion: Uart1_Recibir
  Descripcion: Recibe x cantidad de bytes del periferico uart y lo almacena
  Parametro de entrada: 
                          char *dato:        Puntero hacia la variable donde se quiere guardar el dato obtenido
 Retorna                  1:                 Si se pudo obtener el dato
 *                        2:                 Si no se pudo obtener el dato (timeout)
  ========================================================================*/
uint8_t Uart1_Recibir(char *dato){
    if(k_msgq_get(&uart_msgq, dato, K_NO_WAIT) == 0) {     //Recibo dato de la cola, si retrona 0, se recibio el mensaje, de caso contrario ocurrio un error o timeout
      return 1;
    }else{
      return 0;
    }
        
}
