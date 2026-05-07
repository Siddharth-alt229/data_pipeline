NOTE: Decoder uses port COM3 by default. To change this, change port in data_decoder.py, line 9

Syntax for encoder usage:

#include <encoder.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  uint8_t sensor_id;
  uint8_t status_code;
  uint8_t length;
  uint32_t timestamp;
  uint8_t rawValue[length];

  sensor_id = value;
  status_code = value;
  length = length of raw value;
  timestamp = millis();
  [Declare raw value array
  cobs_packet parsed_packet = parse(sensor_id, length, rawValue, status_code, timestamp);
  This frames and writes the packet into the serial buffer]
}

Eg:

#include <encoder.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  uint8_t sensor_id;
  uint8_t status_code;
  uint8_t length;
  uint32_t timestamp;
  uint8_t rawValue[3];

  sensor_id = 8;
  status_code = 1;
  length = 3;
  timestamp = millis();
  rawValue[0] = 5;
  rawValue[1] = 6;
  rawValue[2] = 7;
  cobs_packet parsed_packet = parse(sensor_id, length, rawValue, status_code, timestamp);
}

This returns a cobs_packet struct having 2 attributes
The pointer to the packet buffer (uint8_t*) type
The length of the packet buffer uint8_t

The structure in c is as follows:

struct cobs_packet {
    uint8_t* packet_addr;
    uint8_t length;
};

Syntax for decoder usage:

import data_decoder

while True:
	obtained_packet = data_decoder.get_packet()
	This returns a dictionary with one packet which is an instance of the class data, and the status of the packet. 0=OK, 1=Corrupted, 2=packet incomplete/waiting.

Example:

import data_decoder

while True:
	obtained_packet = Data_decoder.get_packet()
	if not obtained_packet['status']:
		print(f"{obtained_packet['packet'].sensor_id}, {obtained_packet['packet'].packet_count}, {obtained_packet['packet'].raw_value}, {Data_decoder.data.dropped_packets}\n")
	elif obtained_packet['status'] == 1:
		print(f"{obtained_packet['status']}, {Data_decoder.data.dropped_packets}\n")
	else:
		pass

The data class is declared as follows:

class data():
	successful_packets = 0
	corrupted_packets = 0
	dropped_packets = 0
	previous_packet = None
	def __init__(self, sensor_id, raw_value, status_code, timestamp, length, packet_count):
		self.packet_count = packet_count
		self.sensor_id = sensor_id
		self.raw_value = raw_value
		self.status_code = status_code
		self.timestamp = timestamp
		self.length = length

Instance attributes are:
sensor_id
packet_count
raw_value --> raw value is a raw bitstream
status_code
timestamp
length --> length of the packet

Class attributes are:
Successful packets
Corrupted packets representing packets which were received but corrupted
Dropped packets representing corrupted packets, along with those which were never received, inferred from packet_count which is incremented by the encoder every packet