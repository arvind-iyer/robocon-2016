#ifndef __CAN_CHAT_H
#define __CAN_CHAT_H

#include "can_protocol.h"
#include "tft_160x128.h"
#include "led.h"

#define endOfMessage 1

void can_chat_init(u32 filterBase);
void SendChatMessage(u32 target_id, u8 vel[], u8 length);

#endif