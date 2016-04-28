#include <string.h>
#include <can_protocol.h>

typedef struct {
	uint32_t id;
	uint8_t* data;
	size_t pos;
	size_t size;
} CANPacket;

void can_packet_send(CANPacket *packet);
void can_write_int(CANPacket *packet, uint32_t val);
void can_write_byte(CANPacket *packet, uint8_t val);
void can_write_string(CANPacket *packet, char* val);
