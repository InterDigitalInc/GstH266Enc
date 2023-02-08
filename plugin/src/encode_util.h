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

#include "common.h"
#include "vvenc_util.h"

typedef int (*H266_INIT)(H266Config*);
typedef int (*H266_START)();
typedef int (*H266_ENCODE)(H266Frame*);
typedef int (*H266_STOP)();
typedef int (*H266_FLUSH)();
typedef int (*H266_CLOSE)();

typedef enum H266_ENCODER_TYPE H266EncoderType;
typedef struct ENCODER_UTIL EncoderUtil;

enum H266_ENCODER_TYPE {
    UNKNOWN_ENCODER = 0,
    VVENC_ENCODER = 1,
    UVG_ENCODER = 2
};

struct ENCODER_UTIL {
    H266_INIT init;
    H266_START start;
    H266_ENCODE handle;
    H266_STOP stop;
    H266_FLUSH flush;
    H266_CLOSE close;
};

static H266EncoderType encoderType;
static EncoderUtil encoderUtil;

H266Status initEncoder(H266EncoderType type, H266Config* vvencConfig);
H266Status encodeFrame(H266Frame *frame);
H266Status closeEncoder();
