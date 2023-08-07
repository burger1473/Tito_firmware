/*=============================================================================
 * Author: Spacebeetech - Navegacion
 * Date: 23/02/2023 
 * Board: Atmel ARM Cortex-M7 Xplained Ultra Dev Board ATSAMV71-XULT ATSAMV71Q21B
 * Entorno de programacion: Docker with Zephyr
 *
 * Descripcion: Libreria IMU MPU6050 para SAMV71
*===========================================================================*/

/*=====================[ Inclusiones ]============================*/
  #include "MPU6050.h"

/*=================[Prototipos de funciones]======================*/

/*=====================[Variables]================================*/
 const struct device *const mpu6050 = DEVICE_DT_GET_ONE(invensense_mpu6050);
 //const struct device *const mpu6050;

 float temperature=0;
 float accel[4]={0};
 float gyro[4]={0};

/*=====================[Definiciones]================================*/

/*=====================[Implementaciones]==============================*/


/*========================================================================
  Funcion: MPU6050_init()
  Descripcion: Inicializa la imu
  Sin parametro de entrada
  Rertorna: uint8_t   0 = resultado ok
                      1 = No se pudo inicializar
  ========================================================================*/
  uint8_t MPU6050_init(void){
  if (!device_is_ready(mpu6050)) {
		return 1;
	}else{
        return 0;
    }
  }

/*========================================================================
  Funcion: process_mpu6050
  Descripcion: Obtiene la aceleracion y el giro
  Parametro de entrada: device *dev: indica el dispositivo MPU
  Rertorna: uint8_t   0 = resultado ok
  ========================================================================*/
static int process_mpu6050(const struct device *dev)
{
	struct sensor_value temperature2;
	struct sensor_value accel2[3];
	struct sensor_value gyro2[3];
	int rc = sensor_sample_fetch(dev);                                             //Obtenga una muestra del sensor y guárdela en un búfer de controlador interno. Retorna 0 si tiene éxito o código errno negativo si falla.

	if (rc == 0) {      
		rc = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel2);                 //Obtenga una lectura de un dispositivo sensor. Devuelve un valor útil para un canal en particular, a partir de los datos internos del controlador. Antes de llamar a esta función, se debe obtener una muestra llamando a sensor_sample_fetch. Retorna 0 si se puedo hacer con exito
	}
	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyro2);                   //Obtenga una lectura de un dispositivo sensor. Devuelve un valor útil para un canal en particular, a partir de los datos internos del controlador. Antes de llamar a esta función, se debe obtener una muestra llamando a sensor_sample_fetch. Retorna 0 si se puedo hacer con exito
	}
	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temperature2);       //Obtenga una lectura de un dispositivo sensor. Devuelve un valor útil para un canal en particular, a partir de los datos internos del controlador. Antes de llamar a esta función, se debe obtener una muestra llamando a sensor_sample_fetch. Retorna 0 si se puedo hacer con exito
	}
	if (rc == 0) {
     temperature=sensor_value_to_double(&temperature2);
	   accel[0] = sensor_value_to_double(&accel2[0]);
	   accel[1] = sensor_value_to_double(&accel2[1]);
	   accel[2] = sensor_value_to_double(&accel2[2]);
	   gyro[0]  = sensor_value_to_double(&gyro2[0]);
	   gyro[1]  = sensor_value_to_double(&gyro2[1]);
	   gyro[2]  = sensor_value_to_double(&gyro2[2]);
	}

	return rc;
}

/*========================================================================
  Funcion: MPU6050_getdata()
  Descripcion: Obtiene la aceleracion y el giro de la imu
  Parametro de entrada: device *dev: indica el dispositivo MPU
  Rertorna: uint8_t   0 = resultado ok
  ========================================================================*/
uint8_t MPU6050_getdata(float *temp, float *accell, float *gyroo){
    
    uint8_t resultado = 1;
    if (device_is_ready(mpu6050)) {
      resultado = process_mpu6050(mpu6050);
      *temp = temperature;
      accell[0] = accel[0];
      accell[1] = accel[1];
      accell[2] = accel[2];
      gyroo[0] = gyro[0];
      gyroo[1] = gyro[1];
      gyroo[2] = gyro[2];
    }
    return resultado;
}
