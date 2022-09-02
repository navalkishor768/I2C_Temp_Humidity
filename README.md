# i2c_SHT21_Atmega32
Interfacing temperature and humidity sensor with I2C communication.

 I have used a SHT21 temperature and humidity which have I2C communication.
 
 On  Internet, several supports are related to SHT21 are based on Arduino.
 
 By refering Datasheet and using virtual I2C debugger resulted in successful establishment of communication.
 
 Functions sht21_getTemp() and sht21_getHum() are used in main to get respective values.
 
 These float values are then converted to string using dtostrf(*your float value*,2,2,st1);
 
 Using sprintf(st,"Temp is %s",st1), a single string is generated to transmit on terminal.
 
 Below is the Proteus Simulation schematic and video.
 
![i2c_sht21_temphum](https://user-images.githubusercontent.com/111571035/187496508-6a42ad24-5cc3-4fc6-97c4-e5b920ad4194.SVG)

https://drive.google.com/file/d/1REUr8g_ixc0Ng-FG9YMLUKReEDBTLGZ3/view?usp=sharing
