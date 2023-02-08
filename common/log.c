/* Copyright (c) 2023, InterDigital Communications, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of InterDigital Communications, Inc nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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