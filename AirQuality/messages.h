#ifndef __AIRQ_MESSAGES_H__
#define __AIRQ_MESSAGES_H__

const char message0[] PROGMEM =
    "\e[33m           _       ____   \r\n"
    "     /\\   (_)     / __ \\            \e[32mSensor:\e[33m iAQ-Core\r\n"
    "    /  \\   _ _ __| |  | |           \e[32mS/N:\e[33m 202006-001\r\n"
    "   / /\\ \\ | | '__| |  | |           \e[32mAssembled:\e[33m 2020-06-30\r\n"
    "  / ____ \\| | |  | |__| |           \e[32mBuilt:\e[33m " __DATE__ " " __TIME__ "\r\n"
    " /_/    \\_\\_|_|   \\___\\_\\           \e[32mSrc:\e[33m github.com/nicolacimmino/AirQuality\r\n"
    "         \e[32m(c) Nicola 2020";

const char *const messages[] PROGMEM = {message0};

void printMessage(uint8_t messageId)
{
    char buffer[512];
    strcpy_P(buffer, (char *)pgm_read_word(&(messages[messageId])));
    Serial.print(buffer);
}

#endif
