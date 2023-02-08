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