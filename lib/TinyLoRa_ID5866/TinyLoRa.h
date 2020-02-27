#ifndef TINY_LORA_H
#define TINY_LORA_H

#include <Arduino.h>
#include <avr/pgmspace.h>

// uncomment for debug output
// #define DEBUG

/** RFM channel options */
typedef enum rfm_channels
{
  CH0,
  CH1,
  CH2,
  CH3,
  CH4,
  CH5,
  CH6,
  CH7,
  MULTI,
} rfm_channels_t;

/** RFM fixed datarate, dependent on region */
typedef enum rfm_datarates
{
  SF7BW125,
  SF7BW250,
  SF8BW125,
  SF9BW125,
  SF10BW125,
  SF11BW125,
  SF12BW125,
} rfm_datarates_t;

/** Region configuration*/
#define EU863

/* RFM Modes */
#define MODE_SLEEP 0x00 ///<low-power mode
#define MODE_LORA 0x80  ///<LoRa operating mode
#define MODE_STDBY 0x01 ///<Osc. and baseband disabled
#define MODE_TX 0x83    ///<Configures and transmits packet

/* RFM Registers */
#define REG_PA_CONFIG 0x09    ///<PA selection and Output Power control
#define REG_PREAMBLE_MSB 0x20 ///<Preamble Length, MSB
#define REG_PREAMBLE_LSB 0x21 ///<Preamble Length, LSB
#define REG_FRF_MSB 0x06      ///<RF Carrier Frequency MSB
#define REG_FRF_MID 0x07      ///<RF Carrier Frequency Intermediate
#define REG_FRF_LSB 0x08      ///<RF Carrier Frequency LSB
#define REG_FEI_LSB 0x1E      ///<Info from Prev. Header
#define REG_FEI_MSB 0x1D      ///<Number of received bytes
#define REG_MODEM_CONFIG 0x26 ///<Modem configuration register

/**************************************************************************/
/*!
    @brief  TinyLoRa Class
*/
/**************************************************************************/
class TinyLoRa
{
public:
  uint8_t txrandomNum;   ///<random number for AES
  uint16_t frameCounter; ///<frame counter
  void setChannel(rfm_channels_t channel);
  void setDatarate(rfm_datarates_t datarate);
  TinyLoRa(int8_t rfm_dio0, int8_t rfm_nss);
  bool begin(void);
  void sendData(unsigned char *Data, unsigned char Data_Length, unsigned int Frame_Counter_Tx);

private:
  uint8_t randomNum;
  int8_t _cs, _irq;
  bool _isMultiChan;
  unsigned char _rfmMSB, _rfmMID, _rfmLSB, _sf, _bw, _modemcfg;
  static const unsigned char LoRa_Frequency[8][3];
  static const unsigned char S_Table[16][16];
  void RFM_Send_Package(unsigned char *RFM_Tx_Package, unsigned char Package_Length);
  void RFM_Write(unsigned char RFM_Address, unsigned char RFM_Data);
  uint8_t RFM_Read(uint8_t RFM_Address);
  void Encrypt_Payload(unsigned char *Data, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction);
  void Calculate_MIC(unsigned char *Data, unsigned char *Final_MIC, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction);
  void Generate_Keys(unsigned char *K1, unsigned char *K2);
  void Shift_Left(unsigned char *Data);
  void XOR(unsigned char *New_Data, unsigned char *Old_Data);
  void AES_Encrypt(unsigned char *Data, unsigned char *Key);
  void AES_Add_Round_Key(unsigned char *Round_Key, unsigned char (*State)[4]);
  unsigned char AES_Sub_Byte(unsigned char Byte);
  void AES_Shift_Rows(unsigned char (*State)[4]);
  void AES_Mix_Collums(unsigned char (*State)[4]);
  void AES_Calculate_Round_Key(unsigned char Round, unsigned char *Round_Key);
};

#endif