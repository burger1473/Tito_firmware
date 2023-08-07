#include "message_handler.h"

#include <stdlib.h>

#define MAXIMUM_BUFFER_SIZE 512

//******************************************************************************
// Private functions
//******************************************************************************

/*========================================================================
  Funcion: crc16
  Descripcion: Genera el codigo CRC16 utilizando polinomio normal 0x1021
  Parametro de entrada:  const uint8_t *buffer          = Buffer al cual se quiere generar el CRC
                         size_t size        = Tamaño del buffer
  Retortna:
			crc: Valor de CRC obtenido.
  ========================================================================*/
static uint16_t _crc16(const uint8_t *buffer, size_t size) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < size; ++i) {
    crc ^= (uint16_t)buffer[i] << 8;
    for (size_t j = 0; j < 8; ++j) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ 0x1021;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

//******************************************************************************
// Private methods
//******************************************************************************
static void MessageHandler_parseHeader(MessageHandler *self, const char *buffer) {
  size_t memoryOffset = 0;
  memcpy(&self->message.header.timestamp, buffer + memoryOffset,
         sizeof(self->message.header.timestamp));
  memoryOffset += sizeof(self->message.header.timestamp);
  memcpy(&self->message.header.interactionType, buffer + memoryOffset,
         sizeof(self->message.header.interactionType));
  memoryOffset += sizeof(self->message.header.interactionType);
  memcpy(&self->message.header.interactionStage, buffer + memoryOffset,
         sizeof(self->message.header.interactionStage));
  memoryOffset += sizeof(self->message.header.interactionStage);
  memcpy(&self->message.header.transactionId, buffer + memoryOffset,
         sizeof(self->message.header.transactionId));
  memoryOffset += sizeof(self->message.header.transactionId);
  memcpy(&self->message.header.service, buffer + memoryOffset,
         sizeof(self->message.header.service));
  memoryOffset += sizeof(self->message.header.service);
  memcpy(&self->message.header.operation, buffer + memoryOffset,
         sizeof(self->message.header.operation));
  memoryOffset += sizeof(self->message.header.operation);
  memcpy(&self->message.header.areaVersion, buffer + memoryOffset,
         sizeof(self->message.header.areaVersion));
  memoryOffset += sizeof(self->message.header.areaVersion);
  memcpy(&self->message.header.isErrorMessage, buffer + memoryOffset,
         sizeof(self->message.header.isErrorMessage));
  memoryOffset += sizeof(self->message.header.isErrorMessage);
  memcpy(&self->message.header.bodyLength, buffer + memoryOffset,
         sizeof(self->message.header.bodyLength));
  memoryOffset += sizeof(self->message.header.bodyLength);
}

static void MessageHandler_parseBody(MessageHandler *self, const char *buffer,
                                     size_t bufferLength) {
  size_t bodySize = bufferLength - sizeof(MessageHeader_t) - MESSAGE_HANDLER_CRC_SIZE;
  self->message.body = (char *)malloc(bodySize);
  memcpy(self->message.body, buffer + sizeof(MessageHeader_t), bodySize);
}

static void MessageHandler_parseCrc(MessageHandler *self, const char *buffer,
                                    size_t bufferLength) {
  memcpy(self->message.crc, buffer + bufferLength - MESSAGE_HANDLER_CRC_SIZE,
         MESSAGE_HANDLER_CRC_SIZE);
}

/*========================================================================
  Funcion: MessageHandler_parse
  Descripcion: Parsea el buffer y almacena los datos en los campos correspondiente del objeto
  Parametro de entrada:  MessageHandler *self           = Puntero a la estructura handle donde se va a guardar el mensaje recibido
                         const char *buffer             = Puntero al buffer que se desea parsear
                         size_t bufferLength            = Tamaño del buffer
  Retorna:  
            False: Si no se pudo realizar 
            True:  Si se pudo realizar o si fallo la comprobacion de CRC
  ========================================================================*/
static bool MessageHandler_parse(MessageHandler *self, const char *buffer,
                                 size_t bufferLength) {
  size_t minMessageLength = sizeof(MessageHeader_t) + MESSAGE_HANDLER_CRC_SIZE;     //Tamaño minimo del mensaje
  if (bufferLength < minMessageLength) { return false; }                            //Si el mensaje posee menos bytes, se retorna falso
  MessageHandler_parseHeader(self, buffer);                                         //Se parsea el header
  size_t expectedMessageLength = sizeof(MessageHeader_t) + self->message.header.bodyLength + MESSAGE_HANDLER_CRC_SIZE; //Obtengo el tamaño que deberia tener el mensaje (tamaño header+tamaño que se indica que es el mensaje+ tamaño crc)
  if (bufferLength != expectedMessageLength) { return false; }                      //Si el tamaño del mensaje es diferente del que se espera, se devuelve false
  MessageHandler_parseBody(self, buffer, bufferLength);
  MessageHandler_parseCrc(self, buffer, bufferLength);
  /*
  if((message->crc[1]==(crc2 >> 8))  && (message->crc[0]==(crc2 & 0xFF))){
	  return true;
  }else{
	  return false;
  }
  */
  return true;
}

//******************************************************************************
// Public methods
//******************************************************************************
void MessageHandler_create(MessageHandler *self,
                           CommunicationInterface *communicationChannel) {
  self->communicationChannel = communicationChannel;
}

/*========================================================================
  Funcion: MessageHandler_initializeHeader
  Descripcion: Inicializa el header de la capa de aplicacion de  los mensajes
  Parametro de entrada:
                        SetMode *self:            Puntero a la interface de telecomando
                        uint8_t interactionType        = Indica el tipo de interaccion 1 para ENVIAR, 2 para ENVIAR, 3 para SOLICITAR y 6 para PUBSUB (según CCSDS 521.0-B-2, Sección 4.4.1)
                        uint8_t interactionStage       = Etapa de interacción: 1 o 2 según el orden del mensaje
                        MessageHeaderService_t service = 0 para telemetrías (TM) o 1 para telecomandos (TC)
                        bool isErrorMessage            = Indica si es un mensaje de error: Valor booleano para indicar si es un mensaje de error (0x1 para verdadero, 0x0 para falso)
  No retorna nada
  ===========================================*/
void MessageHandler_initializeHeader(MessageHandler *self,
                                     uint8_t interactionType,
                                     uint8_t interactionStage,
                                     MessageHeaderService_t service,
                                     bool isErrorMessage) {
  self->message.header.interactionType = interactionType;
  self->message.header.interactionStage = interactionStage;
  self->message.header.transactionId = 0;
  self->message.header.service = service;
  self->message.header.areaVersion = MESSAGE_HANDLER_AREA_VERSION;
  self->message.header.isErrorMessage = isErrorMessage;
}

void MessageHandler_send(MessageHandler *self, uint8_t *messageBody,
                         uint16_t messageBodySize) {
  self->message.header.timestamp = (uint64_t)k_uptime_get();
  self->message.header.bodyLength = messageBodySize;
  size_t messageFullSize = sizeof(MessageHeader_t) + messageBodySize + MESSAGE_HANDLER_CRC_SIZE;
  char buffer[messageFullSize];
  memcpy(buffer, &self->message.header, sizeof(MessageHeader_t));
  memcpy(buffer + sizeof(MessageHeader_t), messageBody, messageBodySize);
  uint16_t crc16 = _crc16(buffer, sizeof(MessageHeader_t)+messageBodySize);
  char crc[MESSAGE_HANDLER_CRC_SIZE] = {crc16 & 0xFF, crc16>>8};
  memcpy(buffer + sizeof(MessageHeader_t) + messageBodySize, crc, MESSAGE_HANDLER_CRC_SIZE);
  self->message.header.transactionId++;
  CommunicationInterface_send(self->communicationChannel, buffer, messageFullSize);
}

/*========================================================================
  Funcion: MessageHandler_receive
  Descripcion: Verifica si se recibio mensaje
  Parametro de entrada:
                        MessageHandler *self: puntero a la estructura handle donde se va a guardar el mensaje recibido
  No retorna nada
  ========================================================================*/
bool  MessageHandler_receive(MessageHandler *self) {
  uint8_t buffer[MAXIMUM_BUFFER_SIZE];                                          //Buffer para almacenr mensaje a recibir
  size_t receivedSize;                                                          //Variable para almacenar el tamaño del mensaje recibido
  uint8_t receiveStatus = CommunicationInterface_receive(                       //Se obtiene mensaje retornando valor de UdpBridge_receive. Retorna 0 si se recibo mensaje o 1 si no se recibio
    self->communicationChannel, buffer, MAXIMUM_BUFFER_SIZE, &receivedSize);
  if (receiveStatus == 0) {
    return MessageHandler_parse(self, buffer, receivedSize);                    //Se parsea el mensaje en los campos correspondientes y se verifica CRC
  }
  return false;
}

void MessageHandler_setHeader(MessageHandler *self, MessageHeader_t header) {
  self->message.header = header;
}

MessageHeader_t MessageHandler_getHeader(MessageHandler *self) {
  return self->message.header;
}

void MessageHandler_setOperation(MessageHandler *self, uint16_t operation) {
  self->message.header.operation = operation;
}

/*========================================================================
  Funcion: MessageHandler_getMessage
  Descripcion: Retorna el mensaje obtenido
  Parametro de entrada:
                        MessageHandler *self: puntero a la estructura handle donde se va a guardar el mensaje recibido
  No retorna nada
  ========================================================================*/
Message_t *MessageHandler_getMessage(MessageHandler *self) {
  return &self->message;
}

/*========================================================================
  Funcion: MessageHandler_getMessage
  Descripcion: Determina si el mensaje recibido es un telecomando verificando el tipo de mensaje del header recibido
  Parametro de entrada:
                        MessageHandler *self: puntero a la estructura handle donde se guardo el mensaje recibido
  Returna:
            True: si es TC
            False: si el mensaje no es TC
  ========================================================================*/
bool MessageHandler_isTelecommand(MessageHandler *self) {
  return self->message.header.service == MESSAGE_HEADER_SERVICE_TELECOMMAND;
}
