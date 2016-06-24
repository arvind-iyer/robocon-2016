/*
 * can_packet.c
 *
 *  Created on: Dec 29, 2015
 *      Author: Kenta Iwasaki
 */

#include <can_packet.h>

void assert_size(CANPacket *packet, size_t len) {
	if (packet->pos + len >= packet->size) {
		packet->size += len;
		packet->data = (uint8_t *) realloc(packet->data,
				packet->size * sizeof(uint8_t));
	}
}

// UNCOMMENT WHEN DEBUGGING - STUB FUNCTION FOR DISPLAYED QUEUED MESSAGE DATA.

//char * stringBuffer;
//bool readingString;
//int buffPos;
//
//void can_tx_enqueue(CAN_MESSAGE msg) {
//	printf("ID: %d\n", msg.id);
//	printf("Data: ");
//
//	for (int i = 0; i < msg.length; i++) {
//		if (!readingString) {
//			if (msg.data[i] == '|') {
//				stringBuffer = calloc(512, sizeof(char));
//				readingString = true;
//				buffPos = 0;
//			} else {
//				printf("%d ", msg.data[i]);
//			}
//		} else {
//			if (msg.data[i] == '|') {
//				stringBuffer[buffPos] = '\0';
//				printf("\nMESSAGE: %s\n", stringBuffer);
//			} else {
//				stringBuffer[buffPos++] = msg.data[i];
//			}
//		}
//	}
//	printf("\n\n");
//}

void can_packet_send(CANPacket *packet) {
	// Copies segments of 8 bytes into CAN_MESSAGE's and sends it.
	for (int i = 0; i < packet->size / 8; i++) {
		CAN_MESSAGE msg;
		msg.id = packet->id;

		memcpy(msg.data, &packet->data[i * 8], 8 * sizeof(uint8_t));

		msg.length = sizeof(msg.data) / sizeof(msg.data[0]);

		can_tx_enqueue(msg);
	}

	// If there are remaining bytes, enqueue it.
	if (packet->size % 8 > 0) {
		uint8_t remaining = packet->size % 8;

		CAN_MESSAGE msg;
		msg.id = packet->id;
		msg.length = remaining;
		memcpy(msg.data, &packet->data[packet->size - remaining], remaining);

		can_tx_enqueue(msg);
	}
}

void can_write_int(CANPacket *packet, uint32_t val) {
	assert_size(packet, 4);

	packet->data[packet->pos] = val & 0xFF;
	packet->data[packet->pos + 1] = (val >> 8) & 0xFF;
	packet->data[packet->pos + 2] = (val >> 16) & 0xFF;
	packet->data[packet->pos + 3] = (val >> 24) & 0xFF;
	packet->pos += 4;
}

void can_write_string(CANPacket *packet, char* val) {
	assert_size(packet, 2 + strlen(val));

	packet->data[packet->pos] = '|';
	for (int i = 0; i < strlen(val); i++) {
		packet->data[packet->pos + i + 1] = val[i];
	}
	packet->data[packet->pos + strlen(val) + 1] = '|';

	packet->pos += 2 + strlen(val);
}

void can_write_byte(CANPacket *packet, uint8_t val) {
	assert_size(packet, 1);
	packet->data[packet->pos] = val;
	packet->pos += 1;
}
