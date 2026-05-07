#ifndef COBS_h
#define COBS_h
#include<Arduino.h>


uint8_t encoded_packet[512];
uint8_t length;

struct cobs_packet {
    uint8_t* packet_addr;
    uint8_t length;
};

cobs_packet encoded_returnable;

cobs_packet cobs_encode(uint8_t* data_addr, uint8_t length){
    uint8_t count = 1;
    encoded_packet[0] = 0x00;
    encoded_returnable.length = length+3;
    uint8_t cobs_byte_index = 1;
    for(uint8_t i=0; i<length; i++){
        if((data_addr[i]) != 0x00){
            count++;
            encoded_packet[i+2] = data_addr[i];
        } else{
            encoded_packet[cobs_byte_index] = count;
            cobs_byte_index = i+2;
            count = 1;
        }
    }
    encoded_packet[length+2] = 0x00;
    encoded_packet[cobs_byte_index] = count;
    encoded_returnable.packet_addr = (uint8_t*) encoded_packet;
    return encoded_returnable;
}

#endif