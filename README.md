# CO2 Sensor on TTGO with MQTT
In order to reduce the number of wires and required soldering, this sensor is using the TTGO module.

The module is connected via MQTT to the home automation.
Using MQTT and NodeRED on a raspberry is enabling a simple and fast way to create a nice user interface.
In addition you can control the Sensor module as it is subscribing to a specific topic.

## User Interface
### GUI 1 - All Sensors
![TTGO-1](images/TTGO-1.jpg)
<br><br>
### GUI 2: CO2 + Temperature
![GitHub Logo](images/TTGO-2.jpg)
<br>
### GUI 3: CO2 only - color depending on level
![GitHub Logo](/images/TTGO-3.jpg)

### GUI 4: CO2 gauge
![GitHub Logo](images/TTGO-4.jpg)

### GUI 5: graph
![GitHub Logo](./images/TTGO-5.jpg)
<br>
### GUI 6: Admin page
![GitHub Logo](./images/TTGO-6.jpg)


## CO2 Sensor
MHZ19B
Two good German resources which helped me to get started:
https://unsinnsbasis.de/co2-sensor-mhz19b/
http://steinlaus.de/stinkt-das-hier-teil-2-mit-dem-winsen-mh-z19b/#comment-18510
<br><br>
## Humidity and Temperature Sensor
The DHT22 is measuring both while using only one wire for data submission. As an alternative BME280 might be used.

<br><br>
## Wiring


<br><br>
## MQTT

<br><br>
## Configuration


<br><br>
## ToDo
- Calibration   
