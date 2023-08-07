/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 22/04/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Realiza los movimientos correspondientes segun el estado de las maquinas de estado
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
#include "TITO_Mov.h"

#include <string.h>
#include <stdio.h>

#include "CANopen.h"
#include "Uart1.h"
#include "epos4.h"
#include "MPU6050.h"
#include "height_controller.h"
#include "velocity_controller.h"
#include "driver_emulator.h"

/*=====================[Variables]================================*/
struct k_thread VelocityControl_thread_data;
K_THREAD_STACK_DEFINE(VelocityControl_stack_area, 512);
uint8_t epos_id[6]={0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
uint8_t vector_pos[]={0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static TITO_Mov_Mode_t Estado_movimiento = OFF;
static TITO_Mov_ProfileMode_t Estado_perfil = NOTHING;
static HeightController heightController;
static VelocityController velocityController;

/*===================[Prototipos de funciones]=========================*/
void VelocityControl_thread(void *, void *, void *);
uint8_t Motor_epos_config_parametros(void);
uint8_t Motor_config_parametros(void);
uint8_t Motor_poweroff(void);

/*=====================[Implementaciones]==============================*/
 

/*========================================================================
  Funcion: TITO_Mov_Tasks
  Descripcion: Tarea para verificar el estado de la maquina de estado y realizar los controles de los motores
  Sin parametro de entrada
  No retorna nada
  ========================================================================*/
void TITO_Mov_Tasks ( void *, void *, void * )
{
    CANopen_init();
    MPU6050_init();
    //char destino4[8]="        ";
    //sprintf(destino4, "MPU: %d", ret);
    //Uart1_println(destino4);

    // Movement controllers initialization
    HeightController_new(&heightController);
    VelocityController_new(&velocityController);

    //Paso driver epos a modo "Power disabled" para configurar cada driver
    if(Motor_poweroff()==0){
      Uart1_print("Error al apagar drivers");
    }

    if(Motor_config_parametros()==0){
      Uart1_print("Error al configurar parametros del motor");
    }

    if(Motor_epos_config_parametros()==0){
      Uart1_print("Error al configurar parametros generales en epos");
    }

    // Driver emulator initialization
    static DriverEmulator leg1;
    static DriverEmulator leg2;
    static DriverEmulator leg3;
    static DriverEmulator leg4;
    static DriverEmulator leg5;
    static DriverEmulator leg6;
    DriverEmulator_create(&leg1, epos_id[0]);
    DriverEmulator_create(&leg2, epos_id[1]);
    DriverEmulator_create(&leg3, epos_id[2]);
    DriverEmulator_create(&leg4, epos_id[3]);
    DriverEmulator_create(&leg5, epos_id[4]);
    DriverEmulator_create(&leg6, epos_id[5]);

    // Driver interface initialization
    DriverInterface *leg1Interface = DriverEmulator_viewAsDriverInterface(&leg1);
    DriverInterface *leg2Interface = DriverEmulator_viewAsDriverInterface(&leg2);
    DriverInterface *leg3Interface = DriverEmulator_viewAsDriverInterface(&leg3);
    DriverInterface *leg4Interface = DriverEmulator_viewAsDriverInterface(&leg4);
    DriverInterface *leg5Interface = DriverEmulator_viewAsDriverInterface(&leg5);
    DriverInterface *leg6Interface = DriverEmulator_viewAsDriverInterface(&leg6);

     /*
    uint8_t p[5]={0x00, 0x0F, 0x42, 0x40};          
    uint8_t i[5]={0x00, 0x00, 0x00, 0x00};
    uint8_t d[5]={0x00, 0x00, 0x00, 0x00};  
    // 1=control velocidad  2= control posicion    
    uint8_t ress = Epos4_config_PID (epos_id[0], 2, p, i, d);
    ress = ress +  Epos4_config_PID (epos_id[1], 2, p, i, d);
    ress = ress +  Epos4_config_PID (epos_id[2], 2, p, i, d);
    ress = ress +  Epos4_config_PID (epos_id[3], 2, p, i, d);
    p[0]=0x00; p[1]=0x16; p[2]=0xE3; p[3]=0x60;
    ress = ress +  Epos4_config_PID (epos_id[4], 2, p, i, d);
    p[0]=0x00; p[1]=0x0F; p[2]=0x42; p[3]=0x40;
    ress = ress +  Epos4_config_PID (epos_id[5], 2, p, i, d);
    if(ress==0){
      Uart1_println("Config PID - OK");
    }else{
      Uart1_println("Config PID - Fail");
    }
    */
    while (1)
    {
        switch (Estado_movimiento) {
        case OFF:
          break;
        case MANUAL: 
          uint8_t res = 0;
          float angleDegrees = 0;
          switch (Estado_perfil) {
            case NOTHING:
              break;
            case HOME:
              //Indico que todos los motores vayan a la posicion cero
              res += DriverInterface_setPosition(leg1Interface, 0);
              res += DriverInterface_setPosition(leg2Interface, 0);
              res += DriverInterface_setPosition(leg3Interface, 0);
              res += DriverInterface_setPosition(leg4Interface, 0);
              res += DriverInterface_setPosition(leg5Interface, 0);
              res += DriverInterface_setPosition(leg6Interface, 0);
              if(res==0){
                Uart1_println("Motores en HOME - OK");
              }else{
                Uart1_println("Motores en HOME - OFF");
              }
              break;
            case SET_HEIGHT:
              angleDegrees = HeightController_compute(&heightController);
              res += DriverInterface_setPosition(leg1Interface, angleDegrees);
              res += DriverInterface_setPosition(leg2Interface, angleDegrees);
              res += DriverInterface_setPosition(leg3Interface, angleDegrees);
              res += DriverInterface_setPosition(leg4Interface, angleDegrees);
              res += DriverInterface_setPosition(leg5Interface, angleDegrees);
              res += DriverInterface_setPosition(leg6Interface, angleDegrees);
              if (res == 0) {
                Uart1_println("Perfil - OK");
              } else {
                Uart1_println("Perfil - fail");
              }
              break;
            case FORWARD:
              angleDegrees = VelocityController_compute(&velocityController);
              res += DriverInterface_setPosition(leg1Interface, angleDegrees);
              res += DriverInterface_setPosition(leg2Interface, angleDegrees);
              res += DriverInterface_setPosition(leg3Interface, angleDegrees);
              res += DriverInterface_setPosition(leg4Interface, angleDegrees);
              res += DriverInterface_setPosition(leg5Interface, angleDegrees);
              res += DriverInterface_setPosition(leg6Interface, angleDegrees);
              if (res == 0) {
                Uart1_println("Forward - OK");
              } else {
                Uart1_println("Forward - fail");
              }
              break;
            case TURN:
              break;
          }
          break;
        case AUTO:
          break;
        }
      k_msleep(100);
    }
}


/*========================================================================
  Funcion: VelocityControl_thread
  Descripcion: Envio la trama 2 por canbus
  Sin parametro de entrada
  No retorna nada
  ========================================================================
void VelocityControl_thread(void *, void *, void *){
  Uart1_println("Inicio hilo VelocityControl");
  while (1)
    {
        char dato[32]= {0};
        uint8_t retorno = Uart1_Recibir(dato);
        if (dato[0] != ' ' && retorno == 1)                    //Si el byte es distinto del caracter ' ' y se retorno 1
        {   
            char destino[15]="               ";
            sprintf(destino, "Ingreso: %s", dato);
            Uart1_println(destino);
            
            if (dato[0]  == 'X')                               //Si el dato recibido es el caracter X
            {
               break;                                          //Salgo del while true
            }

            if (dato[0]  == 'S')                               //Si el dato recibido es el caracter S
            {
              uint32_t velocidad = (dato[1]-'0')*1000+(dato[2]-'0')*100+(dato[3]-'0')*10+(dato[4]-'0');
              char destino2[21]="               ";
              sprintf(destino2, "Velocidad set en: %d", velocidad);
              Uart1_println(destino2);

              uint8_t velocity[4]={0x00, 0x00, 0x00, 0x00};
              velocity[3]=velocidad & 0xff;
              velocity[2]=(velocidad>>8) & 0xff;
              velocity[1]=(velocidad>>16) & 0xff;
              velocity[0]=(velocidad>>24) & 0xff;

              Epos4_Cyclic_Synchronous_Velocity_MODE (epos_id[0], velocity);
              Epos4_Cyclic_Synchronous_Velocity_MODE (epos_id[1], velocity);
              Epos4_Cyclic_Synchronous_Velocity_MODE (epos_id[2], velocity);
              Epos4_Cyclic_Synchronous_Velocity_MODE (epos_id[3], velocity);
              Epos4_Cyclic_Synchronous_Velocity_MODE (epos_id[4], velocity);
              Epos4_Cyclic_Synchronous_Velocity_MODE (epos_id[5], velocity);
            }

        }else{
            Uart1_println("Ingrese el setpoint de velocidad con el formato S0000 o presione x para terminar este hilo");
        }

        //Tomo dato de la imu
        float accel[4]={0};
        float gyro[4]={0};
        uint8_t resultado = 1;
        if(resultado==0){
          char imuu[80];
          sprintf(imuu, "accel %lf %lf %lf m/s/s  -  gyro  %lf %lf %lf rad/s", accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);
          Uart1_println(imuu);
        }
                
        k_msleep(1500);
    }
  Uart1_println("Fin hilo VelocityControl");  //Al no tener bucle infinito, el hilo se elimina
}
*/
/*========================================================================
  Funcion: Motor_poweroff
  Descripcion: Apaga los 4 driver EPOS
  Sin parametro de entrada
  Retorna:   0 si no se pudo realizar correctamente
             1 si se pudo realizar correctamente
  ========================================================================*/
uint8_t Motor_poweroff(void){
  uint8_t res = Epos4_Power_Disable(epos_id[0]);
  res = res + Epos4_Power_Disable(epos_id[1]);
  res = res + Epos4_Power_Disable(epos_id[2]);
  res = res + Epos4_Power_Disable(epos_id[3]);
  res = res + Epos4_Power_Disable(epos_id[4]);
  res = res + Epos4_Power_Disable(epos_id[5]);
  if(res==0){
    return 1;
  }else{
    return 0;
  }
}

/*========================================================================
  Funcion: Motor_config_parametros
  Descripcion: Configuro parametros del motor en cada epos
  Sin parametro de entrada
  Retorna:   0 si no se pudo realizar correctamente
             1 si se pudo realizar correctamente
  ========================================================================*/
uint8_t Motor_config_parametros(void){
  uint8_t max_vel[4]={0x00, 0x00, 0x2A, 0x30};      //Maxima velocidad del motor  10800 rpm
  uint8_t nom_curr[4]={0x00, 0x00, 0x06, 0x54};     //Corriente nominal del motor 1620 mA
  uint8_t max_curr[4]={0x00, 0x00, 0x0C, 0xA8};     //3240 mA (Output current limit)
  uint8_t res = Epos4_config_motor_parameters (epos_id[0], max_vel, nom_curr, max_curr);
  res = res + Epos4_config_motor_parameters (epos_id[1], max_vel, nom_curr, max_curr);
  res = res + Epos4_config_motor_parameters (epos_id[2], max_vel, nom_curr, max_curr);
  res = res + Epos4_config_motor_parameters (epos_id[3], max_vel, nom_curr, max_curr);
  res = res + Epos4_config_motor_parameters (epos_id[4], max_vel, nom_curr, max_curr);
  res = res + Epos4_config_motor_parameters (epos_id[5], max_vel, nom_curr, max_curr);
  if(res==0){
    return 1;
  }else{
    return 0;
  }
}

/*========================================================================
  Funcion: Motor_epos_config_parametros
  Descripcion: Configuro parametros generales en cada epos
  Sin parametro de entrada
  Retorna:   0 si no se pudo realizar correctamente
             1 si se pudo realizar correctamente
  ========================================================================*/
uint8_t Motor_epos_config_parametros(void){
  uint8_t max_vel[4]={0x00, 0x00, 0x03, 0xE8};          //Máxima velocidad permitida en los movimientos. 1000 rpm 
  uint8_t des_ramp[4]={0x00, 0x00, 0x00, 0x0A};         //Desaceleracion en modo normal. 10 rpm/s
  uint8_t des_ramp_fast[4]={0x00, 0x00, 0x00, 0x14};    //Desaceleracion en modo fast. 20 rpm/s
  uint8_t max_acel[4]={0x00, 0x00, 0x00, 0x0A};         //Maxima aceleracion.  10 rpm/s 
  uint8_t res = Epos4_config_parameters (epos_id[0], max_vel, des_ramp, des_ramp_fast, max_acel);
  res = res + Epos4_config_parameters (epos_id[1], max_vel, des_ramp, des_ramp_fast, max_acel);
  res = res + Epos4_config_parameters (epos_id[2], max_vel, des_ramp, des_ramp_fast, max_acel);
  res = res + Epos4_config_parameters (epos_id[3], max_vel, des_ramp, des_ramp_fast, max_acel);
  res = res + Epos4_config_parameters (epos_id[4], max_vel, des_ramp, des_ramp_fast, max_acel);
  res = res + Epos4_config_parameters (epos_id[5], max_vel, des_ramp, des_ramp_fast, max_acel);
  if(res==0){
    return 1;
  }else{
    return 0;
  }
}

uint8_t TITO_Mov_setMode(TITO_Mov_Mode_t mode) {
  Estado_movimiento = mode;
  return 0;
}

uint8_t TITO_Mov_setProfileMode(TITO_Mov_ProfileMode_t profileMode) {
  Estado_perfil = profileMode;
  return 0;
}

uint8_t TITO_Mov_setTargetVelocity(float targetVelocity) {
  VelocityController_setNewTargetVelocity(&velocityController, targetVelocity);
  return 0;
}
