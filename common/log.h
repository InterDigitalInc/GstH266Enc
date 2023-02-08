#include <stdarg.h>

#ifndef LOG_H 
#define LOG_H

enum {
	LOG_NONE, // 0	
	LOG_WARNING, // 1	
	LOG_INFO,   // 2
	LOG_DEBUG, // 3
	LOG_ERROR // 4
}h266_log_level_e;

extern unsigned char h266_log_level;
 
extern const char * h266_log_level_strings [];
 

void h266_log_error(const char* message, ...);
void h266_log_info(const char* message, ...);
void h266_log_debug(const char* message, ...);

#endif