#ifndef LIB_VELOCITY_CONTROLLER_H_
#define LIB_VELOCITY_CONTROLLER_H_

/**
 * @brief Controller for rover velocity
 */
typedef struct VelocityController {
  float currentPosition;
  float maxVel;
  float motorSamplingPeriod;
  float minGroundPosition;
  float maxGroundPosition;
  float groundSpeed;
  float normalizedTargetVelocity;
} VelocityController;

/**
 * @brief Constructor for velocity controller
 *
 * @param self Uninitialized velocity controller structure
 */
void VelocityController_new(VelocityController *self);

/**
 * @brief Configure internal parameters for a given target velocity
 *
 * @param self Initialized velocity controller structure
 * @param normalizedTargetVelocity Normalized target velocity (between 0 and 1)
 */
void VelocityController_setNewTargetVelocity(VelocityController *self,
                                             float normalizedTargetVelocity);

/**
 * @brief Compute a single step of velocity control and get compute result
 *
 * @param self Initialized velocity controller structure
 * @return float Leg angle in degrees
 */
float VelocityController_compute(VelocityController *self);

#endif  // LIB_VELOCITY_CONTROLLER_H_
