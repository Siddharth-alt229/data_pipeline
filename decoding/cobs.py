def cobs(packet):
	length = len(packet)
	code_byte = 1;
	jump = 0;
	if packet[0] != 0x00:
		raise ValueError("Start byte invalid")
	counter = 0
	for i in range(0, length):
		if (counter == code_byte):
			jump = packet[i]
			packet[counter] = 0x00
		else:
			if (counter-code_byte) == jump:
				packet[counter] = 0x00
				code_byte = counter
			if(length-code_byte)<jump:
				raise IndexError("Invalid packet")
		counter += 1
	packet.pop(length-1)
	packet.pop(0)
	packet.pop(0)
	if(len(packet) < 13):
		raise ValueError("Invalid packet")
	return packet