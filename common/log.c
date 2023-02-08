#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

unsigned char h266_log_level = LOG_INFO;

const char * h266_log_level_strings [] = {
	"NONE", // 0
	"WARN", // 1
	"INFO", // 2
	"DEBUG",// 3
	"ERROR" // 4
};

void h266_log_format(enum h266_log_level_e level, const char* tag, const char* message, va_list args) {
   if(level <= h266_log_level)
   {
      time_t now;
      time(&now);
      char * date =ctime(&now);
      date[strlen(date) - 1] = '\0';  
      printf("%s [%s] ", date, tag);      
      vprintf(message, args);
      printf("\n");
      fflush(stdout);
   }
}

void h266_log_error(const char* message, ...) {  
   va_list args;
   va_start(args, message);
   h266_log_format(LOG_ERROR, "H266_ERROR", message, args);
   va_end(args);
}

void h266_log_info(const char* message, ...) {   
   va_list args;
   va_start(args, message);
   h266_log_format(LOG_INFO, "H266_INFO", message, args);
   va_end(args);
}

void h266_log_debug(const char* message, ...) {
   va_list args;
   va_start(args, message);
   h266_log_format(LOG_DEBUG, "H266_DEBUG", message, args);
   va_end(args);
}