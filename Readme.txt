source codes for BallIPMini ; a robot balanced on a ball
                                 @kumarobo, KUMAGAI Masaaki

Description:
Basically, these codes are written for a specific robot, BallIP Mini, consisting of 
* dsPIC33e core startup codes (e.g. PLL start up)
* USB OTG code for a few types of wired/wireless Joypads
* Communication via (local, special) protocol "SerialLoop"
* InvenSense MPU6050(3 axes gyros + 3 axes accelerometers) codes via I2C
  (not including DMP but burst reception using FIFO)
* Stepping motor DDS pulse rate command code
* Futaba RC servo module RS401,2,5,6 serial communication codes

How to build:
Use Microchip MPLABX, and load BRC_BallIPMini.X as project.

Licensing: as is
You can use these codes for your projects (except for ones originated from Microchip), but there is no support.
Comments on source files are terribly insufficient.
