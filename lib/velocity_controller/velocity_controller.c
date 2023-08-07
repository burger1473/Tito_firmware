#include "velocity_controller.h"

//******************************************************************************
// Public functions
//******************************************************************************
void VelocityController_new(VelocityController *self) {
  self->currentPosition = 0;
  self->maxVel = 50;
  self->motorSamplingPeriod = 0.1;  // Sampling rate 10 Hz
  self->minGroundPosition = 60;
  self->maxGroundPosition = 120;
  self->normalizedTargetVelocity = 0;

  float groundPeriod = self->maxGroundPosition - self->minGroundPosition;
  self->groundSpeed = self->maxVel * (groundPeriod / (360 - groundPeriod));
}

void VelocityController_setNewTargetVelocity(VelocityController *self,
                                             float normalizedTargetVelocity) {
  self->normalizedTargetVelocity = normalizedTargetVelocity;
}

float VelocityController_compute(VelocityController *self) {
  float velocity = 0;
  float newTargetPosition = 0;
  if (self->currentPosition > self->minGroundPosition && self->currentPosition < self->maxGroundPosition) {
    velocity = self->groundSpeed * self->normalizedTargetVelocity;
  } else {
    velocity = self->maxVel * self->normalizedTargetVelocity;
  }
  newTargetPosition = self->currentPosition + velocity * self->motorSamplingPeriod;
  if (newTargetPosition > 360) { newTargetPosition = 0; }
  if (newTargetPosition < 0) { newTargetPosition = 360; }
  self->currentPosition = newTargetPosition;
  return newTargetPosition;
}
