#ifndef encoder_h
#define encoder_h
#include<COBS.h>
#include<integrity.h>

uint8_t globalBuffer[512];

struct __attribute__((packed)) data{
  uint16_t startByte;
  uint8_t length;
  uint8_t packet_counter;
  uint8_t sensorID;
  uint8_t statusCode;
  uint32_t timestamp;
  uint16_t crc_remainder;
  uint8_t checksum;
};

union telemetry {
  uint8_t localBuffer[512];
  data values;
};

telemetry myData;

struct frame{
  telemetry* packet;
  uint8_t length;
};

cobs_packet parse(uint8_t sensorID, uint8_t length, void* rawValue, uint8_t statusCode, uint32_t timestamp){
  static uint8_t count = 0;
  count++;
  data* info = &(myData.values);
  uint8_t* payload_ptr = (myData.localBuffer + sizeof(data));
  (*info).startByte = 0xFFAA;
  (*info).sensorID = sensorID;
  if(length>(255-sizeof(data))){
    while(1);
  }
  (*info).length = sizeof(data) + length;
  (*info).packet_counter = count;
  memcpy(payload_ptr, rawValue, length);
  (*info).statusCode = statusCode;
  (*info).timestamp = timestamp;
  int checksum_offsets[3];
  int crc_offsets[2];
  checksum_offsets[0] = offsetof(data, crc_remainder);
  checksum_offsets[1] = checksum_offsets[0]+1;
  checksum_offsets[2] = offsetof(data, checksum);
  crc_offsets[0] = checksum_offsets[0];
  crc_offsets[1] = checksum_offsets[1];
  uint8_t checksum = calculateChecksum((uint8_t*)(myData.localBuffer), (*info).length, checksum_offsets);
  (*info).checksum = checksum;
  uint16_t crc_remainder = calculate_crc_remainder((uint8_t*)(myData.localBuffer), (*info).length, crc_offsets);
  (*info).crc_remainder = crc_remainder;
  frame frame;
  frame.packet = &myData;
  frame.length = (*info).length;
  //return(frame);
  cobs_packet final_returnable = cobs_encode((uint8_t*) ((*(frame.packet)).localBuffer), frame.length);
  Serial.write(final_returnable.packet_addr, final_returnable.length);
  return final_returnable;
}
#endif