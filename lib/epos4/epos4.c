/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 17/02/2023
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Libreria driver EPOS4
 *===========================================================================*/

/*=====================[ Inclusiones ]============================*/
  #include "../CANopen/CANopen.h"
  #include "../Usart1/Uart1.h"
  
/*=====================[Implementaciones]==============================*/
/*========================================================================
  Funcion: Epos4_Power_Disable()
  Descripcion:  Cambia el estado del driver epos a desactivado en estado "Ready to switch on"
                Index 0x6040, Subindex 0x00: Data 0x00 0x21
  Parametro de entrada: uint8_t EPOS4_id: Id del driver
  Retorno   true:   si se pudo realizar correctamente
            false:  si no se pudo escribir
  ========================================================================*/
bool Epos4_Power_Disable(uint8_t EPOS4_id){
  uint8_t  Controlword[2]={0};   Controlword[0]=0x00;  Controlword[1]=0x21; //state= Ready to switch on  /power disable
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x2B, 0x6040, 0x00, Controlword, CANopen_SDO_mode_client, 2);
   if(res==0){
    return true;
  }else{
    return false;
  }
}

/*========================================================================
  Funcion: Epos4_config_motor_parameters ()
  Descripcion:  Configura las caracteristicas del motor
  Parametro de entrada: uint8_t EPOS4_id:     Id del driver
                        uint8_t *max_vel      (4 bytes) Velocidad máxima permitida configurada para el motor
                        uint8_t *in_max_vel:  (4 bytes) Velocidad de entrada máxima permitida
                        uint8_t *nom_curr:    (4 bytes) Corriente nominal del motor [mA]
                        uint8_t *max_curr:    (4 bytes) Corriente máxima admisible del motor [mA]. Recomendamos configurar el valor al doble de la corriente nominal.
                        uint8_t *num_pole:    (1 bytes) Número de pares de polos magnéticos (número de polos dividido por 2)
  Retorno   true:   si se pudo realizar correctamente
            false:  si no se pudo escribir
  ========================================================================*/
bool Epos4_config_motor_parameters (uint8_t EPOS4_id, uint8_t *max_vel, uint8_t *nom_curr, uint8_t *max_curr){
  //Indica la velocidad máxima permitida configurada para el motor. Sirve como protección del motor. El valor se da en [rpm]
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x6080, 0x00, max_vel, CANopen_SDO_mode_client, 4);
  //Representa la corriente nominal del motor [mA].
  res = res+CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x3001, 0x01, nom_curr, CANopen_SDO_mode_client, 4);
  //Representa la corriente máxima admisible del motor [mA]. Recomendamos configurar el valor al doble de la corriente nominal.
  res = res+CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x3001, 0x02, max_curr, CANopen_SDO_mode_client, 4);
  if(res==0){
    return true;
  }else{
    return false;
  }
}

/*========================================================================
  Funcion: Epos4_config_parameters ()
  Descripcion:  Configuro parametros del epos y parametros generales del control
  Parametro de entrada: uint8_t EPOS4_id:   Id del driver
                        uint8_t *max_vel:        (4 bytes) Límite de velocidad en un movimiento PPM o PVM.
                        uint8_t *des_ramp:       (4 bytes) Deceleración del perfil de parada
                        uint8_t *des_ramp_fast:  (4 bytes) Deceleración del perfil de parada rápida
                        uint8_t *max_acel:       (4 bytes) Aceleración máxima permitida para evitar daños mecánicos.
  Retorno   true:   si se pudo realizar correctamente
            false:  si no se pudo escribir
  ========================================================================*/
bool Epos4_config_parameters (uint8_t EPOS4_id, uint8_t *max_vel, uint8_t *des_ramp, uint8_t *des_ramp_fast, uint8_t *max_acel){
  //Usado como límite de velocidad en un movimiento PPM o PVM. El valor se da en [unidades de velocidad]
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x607F, 0x00, max_vel, CANopen_SDO_mode_client, 4);
  //Se utiliza con un mando «Quick stop» para determinar la deceleración del perfil de parada rápida. El valor se da en [unidades de aceleración]
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x6085, 0x00, des_ramp_fast, CANopen_SDO_mode_client, 4);
  //Se Define el valor de desaceleración utilizado durante un movimiento perfilado. El valor se da en [unidades de aceleración]
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x6084, 0x00, des_ramp, CANopen_SDO_mode_client, 4);
  //Se utiliza para limitar la aceleración máxima permitida para evitar daños mecánicos. Representa el límite de todos los demás objetos de aceleración/desaceleración del eje. El valor se da en [unidades de aceleración]
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x60C5, 0x00, max_acel, CANopen_SDO_mode_client, 4);
    
  if(res==0){
    return true;
  }else{
    return false;
  }
}

/*========================================================================
  Funcion: Epos4_config_PID
  Descripcion:  Configuro los parametros PID o PI del controlador indicado
  Parametro de entrada: uint8_t EPOS4_id:     Id del driver
                        uint8_t controlador:  1- Control corriente    (PI)   ->  P: [uV/A].     Default: 1'171'880   I: [uV/A ms].  Default: 3'906'250
                                              2- Control de posicion  (PID)  ->  P: [uA/rad].   Default: 1'500'000   I: [uA/rad s]. Default: 780'000      D:  [uA s/rad]. Default: 16'000
                                              3- Control de velocidad (PI)   ->  P: [uA s/rad]. Default: 20'000      I: [uA/rad].   Default: 500'000
                        uint8_t *control_P: (4 bytes) Representa la ganancia proporcional.
                        uint8_t *control_I: (4 bytes) Representa la ganancia integral.
                        uint8_t *control_D: (4 bytes) Representa la ganancia derivativa.
  Retorno   true:   si se pudo realizar correctamente
            false:  si no se pudo escribir
  ========================================================================*/
bool Epos4_config_PID (uint8_t EPOS4_id, uint8_t controlador, uint8_t *control_P, uint8_t *control_I, uint8_t *control_D){
  //Usado como límite de velocidad en un movimiento PPM o PVM. El valor se da en [unidades de velocidad]
  uint8_t res = 0;
  if(controlador==1){
    res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x30A0, 0x01, control_P, CANopen_SDO_mode_client, 4);
    res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x30A0, 0x02, control_I, CANopen_SDO_mode_client, 4);
  }

  if(controlador==2){
    res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x30A1, 0x01, control_P, CANopen_SDO_mode_client, 4);
    res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x30A1, 0x02, control_I, CANopen_SDO_mode_client, 4);
    res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x30A1, 0x03, control_D, CANopen_SDO_mode_client, 4);
  }
  
  if(controlador==3){
    res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x30A2, 0x01, control_P, CANopen_SDO_mode_client, 4);
    res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x30A2, 0x02, control_I, CANopen_SDO_mode_client, 4);
  }
  
  if(res==0){
    return true;
  }else{
    return false;
  }
}

/*========================================================================
  Funcion: Epos4_PROFILE_POSITION_MODE()
  Descripcion: Realiza movimiento de posicion, para esto, configura el modo de perfil de posicion y cambia el estado del driver a "operation enabled" en el controlword
  Parametro de entrada: uint8_t EPOS4_id:       Id del driver
                        uint8_t *position:      (4 bytes) La posición a la que se supone que debe moverse el variador
                        uint8_t *max_vel:       (4 bytes) Velocidad normalmente alcanzada al final de la rampa de aceleración durante un movimiento perfilado
                        uint8_t *ramp_acel:     (4 bytes) Rampa de aceleración durante un movimiento
                        uint8_t *ramp_desacel:  (4 bytes) Rampa de deceleración durante un movimiento
  Retorno   true:   si se pudo realizar correctamente
            false:  si no se pudo escribir
  ========================================================================*/
bool Epos4_PROFILE_POSITION_MODE (uint8_t EPOS4_id, uint8_t *position, uint8_t *max_vel, uint8_t *ramp_acel, uint8_t *ramp_desacel){
  //Configuro modo PPM
  uint8_t bytee = 0x01;
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x2F, 0x6060, 0x00, &bytee, CANopen_SDO_mode_client, 1);
  //Configuro posicion
  //La posición a la que se supone que debe moverse el variador usando los parámetros de control de movimiento, como velocidad, aceleración, tipo de perfil de movimiento, etc. Se interpretará como absoluta o relativa según el indicador de palabra de control "abs / rel".
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x607A, 0x00, position, CANopen_SDO_mode_client, 4);
  //La velocidad normalmente alcanzada al final de la rampa de aceleración durante un movimiento perfilado
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x6081, 0x00, max_vel, CANopen_SDO_mode_client, 4);
  //Define la rampa de aceleración durante un movimiento
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x6083, 0x00, ramp_acel, CANopen_SDO_mode_client, 4);
  //Define la rampa de deceleración durante un movimiento
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x6084, 0x00, ramp_desacel, CANopen_SDO_mode_client,4);
  //Configuro CONTROLWORD (PROFILE POSITION MODE-SPECIFIC BITS)
  
  uint8_t CONTROLWORD[2]={0};  CONTROLWORD[0]=0x00; CONTROLWORD[1]=0x00;
  //Controlword low byte
  CONTROLWORD[1]=CONTROLWORD[1]|(0x0F&0x07);   //Necesario los primeros 4 byte en "0111" para pasar el driver a modo "operation enabled"
  CONTROLWORD[1]=CONTROLWORD[1]|(0x10&0xFF);   //1(0xFF) Assume New setpoint   0(0x00) Does not assume New setpoint
  CONTROLWORD[1]=CONTROLWORD[1]|(0x20&0xFF);   //1(0xFF) Cancelar el posicionamiento real y comenzar el siguiente posicionamiento  
                                               //0(0x00) Finalice el posicionamiento actual, luego comience el siguiente posicionamiento.
                                               //Necesario en 1 para pasar el driver a modo "operation enabled"
  CONTROLWORD[1]=CONTROLWORD[1]|(0x40&0x00);   //Abs/rel -> 1(0xFF) La posición de destino es un valor relativo   0(0x00) La posición de destino es un valor absoluto
                                               //Necesario en 0 para pasar el driver a modo "operation enabled"
  //Controlword high byte
  CONTROLWORD[0]=CONTROLWORD[0]|(0x02&0x00);   //Halt -> 0(0x00) Ejecutar o continuar posicionando   1(0xFF) Detener eje con -> Profile deceleración
  CONTROLWORD[0]=CONTROLWORD[0]|(0x80&0x00);   //Endless movement -> 0(0x00) Modo de operacion normal  1(0xFF) El sistema realizará un movimiento sin fin
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x2B, 0x6040, 0x00, CONTROLWORD, CANopen_SDO_mode_client, 2);
  
  if(res==0){
    return true;
  }else{
    return false;
  }
}


/*========================================================================
  Funcion: Epos4_Cyclic_Synchronous_Velocity_MODE()
  Descripcion:  Genera un control por velocidad cincrona
  Parametro de entrada: uint8_t EPOS4_id: Id del driver
                        uint8_t *velocity: (4 bytes) La velocidad que se supone que debe alcanzar la unidad. por defecto en rev/min (rpm)
  Retorno   true:   si se pudo escribir correctamente
            false:  si no se pudo escribir
  ========================================================================*/
bool Epos4_Cyclic_Synchronous_Velocity_MODE (uint8_t EPOS4_id, uint8_t *velocity){
  //Configuro modo CSV
  uint8_t byte = 0x09;
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x2F, 0x6060, 0x00, &byte, CANopen_SDO_mode_client, 1);
  //Seteo velocidad
  res = res + CANopen_SDO_Expedited_Write(EPOS4_id, 0x23, 0x60FF, 0x00, velocity, CANopen_SDO_mode_client, 4);
  
  if(res==0){
    return true;
  }else{
    return false;
  }
}

/*========================================================================
  Funcion: Epos4_read_actual_position
  Descripcion: Mando por canbus un mensaje SDO de solicitud de lectura (0x40) mediante can open para obtener la posicion
               Index 0x6064, Subindex 0x00
  Parametro de entrada: uint8_t EPOS4_id:      Id del driver
                        uint8_t *resultado:    Puntero a la variable uint8 de 4 bytes donde se va a guardar la posicion actual
  Retorno   true:   si se pudo leer correctamente
            false:  si no se pudo leer
  ========================================================================*/
bool Epos4_read_actual_position(uint8_t EPOS4_id, uint8_t *resultado, uint8_t *data){
  //uint8_t data[4]={0};
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x40, 0x6064, 0x00, data, CANopen_SDO_mode_client, 4);
    
  resultado=data;
  if(res==0){
    return true;
  }else{
    return false;
  }
}

/*========================================================================
  Funcion: Epos4_read_actual_Velocity
  Descripcion: Mando por canbus un mensaje SDO de solicitud de lectura (0x40) mediante can open para obtener la velocidad actual
               Index 0x606C, Subindex 0x00
  Parametro de entrada: uint8_t EPOS4_id:      Id del driver
                        uint8_t *resultado:    Puntero a la variable uint8 de 4 bytes donde se va a guardar la velocidad actual
  Retorno   true:   si se pudo leer correctamente
            false:  si no se pudo leer
  ========================================================================*/
bool Epos4_read_actual_Velocity(uint8_t EPOS4_id, uint8_t *resultado, uint8_t *data){
  //uint8_t data[4]={0};
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x40, 0x606C, 0x00, data, CANopen_SDO_mode_client, 4);
    
  resultado=data;
  if(res==0){
    return true;
  }else{
    return false;
  }
}

/*========================================================================
  Funcion: Epos4_read_actual_ Torque
  Descripcion: Mando por canbus un mensaje SDO de solicitud de lectura (0x40) mediante can open para obtener el Torque actual
               Index 0x606C, Subindex 0x00
  Parametro de entrada: uint8_t EPOS4_id:      Id del driver
                        uint8_t *resultado:    Puntero a la variable uint8 de 4 bytes donde se va a guardar el Torque actual
  Retorno   true:   si se pudo leer correctamente
            false:  si no se pudo leer
  ========================================================================*/
bool Epos4_read_actual_Torque(uint8_t EPOS4_id, uint8_t *resultado, uint8_t *data){
  //uint8_t data[4]={0};
  uint8_t res = CANopen_SDO_Expedited_Write(EPOS4_id, 0x40, 0x6077, 0x00, data, CANopen_SDO_mode_client, 4);
    
  resultado=data;
  if(res==0){
    return true;
  }else{
    return false;
  }
}