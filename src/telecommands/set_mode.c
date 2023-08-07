#include "set_mode.h"
#include "Telecommands.h"
#include <string.h>

#define SET_MODE_OPERATION TC_Operation_SetMode     //Numero de operacion o ID de este TC (Esto sirve para dar un ID a cada telecomando y poder manejarlo con el protocolo de aplicacion que viene de tierra)

//******************************************************************************
// Interface implementations
//******************************************************************************

/*========================================================================
  Funcion: SetMode_isEnabled
  Descripcion: Funcion par habilitar este TC. NO se usa actualmente
  Parametro de entrada:
                        SetMode *self:            Puntero a la interface de este telecomando
  No retorna nada
  ========================================================================*/
static bool SetMode_isEnabled(void *self) {
  return true;
}

/*========================================================================
  Funcion: SetMode_loadInputArguments
  Descripcion: Se asignan los argumentos de entrada de esta interface
  Parametro de entrada:
                        SetMode *self:            Puntero a la interface de este telecomando
                        uint8_t *arguments:       Puntero de la variable que contiene los argumentos
                        size_t *argumentsSize:    Puntero de la variable que contiene el tamaño de los argumentos
  No retorna nada
  ========================================================================*/
static void SetMode_loadInputArguments(void *self, uint8_t *arguments, size_t argumentsSize) {
  SetMode *_self = (SetMode *)self;
  memcpy(&_self->inputArguments, arguments, argumentsSize);           //Se copian los argumentos desde el puntero a la interface
}

/*========================================================================
  Funcion: SetMode_execute
  Descripcion: Ejecuta el TC de set mode
  Parametro de entrada:
                        SetMode *self:            Puntero a la interface de este telecomando
  No retorna nada
  ========================================================================*/
static bool SetMode_execute(void *self) {
  SetMode *_self = (SetMode *)self;
  TITO_Mov_setMode(_self->inputArguments.mode);   //Se envia la altura que esta en el argumento de entrada de esta interface a la funcion de tito mode que esta en TITO_MOV.C
  return true;
}

/*========================================================================
  Funcion: SetMode_getReturnArguments
  Descripcion: Devuelve los argumentos de salida de esta interface
  Parametro de entrada:
                        SetMode *self:            Puntero a la interface de este telecomando
                        uint8_t *arguments:       Puntero de la variable donde se desea guardar los argumentos
                        size_t *argumentsSize:    Puntero de la variable donde se desea guardar el tamaño de los argumentos
  No retorna nada
  ========================================================================*/
static void SetMode_getReturnArguments(void *self, uint8_t *arguments, size_t *argumentsSize) {
  SetMode *_self = (SetMode *)self;
  *argumentsSize = sizeof(SetModeOutputArguments_t);              //Se obtiene tamaño
  memcpy(arguments, &_self->outputArguments, *argumentsSize);     //Se copian los argumentos desde la interface al puntero
}

/*========================================================================
  Funcion: SetMode_getOperation
  Descripcion: Retorna el numero de operacion o ID de este TC (Esto sirve para dar un ID a cada telecomando y poder manejarlo con el protocolo de aplicacion que viene de tierra)
  Parametro de entrada:
                        SetMode *self:     Puntero a la interface de este telecomando 
  Retorna el numero de operacion
  ========================================================================*/
static uint16_t SetMode_getOperation(void *self) {
  return SET_MODE_OPERATION;                                                       //Retorna el numero de operacion
}

//******************************************************************************
// Public methods
//******************************************************************************
/*========================================================================
  Funcion: SetMode_create
  Descripcion: Crea telecomando para setear el modo de tito
  Parametro de entrada:
                        SetMode *self:            Puntero a la interface de este telecomando
  No retorna nada
  ========================================================================*/
void SetMode_create(SetMode *self) {
  self->telecommandInterfaceView.instance = self;                                   //Puntero a su propia interface
  self->telecommandInterfaceView.isEnabled = SetMode_isEnabled;                     //Puntero a la funcion para habilitar este TC (no se usa actualmente)
  self->telecommandInterfaceView.loadInputArguments = SetMode_loadInputArguments;   //Puntero a la funcion para cargar los argumento de entrada de la interface (actualmente el unico argumento de entrada es el modo de operacion de tito)
  self->telecommandInterfaceView.execute = SetMode_execute;                         //Puntero a la funcion para ejecutar el TC  (Actualmente se llama a TITO_Mov_setMode en tito_Mov.c)
  self->telecommandInterfaceView.getReturnArguments = SetMode_getReturnArguments;   //Puntero a la funcion que retorna los argumentos de salida de la interface
  self->telecommandInterfaceView.getOperation = SetMode_getOperation;               //Puntero a la funcion que devuelve el numero de operacion o TC (Esto sirve para dar un ID a cada telecomando y poder manejarlo con el protocolo de aplicacion que viene de tierra)
}

TelecommandInterface *SetMode_viewAsTelecommandInterface(SetMode *self) {
  return &self->telecommandInterfaceView;
}
