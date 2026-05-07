crc_key = 0x1021

def calc_checksum(packet_data, offset_bytes):
	checksum = 0
	counter = 0
	for i in packet_data:
		if(counter not in offset_bytes):
			checksum += i
		counter += 1
	return checksum%256

def calc_crc_remainder(packet_data, offset_bytes):
	crc_remainder = 0x00
	first_byte = 0
	second_byte = 0
	for i in range(0, len(packet_data), 2):
		first_byte = (0x00 if i in offset_bytes else packet_data[i])
		if ((i+1<len(packet_data)) and ((i+1) in offset_bytes)):
			second_byte = 0x00
		else:
			if((i+1)<len(packet_data)):
				second_byte = packet_data[i+1]
			else:
				second_byte = 0b00;
		if((i+1)<len(packet_data)):
			case_detector = 16
			point = (first_byte<<8) | second_byte
			crc_remainder ^= point
		else:
			case_detector = 8
			crc_remainder = (crc_remainder) ^ first_byte
		for bit in range(0, case_detector):
			if(crc_remainder & 0x8000):
				crc_remainder = ((crc_remainder << 1) ^ crc_key) & 0xFFFF
			else:
				crc_remainder = (crc_remainder << 1) & 0xFFFF
	return crc_remainder