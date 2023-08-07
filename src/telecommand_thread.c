#include "telecommand_thread.h"

#include "network_parameters.h"

#define RETURN_ARGUMENTS_MAX_SIZE 1024

//******************************************************************************
// Private methods
//******************************************************************************
static void TelecommandThread_sendResponseAck(TelecommandThread *self) {
  MessageHeader_t header = MessageHandler_getHeader(&self->receivedMessage);
  MessageHandler_setHeader(&self->responseMessage, header);
  MessageHandler_send(&self->responseMessage, NULL, 0);
}

static void TelecommandThread_sendResponseNominal(TelecommandThread *self,
                                                  TelecommandInterface *telecommand) {
  MessageHeader_t header = MessageHandler_getHeader(&self->receivedMessage);
  MessageHandler_setHeader(&self->responseMessage, header);  // Response message shall have same header as received message
  uint8_t buffer[RETURN_ARGUMENTS_MAX_SIZE];
  size_t currentSize = 0;
  TelecommandInterface_getReturnArguments(telecommand, buffer, &currentSize);  // Get return arguments
  MessageHandler_send(&self->responseMessage, buffer, currentSize);  // Send response message with telecommand return arguments
}

static void TelecommandThread_sendResponseFailure(TelecommandThread *self) { /* TODO: Implement */ }
static void TelecommandThread_sendResponseNotEnabled(TelecommandThread *self) { /* TODO: Implement */ }

//******************************************************************************
// Public methods
//******************************************************************************
/*========================================================================
  Funcion: TelecommandThread_create
  Descripcion: Agrega la informacion necesaria a la estructura que contine la informacion del hilo telecomando
  Parametro de entrada:
                        SetMode *self:            Puntero a la estructura donde se guardara la informacion del hilo de telecomandos
                        TelecommandsManager *telecommandsManager: Puntero al manejador de telecomandos
                        CommunicationInterface *messageChannel:   Puntero a la interface de telecomando
  No retorna nada
  ========================================================================*/
void TelecommandThread_create(TelecommandThread *self,
                              TelecommandsManager *telecommandsManager,
                              CommunicationInterface *messageChannel) {
  self->telecommandsManager = telecommandsManager;            

  MessageHandler_create(&self->receivedMessage, messageChannel);   //Guarda en la estructura del hilo de telecomando el puntero de la interface de telecomando para la recepcion de mensajes
  MessageHandler_create(&self->responseMessage, messageChannel);   //Guarda en la estructura del hilo de telecomando el puntero de la interface de telecomando para la transmicion de mensajes
  MessageHandler_initializeHeader(&self->responseMessage,          //Inicializa el header de la capa de aplicacion de  los mensajes
                                  MESSAGE_HEADER_INTERACTION_TYPE_REQUEST,
                                  2, MESSAGE_HEADER_SERVICE_TELECOMMAND, false);
}

/*========================================================================
  Funcion: TelecommandThread_start
  Descripcion: Hilo o tarea periodica para procesar los TC
  Parametro de entrada:
  No retorna nada
  ========================================================================*/
void TelecommandThread_start(void *self, void *, void *) {
  TelecommandThread *_self = (TelecommandThread *)self;                        //Puntero a la interface del hilo de telecomando
  while (true) {
    bool isMessageReceived = MessageHandler_receive(&_self->receivedMessage);  //Si se recibio mensaje
    if (isMessageReceived) {                                                   //Si se recibio mensaje
      Message_t *message = MessageHandler_getMessage(&_self->receivedMessage); //Obtengo el mensaje que se guardo en la interface del hilo
      bool isMessageTelecommand = MessageHandler_isTelecommand(&_self->receivedMessage); //Verifica si el mensaje llegado es TC
      bool isKnownOperation = TelecommandsManager_isKnownOperation(            //Verifica si el TC recibio es un telecomadno existente en el manejador de TC
        _self->telecommandsManager, message);
      if (isMessageTelecommand & isKnownOperation) {                           //Si es un TC y existe
        TelecommandInterface *telecommand = TelecommandsManager_getTelecommandFromMessage( //Se obtiene el TC que se debe ejecutar desde el mensaje
          _self->telecommandsManager, message);
        if (TelecommandInterface_isEnabled(telecommand)) {                     //Si el telecomando esta habilitado
          TelecommandInterface_loadInputArguments(telecommand, message->body,  //Se pasa el cuerpo del mensaje a los argumentos de entrada del TC
                                                  message->header.bodyLength);
          switch (message->header.interactionType) {                           //Segun el tipo de mensaje
            case MESSAGE_HEADER_INTERACTION_TYPE_SEND: {
              printk("New TC OK\n");
              TelecommandInterface_execute(telecommand);                       //Se ejecuta el comando llamando a la funcion correspondiente
              break;
            }
            case MESSAGE_HEADER_INTERACTION_TYPE_SUBMIT: {
              TelecommandInterface_execute(telecommand);
              if (true) {
                TelecommandThread_sendResponseAck(_self);
              } else {
                TelecommandThread_sendResponseFailure(_self);
              }
              break;
            }
            case MESSAGE_HEADER_INTERACTION_TYPE_REQUEST: {
              TelecommandInterface_execute(telecommand);
              if (true) {
                TelecommandThread_sendResponseNominal(_self, telecommand);
              } else {
                TelecommandThread_sendResponseFailure(_self);
              }
              break;
            }
            default: { }
          }
        } else {
          TelecommandThread_sendResponseNotEnabled(_self);
        }
      }else{
        if(isKnownOperation==false){printk("El TC recibido no es una operacion\n");}
        if(isMessageTelecommand==false){printk("No es un TC\n");}
        
      }
    }
    k_msleep(100);
  }
}
