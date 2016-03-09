# Eco Robot - IMU - ROBOCON 2016

## Brief
This is the implementation of ArduIMU(http://www.geeetech.com/xzn-arduimu-9-degrees-of-freedom-p-535.html) in the Eco Robot.
It uses the razor-9dof-ahrs library(https://github.com/ptrbrtz/razor-9dof-ahrs) for the firmware

## Limitation
This version disabled the magnetic calibration feature, such that serious dirft in yaw exists. (pitch and roll are ok.) The drift is about 1deg/sec.