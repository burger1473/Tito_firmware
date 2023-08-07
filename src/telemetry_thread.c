#include "telemetry_thread.h"

//******************************************************************************
// Private methods
//******************************************************************************
TelemetryThread *TelemetryThread_self;

static void TelemetryThread_computeTelemetries(TelemetryThread *self) {
  for (int i = 0; i < self->telemetryManager->currentNumberOfTelemetry; i++) {
    TelemetryInterface *telemetry=self->telemetryManager->telemetry[i];
    if(TelemetryInterface_isEnabled(telemetry)){
      TelemetryInterface_execute(telemetry, &self->messageHandler);
    }
  }
}

//******************************************************************************
// Public methods
//******************************************************************************
void TelemetryThread_create(TelemetryThread *self,
                            TelemetryManager *telemetryManager,
                            CommunicationInterface *messageChannel) {

  self->telemetryManager = telemetryManager;                      //Guarda en la estructura del hilo de Tm el puntero al manajador de telemetria
  MessageHandler_create(&self->messageHandler, messageChannel);   //Guarda en la estructura del hilo de telecomando el puntero de la interface de telecomando para la recepcion de mensajes
  MessageHandler_initializeHeader(&self->messageHandler,          //Inicializa el header de la capa de aplicacion de  los mensajes
                                  MESSAGE_HEADER_INTERACTION_TYPE_PUBSUB,
                                  1, MESSAGE_HEADER_SERVICE_TELEMETRY, false);
}

void TelemetryThread_start(void *self, void *, void *) {
  TelemetryThread  *_self = (TelemetryThread *)self;
  TelemetryThread_self = (TelemetryThread *)self;
  while (true) {
    TelemetryThread_computeTelemetries(_self);
    k_msleep(1000);
  }
}
