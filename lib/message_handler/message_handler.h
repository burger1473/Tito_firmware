#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <zephyr/kernel.h>

#include "communication_interface.h"

#define MESSAGE_HANDLER_AREA_VERSION 1     //Version del protocolo
#define MESSAGE_HANDLER_CRC_SIZE 2         //Tamaño del CRC segun algortimo

//Estructura del encabeado o header del protocolo
typedef struct __attribute__((packed)) MessageHeader {
  uint64_t timestamp;
  uint16_t interactionType;
  uint8_t interactionStage;
  uint64_t transactionId;
  uint16_t service;
  uint16_t operation;
  uint16_t areaVersion;
  uint8_t isErrorMessage;
  uint16_t bodyLength;
} MessageHeader_t;

//Enumeracion y definiciones de los tipos de interaccion en el protocolo
typedef enum MessageHeaderInteractionType {
  MESSAGE_HEADER_INTERACTION_TYPE_SEND = 1,
  MESSAGE_HEADER_INTERACTION_TYPE_SUBMIT = 2,
  MESSAGE_HEADER_INTERACTION_TYPE_REQUEST = 3,
  MESSAGE_HEADER_INTERACTION_TYPE_PUBSUB = 6
} MessageHeaderInteractionType_t;

//Enumeracion y definiciones de los tipos de servicio en el protocolo (Determina si es TM o TC)
typedef enum MessageHeaderService {
  MESSAGE_HEADER_SERVICE_TELEMETRY,
  MESSAGE_HEADER_SERVICE_TELECOMMAND
} MessageHeaderService_t;

/*Estructura del mensaje completo: Header+body+crc
+--------------+---------------------+-------------+
| Header       | Body                | CRC         |
+--------------+---------------------+-------------+
|<- 224 bits ->|<- Variable length ->|<- 16 bits ->|
*/
typedef struct Message {
  MessageHeader_t header;
  uint8_t *body;
  uint8_t crc[MESSAGE_HANDLER_CRC_SIZE];
} Message_t;


typedef struct MessageHandler {
  CommunicationInterface *communicationChannel;
  Message_t message;
} MessageHandler;


//Prototipos
void MessageHandler_create(MessageHandler *self, CommunicationInterface *communicationChannel);

void MessageHandler_initializeHeader(MessageHandler *self,
                                     uint8_t interactionType,
                                     uint8_t interactionStage,
                                     MessageHeaderService_t service,
                                     bool isErrorMessage);

void MessageHandler_send(MessageHandler *self, uint8_t *messageBody,
                         uint16_t messageBodySize);

bool MessageHandler_receive(MessageHandler *self);

void MessageHandler_setHeader(MessageHandler *self, MessageHeader_t header);

MessageHeader_t MessageHandler_getHeader(MessageHandler *self);

void MessageHandler_setOperation(MessageHandler *self, uint16_t operation);

Message_t *MessageHandler_getMessage(MessageHandler *self);

bool MessageHandler_isTelecommand(MessageHandler *self);

#endif  // MESSAGE_HANDLER_H_
