#ifndef __CAN_CHAT_H
#define __CAN_CHAT_H

#include "can_protocol.h"
#include "tft_160x128.h"
#include "led.h"

#define endOfMessage 127

void can_chat_init(u16 filterBase);
void SendChatMessage(u8 target_id, u8 vel[], u8 length);


#endif