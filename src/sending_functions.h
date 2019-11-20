
/*!  \name File usage
 * In this File the functions for LaRa communication are stored.
 */
#pragma once

#include <Debug.h>
#include <TinyLoRa.h>
#include <config.h>
#include <camera.h>
#include <SI7021.h>
#include <lmic_util.h>
#include <global_variable.h>
//******************Configuration of TinyLoRa********************
/*!  \brief Network Session Key (MSB)*/
uint8_t NwkSkey[16] = NWKSKEY;

/*!  \brief Application Session Key (MSB) */
uint8_t AppSkey[16] = APPSKEY;

/*!  \brief Device Address (MSB). Used by TTN */
uint8_t DevAddr[4] = DEVADDR;

/*!  \brief Buffer for the LoRa payload. */
unsigned char payload[14];

// Pinout for Adafruit Feather M0 LoRa
TinyLoRa LoRa_jdmt_data_logger = TinyLoRa(3, 8);

/*!  \brief Converts a float variable to a 2 byte int and ads it to the payload buffer. The Value needs to be decoded in the TTN decoder. */
void mapToPayload(uint8_t i,  /*!< Position in the payload array, where the low byte of the value get set.  */
                  float value /*!< Value that gets written in to the buffer. Has to be between 0 and 1. */
)
{
  // float -> int
  uint16_t payloadValue = LMIC_f2sflt16(value);
  // int -> bytes
  byte low = lowByte(payloadValue);
  byte high = highByte(payloadValue);
  // place the bytes into the payload
  payload[i] = low;
  payload[i + 1] = high;
}

/*!  \brief Prints the payload. Used for debugging. */
void print_payload()
{
  debug("Payload: ");
  for (unsigned int i = 0; i < sizeof(payload); i++)
  {
    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

/*!  \brief Encode the data to send to the byte array payload. */
void preparePayolad()
{
  // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
  debugLn();
  float temperature = envSensor->getCelsiusHundredths() / 100;
  debug("Tempercature: ");
  debug(temperature);
  debugLn(" *C");
  // adjust for the f2sflt16 range (-1 to 1)
  temperature = temperature / 100;
  mapToPayload(0, temperature);
  // read the humidity from the DHT22
  float rHumidity = envSensor->getHumidityPercent();
  debug("Humidity: ");
  debug(rHumidity);
  debugLn(" RH");
  rHumidity = rHumidity / 100;
  mapToPayload(2, rHumidity);
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  debug("Battery: ");
  debug(vbat);
  debugLn(" V");
  vbat /= 10;
  mapToPayload(4, vbat);
  debug("Liklihood of Display shows a check: ");
  debugLn(LikelihoodDeviceOk);
  mapToPayload(6, LikelihoodDeviceOk);
  payload[8] = is_there_CameraModul() + '0'; // conversion from int to char
  debug("is there a Camera Modul: ")
      debugLn(is_there_CameraModul());
  debug("Device Lat: ");
  debugLn(Device_Position_latitude);
  mapToPayload(9, Device_Position_latitude / 90);
  debug("Device Lng: ");
  debugLn(Device_Position_longitude);
  mapToPayload(11, Device_Position_longitude / 180);
  debugLn();
  payload[13] = DeviceNumber;
}