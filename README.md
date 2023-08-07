# Rover-Testing
Software and firmware development of the Roverito project.

This thread presents the corresponding code for the SAMV71 with the following applications:

1. SDO CanOpen communication through the SAMV71 peripheral can.
2. Serial communication via UART peripheral.
3. Library to read MPU6050.
4. Library to command EPOS4 driver with the following actions available:

      a. Motor parameter setting.

      b. Driver parameter setting.

      c. Power Disable.

      d. Read actual position.

      e. Read actual Torque.

      f. Read actual Velocity.

      g. Control mode `PROFILE_POSITION`.

      h. Control mode `Cyclic_Synchronous_Velocity`.

      i. PID parameter configuration.
