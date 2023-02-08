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

#include <stdio.h>
#include <windows.h>

#include "encode_util.h"

static HINSTANCE hLibrary;

int initEncoder(H266EncoderType type, H266Config* h266Config) {
    encoderType = type;

    switch(encoderType) {
        case VVENC_ENCODER:
        hLibrary = LoadLibrary("vvenc_lib.dll");

        if (hLibrary != NULL) {
            
            encoderUtil.init = (H266_INIT)GetProcAddress(hLibrary, "vvenc_init");
            encoderUtil.handle = (H266_ENCODE)GetProcAddress(hLibrary, "vvenc_handle");
            encoderUtil.close = (H266_CLOSE)GetProcAddress(hLibrary, "vvenc_close");

            encoderUtil.init(h266Config);
        }
        break;

        case UVG_ENCODER:
        hLibrary = LoadLibrary("uvg_lib.dll");

        if (hLibrary != NULL) {
            
            encoderUtil.init = (H266_INIT)GetProcAddress(hLibrary, "uvg_init");
            encoderUtil.handle = (H266_ENCODE)GetProcAddress(hLibrary, "uvg_handle");
            encoderUtil.close = (H266_CLOSE)GetProcAddress(hLibrary, "uvg_close");

            encoderUtil.init(h266Config);
        }
        break;

        default:
        break;
    }


    return H266_SUCCESS;
}

H266Status encodeFrame(H266Frame *frame) {
   return encoderUtil.handle(frame); 
}

H266Status closeEncoder() {
    return encoderUtil.close();
}