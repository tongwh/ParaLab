#ifndef UTIL_DEBUG_H
#define UTIL_DEBUG_H

#include "logger.h"

const int L_LOW = 0;
const int L_MED = 10;
const int L_MEDPLUS = 15;
const int L_HIGH = 20;
const int L_FREQ = 30;
extern Logger Log;

#define LOG_MESSAGE_BLOCK(level, message) const Logger::Block &_Log_Block = Log.block(level, message);

#endif // UTIL_DEBUG_H

