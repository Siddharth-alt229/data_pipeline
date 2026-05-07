import serial
import struct
import time
from collections import deque
import cobs
import integrity

baud_rate = 115200
ser = serial.Serial('COM3', baud_rate, timeout=1)
sleep_time = 0

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

window = deque(maxlen = 512)
def get_packet(baud_rate = baud_rate, crc_key = integrity.crc_key):
	while(ser.in_waiting > 0):
		window.append(ser.read(1)[0])
	packet_stat = 0
	packet = None
	while(len(window)>16):
		prov_packet = []
		if(window[0] == 0x00 and window[1] != 0x00):
			counter = 0
			while(window[counter] != 0x00 or ((window[counter] == 0x00) and counter == 0)):
				try:
					prov_packet.append(window[counter])
				except IndexError as e:
					window.popleft()
					return {'packet': None, 'status': 2}
				counter += 1
			if(len(prov_packet)>4 and prov_packet[2] == 0xAA and prov_packet[3] == 0xFF):
				try:
					decoded_packet = cobs.cobs(prov_packet)
				except IndexError as e:
					window.popleft()
					return {'packet': None, 'status': 2}
			else:
				window.popleft()
				return {'packet': None, 'status': 1}
			received_crc_remainder = (decoded_packet[10] << 8) | decoded_packet[11]
			recieved_checksum = decoded_packet[12]
			calculated_sum = integrity.calc_checksum(decoded_packet, [10, 11, 12])
			calculated_crc_remainder = integrity.calc_crc_remainder(decoded_packet, [10, 11])
			if((recieved_checksum == calculated_sum) and (received_crc_remainder == calculated_crc_remainder)):
				packet_count, sensor_id, status, timestamp = struct.unpack('<BBBI', bytes(decoded_packet[3:10]))
				raw_value = decoded_packet[13:]
				packet_stat = 0
				data.successful_packets += 1
				length = len(decoded_packet)
				packet = data(sensor_id, raw_value, status, timestamp, length, packet_count)
				if data.previous_packet and (packet_count != (data.previous_packet.packet_count + 1) % 256):
					data.dropped_packets += 1;
				data.previous_packet = packet
				for _ in range(len(prov_packet)): window.popleft()
				return {'packet': packet, 'status': packet_stat}
			else:
				window.popleft()
				packet_stat = 1
				data.corrupted_packets += 1
				return{'packet': packet, 'status': packet_stat}


	packet_stat = 3
	return {'packet': None, 'status': packet_stat}