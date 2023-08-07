#ifndef LIB_HEIGHT_CONTROLLER_H_
#define LIB_HEIGHT_CONTROLLER_H_

/**
 * @brief Controller for rover height
 */
typedef struct HeightController {
    float currentPosition;
    float touchAngle;
    float maxVel;
    float motorSamplingPeriod;
    float initAccelTime;
    float maxAccel;
    float currentVel;
    int totalSteps;
    int currentStep;
    float accelTime;
    int signVel;
    float timeAtMaxSpeed;
    float totalTime;
} HeightController;

/**
 * @brief Constructor for height controller
 *
 * @param self Uninitialized height controller structure
 */
void HeightController_new(HeightController *self);

/**
 * @brief Configure internal parameters for a given target height
 *
 * @param self Initialized height controller structure
 * @param normalizedTargetHeight Normalized target height (between 0 and 1)
 */
void HeightController_setNewTargetHeight(HeightController *self, float normalizedTargetHeight);

/**
 * @brief Compute a single step of height control and get compute result
 *
 * @param self Initialized height controller structure
 * @return float Leg angle in degrees
 */
float HeightController_compute(HeightController *self);

#endif  // LIB_HEIGHT_CONTROLLER_H_
