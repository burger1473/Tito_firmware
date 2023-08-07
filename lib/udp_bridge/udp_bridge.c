#include "udp_bridge.h"

#include "TMTC_I2C.h"

#define UDP_BRIDGE_ADDRESS 0x50

//******************************************************************************
// Interface implementations prototypes (Prototipo de funciones)
//******************************************************************************
static uint8_t UdpBridge_send(void *self, const uint8_t *buffer, size_t bufferSize);
static uint8_t UdpBridge_receive(void *self, uint8_t *buffer, size_t bufferSize,
                                 size_t *receivedSize);

//******************************************************************************
// Private methods
//******************************************************************************
/*========================================================================
  Funcion: dpBridge_initializeInterface
  Descripcion: Inicia interface udpbridg
  Parametro de entrada:
                        UdpBridge *self:            Puntero a la interface que se quiere inicializar
  No retorna nada
  ========================================================================*/
static void UdpBridge_initializeInterface(UdpBridge *self) {
  self->communicationInterfaceView.instance = self;               //Guarda puntero a su misma interface
  self->communicationInterfaceView.send = UdpBridge_send;         //Puntero a la funcion para enviar TM
  self->communicationInterfaceView.receive = UdpBridge_receive;   //Puntero a la funcion para recibir TC
}

//******************************************************************************
// Interface implementations
//******************************************************************************
/*========================================================================
  Funcion: UdpBridge_send
  Descripcion: Funcion para enviar TM por UDP
  Parametro de entrada:
                        void *self: -
                        const uint8_t *buffer: Buffer que contiene la telemetria a enviar.
                        size_t bufferSize:     Tamaño del buffer
  Rertorna: uint8_t   0 = resultado ok
                      1 = No se pudo realizar
  ========================================================================*/
static uint8_t UdpBridge_send(void *self, const uint8_t *buffer, size_t bufferSize) {
  UdpBridge *_self = (UdpBridge *)self;
  return TMTC_I2C_sendMsg(buffer, bufferSize, UDP_BRIDGE_ADDRESS, _self->destinationPort);  //Envia TM udp por i2c mediante udpbridge
}

/*========================================================================
  Funcion: UdpBridge_receive
  Descripcion: Funcion para recibir TC por UDP
  Parametro de entrada:
                        void *self: -
                        const uint8_t *buffer: Buffer para guardar TC recibido.
                        size_t bufferSize:     Maximo tamaño del buffer.
                        size_t *receivedSize:  Variable para guardar el tamaño del buffer recibido
  Rertorna: uint8_t   0 = resultado ok
                      1 = No se pudo realizar
  ========================================================================*/
static uint8_t UdpBridge_receive(void *self, uint8_t *buffer, size_t bufferSize,
                                 size_t *receivedSize) {
  return TMTC_I2C_readMsg(buffer, bufferSize, receivedSize, UDP_BRIDGE_ADDRESS); //Se obteien dato del udpbridge por i2c
}

//******************************************************************************
// Public methods
//******************************************************************************

/*========================================================================
  Funcion: UdpBridge_create
  Descripcion: Inicia interface udp y asigna un puerto
  Parametro de entrada:
                        UdpBridge *self:            Puntero a la interface que se quiere inicializar
                        uint16_t destinationPort:   Puerto udp que se quiere configurar
  No retorna nada
  ========================================================================*/
void UdpBridge_create(UdpBridge *self, uint16_t destinationPort) {
  UdpBridge_initializeInterface(self);
  self->destinationPort = destinationPort;
}

/*========================================================================
  Funcion: UdpBridge_viewAsCommunicationInterface
  Descripcion: Devuelve el puntero a la interface que se solicita
  Parametro de entrada:
                        UdpBridge *self:            Puntero a la interface
  No retorna nada
  ========================================================================*/
CommunicationInterface *UdpBridge_viewAsCommunicationInterface(UdpBridge *self) {
  return &self->communicationInterfaceView;
}
