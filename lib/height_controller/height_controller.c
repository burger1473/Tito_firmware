#include "height_controller.h"

#include <math.h>

#define ABS(x) ((x) > 0 ? (x) : -(x))

//******************************************************************************
// Private functions
//******************************************************************************
static float HeightController_findNearest(HeightController *self, float theta) {
    float discrete_theta = 0;
    float error = 0;
    float step = self->maxVel * self->motorSamplingPeriod;
    while (discrete_theta <= 90) {
        error = theta - discrete_theta;
        if (ABS(error) < step) { break; }
        discrete_theta += step;
    }
    return discrete_theta;
}

static float HeightController_height2theta(HeightController *self, float height) {
    float theta = height * self->touchAngle * 180 / M_PI;
    return HeightController_findNearest(self, theta);
}

static float HeightController_round(HeightController *self, float number) {
    number /= self->motorSamplingPeriod;
    number = (int)(number + 0.5);
    return number * self->motorSamplingPeriod;
}

static float HeightController_ceil(HeightController *self, float number) {
    int intNumber = (int)number;
    if (number == (float)intNumber) {
        return intNumber;
    }
    return intNumber + 1;
}

//******************************************************************************
// Public functions
//******************************************************************************
void HeightController_new(HeightController *self) {
    self->currentPosition = 0;
    self->touchAngle = M_PI_2;
    self->maxVel = 50;
    self->motorSamplingPeriod = 0.1;  // Sampling rate 10 Hz
    self->initAccelTime = 2 * self->motorSamplingPeriod;
    self->maxAccel = self->maxVel / self->initAccelTime;
    self->currentVel = 0;
}

void HeightController_setNewTargetHeight(HeightController *self, float normalizedTargetHeight) {
    float targetPosition = HeightController_height2theta(self, normalizedTargetHeight);
    float angleError = targetPosition - self->currentPosition;
    float absoluteAngleError = ABS(angleError);
    self->timeAtMaxSpeed = HeightController_round(self,
        absoluteAngleError / self->maxVel - self->initAccelTime + self->motorSamplingPeriod);
    if (self->timeAtMaxSpeed == 0) {
        self->totalTime = HeightController_ceil(self, sqrt(angleError / self->maxAccel));
        self->accelTime = self->totalTime / 2;
    } else {
        float absoluteTimeAtMaxSpeed = ABS(self->timeAtMaxSpeed);
        self->totalTime = HeightController_round(self, absoluteTimeAtMaxSpeed + 2 * self->initAccelTime);
        self->accelTime = HeightController_round(self, self->initAccelTime);
    }
    self->signVel = angleError < 0 ? -1 : 1;
    self->totalSteps = HeightController_ceil(self, self->totalTime / self->motorSamplingPeriod);
    self->currentVel = 0;
    self->currentStep = 0;
}

float HeightController_compute(HeightController *self) {
    if (self->currentStep < self->totalSteps) {
        float time = self->currentStep * self->motorSamplingPeriod;
        float previousVel = self->currentVel;
        if (time < self->accelTime) {
            self->currentVel = previousVel + self->signVel * self->maxAccel * self->motorSamplingPeriod;
        } else if (time < self->accelTime + self->timeAtMaxSpeed) {
            self->currentVel = self->signVel * self->maxVel;
        } else if (time < self->totalTime) {
            self->currentVel = previousVel - self->signVel * self->maxAccel * self->motorSamplingPeriod;
        }
        self->currentPosition += self->currentVel * self->motorSamplingPeriod;
        self->currentStep++;
    }
    return self->currentPosition;
}
