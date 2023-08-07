/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 12/01/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
#include <zephyr/kernel.h>

#include "Uart1.h"                  //Libreria del periferico UART
#include "TMTC_I2C.h"               //Libreria del periferico I2C
#include "TITO_Mov.h"               //Libreria para mover a tito
#include "udp_bridge.h"             //Libreria del bridge UDP por I2C
#include "telemetry_manager.h"      //Libreria para el manejo de TM
#include "telecommands_manager.h"   //Libreria para el manejo de TC
#include "telemetry_thread.h"       //Hilo para procesar TM
#include "telecommand_thread.h"     //Hilo para procesar TC
#include "get_timestamp.h"          //Libreria del telecomando Tiimestamp
#include "set_mode.h"               //Libreria del telecomando setmode
#include "set_profile_mode.h"       //Libreria del telecomando Perfilmode
#include "set_target_velocity.h"    //Libreria del telecomando set velocity
#include "TM_Enable.h"              //Libreria del telecomando de manejo de TM
#include "TM_Acelerometro.h"        //Libreria de para el envio de la telemetria acelerometro

/*=====================[ Defines ]============================*/
#define STACK_SIZE 16384  // Size of stack area used by every thread
#define COMMUNICATION_THREADS_PRIORITY 7

/*=====================[ Threads ]============================*/
//Se define el stack y el puntero de las tareas o hilos
K_THREAD_STACK_DEFINE(telecommand_thread_stack_area, STACK_SIZE);
struct k_thread telecommand_thread;

K_THREAD_STACK_DEFINE(telemetry_thread_stack_area, STACK_SIZE);
struct k_thread telemetry_thread;

K_THREAD_STACK_DEFINE(TITO_Mov_stack_area, STACK_SIZE);
struct k_thread TITO_Mov_thread_data;

/*========================================================================
  Funcion: main
  Descripcion: Crea los hilos de operacion
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void main(void)
{
  //Se configura el puerto uart
	Uart1_Config();
	Uart1_println("Iniciando Rovertito V1.0.0 ");

  static UdpBridge udpBridgeTelecommand;  //Estructura que contiene la informacion del puerto y el puntero a su interface
  static UdpBridge udpBridgeTelemetry;    //Estructura que contiene la informacion del puerto y el puntero a su interface
  static TelecommandsManager telecommandsManager; //Estructura que contiene el numero maximo de telecomandos y el numero actual
  static TelemetryManager telemetryManager; //Estructura que contiene el numero maximo de telemetrias y el numero actual

  //Estructuras para cada Telecomando. Cada Telecomando es una interface diferente
  //Cada estructura contiene un puntero a su propia interface y dos estrucutras correspondientes a las variables de entrada y salida de la interface
  static GetTimestamp TC_getTimestamp;
  static SetMode TC_setMode;
  static SetProfileMode TC_setProfileMode;
  static SetTargetVelocity TC_setTargetVelocity;
  static TMEnable TC_TMEnable;

  //Estructuras para cada Telemetria. Cada Telemetria es una interface diferente
  //Cada estructura contiene un puntero a su propia interface y dos estrucutras correspondientes a las variables de entrada y salida de la interface
  static TM_Acelerometro TMp_Acelerometro;

  //Estructuras que contienen la informacion de los hilos
  static TelecommandThread telecommandThread;
  static TelemetryThread telemetryThread;

  //Se inicia y configura la comunicacion UDP mediante el bridge
  TMTC_I2C_init();                                                    //Se inicia el periferico i2c
  UdpBridge_create(&udpBridgeTelecommand, LANDER_PORT_TELECOMMAND);   //Inicia la interfaces para los telecomandos y le asigna un puerto
  UdpBridge_create(&udpBridgeTelemetry, LANDER_PORT_TELEMETRY);       //Inicia la interfaces para los telemetrias y le asigna un puerto

//Configuracion de Telecomandos
  TelecommandsManager_create(&telecommandsManager);                   //Se inicializa el manejador de telecomandos con 0 telecomandos actualmente

  
  //Se crea telecomandando para cambiar el modo de operacion de tito (Maquina de estado en TITO_Mov.c)
  SetMode_create(&TC_setMode);                                           //Se crea interface para este telecomando
  TelecommandsManager_addNewTelecommand(&telecommandsManager,         //Se agrega la interface creada, al manejador de telecomando
                                        SetMode_viewAsTelecommandInterface(&TC_setMode));
  //Se crea telecomandando para cambiar la altura de tito
  SetProfileMode_create(&TC_setProfileMode);
  TelecommandsManager_addNewTelecommand(&telecommandsManager,
                                        SetProfileMode_viewAsTelecommandInterface(&TC_setProfileMode));
  //Se crea telecomandando para cambiar la velocidad de tito
  SetTargetVelocity_create(&TC_setTargetVelocity);
  TelecommandsManager_addNewTelecommand(&telecommandsManager,
                                        SetTargetVelocity_viewAsTelecommandInterface(&TC_setTargetVelocity));

  //Se crea telecomandando para manejo de timestamp
  GetTimestamp_create(&TC_getTimestamp);                                 
  TelecommandsManager_addNewTelecommand(&telecommandsManager,        
                                        GetTimestamp_viewAsTelecommandInterface(&TC_getTimestamp));

  //Se crea telecomandando para manejo de TM
  TMEnable_create(&TC_TMEnable);                                 
  TelecommandsManager_addNewTelecommand(&telecommandsManager,        
                                        TMEnable_viewAsTelecommandInterface(&TC_TMEnable));
                                        
  //Se obtiene puntero a la interface del udpbridge para los telecomandos
  CommunicationInterface *telecommandChannel = UdpBridge_viewAsCommunicationInterface(&udpBridgeTelecommand);
 
  //Se inicializa la interface de telecomando (Agrega la informacion necesaria a la estructura que contine la informacion del hilo telecomando e inicializa el header de los mensajes TC)
  TelecommandThread_create(&telecommandThread, &telecommandsManager, telecommandChannel);

//Configuracion de telemetrias
  TelemetryManager_create(&telemetryManager);                   //Se inicializa el manejador de telemetrias con 0 telemetrias actualmente
  
  //Se configura telemetria de acelerometro
  TM_Acelerometro_inicialize(&TMp_Acelerometro);                                        //Se inicializa interface para esta telemetria
  TelemetryManager_addNewTelemetry(&telemetryManager, TM_Acelerometro_viewAsTelemetryInterface(&TMp_Acelerometro)); //Se agrega la interface creada, al manejador de telemetria
  
  //Se obtiene puntero a la interface del udpbridge para la telemetria
  CommunicationInterface *telemetryChannel = UdpBridge_viewAsCommunicationInterface(&udpBridgeTelemetry);

  TelemetryThread_create(&telemetryThread, &telemetryManager, telemetryChannel);

//Se crean las hilos o tareas en zephyr
  //Tarea o hilo periodico para procesar los telecomandos
  k_thread_create(&telecommand_thread, telecommand_thread_stack_area,
                  K_THREAD_STACK_SIZEOF(telecommand_thread_stack_area),
                  TelecommandThread_start,
                  &telecommandThread, NULL, NULL,
                  COMMUNICATION_THREADS_PRIORITY, 0, K_NO_WAIT);
  //Tarea o hilo periodico para procesar los telemetria
  k_thread_create(&telemetry_thread, telemetry_thread_stack_area,
                  K_THREAD_STACK_SIZEOF(telemetry_thread_stack_area),
                  TelemetryThread_start,
                  &telemetryThread, NULL, NULL,
                  COMMUNICATION_THREADS_PRIORITY+1, 0, K_NO_WAIT);
  //Tarea o hilo periodico para procesar los movimientos de tito
  k_thread_create(&TITO_Mov_thread_data, TITO_Mov_stack_area,
                  K_THREAD_STACK_SIZEOF(TITO_Mov_stack_area),
                  TITO_Mov_Tasks,
                  NULL, NULL, NULL,
                  COMMUNICATION_THREADS_PRIORITY + 2, 0, K_NO_WAIT);
}
