#include "unity.h"
#include "height_controller.h"

static HeightController instance;

void setUp() {
    HeightController_new(&instance);
}

void tearDown() { }

void test_HeightController_new() {
    TEST_ASSERT_EQUAL(0, instance.currentPosition);
    TEST_ASSERT_EQUAL_FLOAT(0.1, instance.motorSamplingPeriod);
}

void test_HeightController_setNewTargetHeight() {
    HeightController_setNewTargetHeight(&instance, 0.5);

    TEST_ASSERT_EQUAL_FLOAT(0.8, instance.timeAtMaxSpeed);
    TEST_ASSERT_EQUAL_FLOAT(1.2, instance.totalTime);
    TEST_ASSERT_EQUAL_FLOAT(0.2, instance.accelTime);
    TEST_ASSERT_EQUAL_INT(1, instance.signVel);
    TEST_ASSERT_EQUAL_INT(12, instance.totalSteps);
    TEST_ASSERT_EQUAL_FLOAT(0, instance.currentVel);
    TEST_ASSERT_EQUAL_FLOAT(0, instance.currentStep);
}

void test_HeightController_compute() {
    HeightController_setNewTargetHeight(&instance, 0.5);

    float currentPosition = 0;

    currentPosition = HeightController_compute(&instance);
    TEST_ASSERT_EQUAL_FLOAT(2.5, currentPosition);

    currentPosition = HeightController_compute(&instance);
    TEST_ASSERT_EQUAL_FLOAT(7.5, currentPosition);

    currentPosition = HeightController_compute(&instance);
    TEST_ASSERT_EQUAL_FLOAT(12.5, currentPosition);

    currentPosition = HeightController_compute(&instance);
    TEST_ASSERT_EQUAL_FLOAT(17.5, currentPosition);

    currentPosition = HeightController_compute(&instance);
    TEST_ASSERT_EQUAL_FLOAT(22.5, currentPosition);

    currentPosition = HeightController_compute(&instance);
    TEST_ASSERT_EQUAL_FLOAT(27.5, currentPosition);
}

#ifdef TEST_STATIC_FUNCTIONS
void test_HeightController_round() {
    float rounded = HeightController_round(&instance, 0.533);

    TEST_ASSERT_EQUAL_FLOAT(0.5, rounded);
}

void test_HeightController_ceil() {
    float ceil = HeightController_ceil(&instance, 3.533);

    TEST_ASSERT_EQUAL_FLOAT(4, ceil);
}
#endif
