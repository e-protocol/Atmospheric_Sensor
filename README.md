********************************************************************************
Atmospheric Sensor<br/>
<p float="left">
<img src = "Project Photo/1.jpg" width = "200" height = "400" />
<img src = "Project Photo/2.jpg" width = "400" height = "200" />
<img src = "Project Photo/3.jpg" width = "400" height = "200" />
<img src = "Project Photo/4.jpg" width = "200" height = "400" />
<img src = "Project Photo/5.jpg" width = "200" height = "400" />
<img src = "Project Photo/6.jpg" width = "200" height = "400" />
<img src = "Project Photo/7.jpg" width = "200" height = "400" />
<img src = "Project Photo/8.jpg" width = "200" height = "400" />
<img src = "Project Photo/8.jpg" width = "400" height = "400" />
</p>

Created by E-Protocol
https://github.com/e-protocol

Specs: Linux Debian x64, Qt 5.15.1, Arduino Nano, Android

Description: 
This project is about portable device, which allows to measure air quality:
- atmospheric temperature
- pressure
- humidity
- radiation level
- eCO2(air pollution equivalent of CO2) 
- TVOC(air pollutants)

Data transferring via Bluetooth on Android phone.
It also allows to scan for any bluetooth device within the range - 
Max 30 m on open space. Shows Low Energy Bluetooth devices by placing "LE" 
near device name. The Device is found as P-Climate Sensor(from older project). 
Pairing pin 4444. Device can be charged fromg phone charger. 
Project includes some photos.
********************************************************************************

Arduino part

********************************************************************************

Component list:

1) Arduino Nano
2) BME 280 Sensor
3) CCS811 Sensor
4) Geiger sensor with M4011 tube
5) DC-DC converter 0.9-5V Input 5V Output
6) TP4056 charge module for 18650 batteries
7) 2x 18650 battery 2400 mA
8) double battery holder
9) USB-Micro port
10) 3x leds 3v
11) Resistor 220 Ohm
12) Resistor 1000 Ohm
13) Capacitor 1000 uf 16v
14) couple of wires M-F for Arduino
15) 3-pin computer fan plugger
16) 1x key buttton
17) 0.25mm wires
18) Bluetooth HC-06 module
19) plastic electrical box
20) some Solder and flux
21) some hot glue
22) Metall speakers lattice
23) x6 screws and x4 bolts
24) some insulating tape

Arduino programm(sketch):

1) Requires some Arduino libraries: see in sketch
2) Requires make settings of HC-06 Bluetooth module before uploading sketch, i.e. pin and device name setup.
Used pin in project 4444, device name P-Climate Sensor

NOTE: shematic is from older project: "Portable Climate Sensor" - similar to this one. 
I think it will be helpfull to understand basic concept.
