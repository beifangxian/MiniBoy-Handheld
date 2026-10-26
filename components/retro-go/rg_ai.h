#pragma once

#include <stdbool.h>
#include <stddef.h>

bool rg_ai_voice_chat(const char *context, char *reply, size_t reply_size, char *error, size_t error_size);
void rg_ai_show_reply(const char *title, const char *reply);
