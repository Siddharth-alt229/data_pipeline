#ifndef integrity_h
#define integrity_h
#include<Arduino.h>

uint16_t crc_key = 0x1021;

uint8_t calculateChecksum(uint8_t* ptr, size_t len, int offset_bytes[3]) {
  uint8_t sum = 0;
  for (size_t i = 0; i < len; i++) {
    if(i != offset_bytes[0] && i != offset_bytes[1] && i != offset_bytes[2]){
      sum += ptr[i];
    }
  }
  return sum;
}

uint16_t calculate_crc_remainder(uint8_t* ptr, size_t len, int offset_bytes[2]) {
  uint16_t crc_remainder = 0x00;
  uint8_t first_byte;
  uint8_t second_byte;
  for (int i=0; i<len; i+=2){
    first_byte = (i == offset_bytes[0] || i==offset_bytes[1]) ? 0b00 : ptr[i];
    if(((i+1)>=len) || (i+1) == offset_bytes[0] || (i+1) == offset_bytes[1]){
        second_byte = 0x00;
    } else{
        second_byte = ptr[i+1];
    }
    int case_detector = 16;
    if((i+1)<len){
      crc_remainder ^= ((first_byte<<8) | second_byte);
    } else{
      crc_remainder ^= first_byte;
      case_detector = 8;
    }
    for(int j=0; j<case_detector; j++){
      if(crc_remainder & 0x8000){
        crc_remainder = (crc_remainder<<1) ^ crc_key;
      } else{
        crc_remainder = crc_remainder << 1;
      }
    }
  }
  return crc_remainder;
}

#endif