#_MR. ANTELOPE_

##HOW TO USE
1. Turn on the switch
2. Do not move until the IMU is calibrated, wait until Mario song is played.
3. Choose your side, if you are on the red side, press the red button, press white button for vice versa
4. Press white button to activate the servo, a beep sound will come out, that means servo is ready
5. Wait until the Mario sound plays, that means the IMU is calibrated
6. You are ready to go!

## Retry Situation (Important)
1. For the sloping part, there are 7 stages: all values displayed on the TFT LCD
2. START --> GREENSLOPE1 --> ORANGE1 --> GREENSLOPE2 --> ORANGE2 --> GREENSLOPE3 --> FINISHEDSLOPE
3. In case you fail in the sloping part, restart the program(see HOW TO USE)
4. Press the RED button until you get the correct region and then you are ready to go again
5. In case you fail after the ninety-degree, restart the program(see HOW TO USE)
6. Press the WHITE button, and you are ready to go.

##CHANGING THE BEHAVIOUR OF THE ROBOT
1. Open Keil
2. Open "bajaj.c"
3. Find the function called "initializeValues()"
4. Modify the constants there
5. Use ST-Link and burn program to the robot

##WARNINGS (IMPORTANT!)
**Make sure:**

1. The sail is tight
2. Servo horn's screw is tight enough
3. The 'count' variable always starts with 0
4. If you want to retry after 90 degrees area, Turn joystick to left, otherwise **you will not be able to cross river**!

##CONTRIBUTOR
Budi RYAN (bryanaa@connect.ust.hk)

##SPECIAL THANKS TO
Rex CHENG(hkchengad@connect.ust.hk) for the IMU program


##![alt tag](http://efdreams.com/data_images/dreams/antelope/antelope-01.jpg)
