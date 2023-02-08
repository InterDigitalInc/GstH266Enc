#pragma once

#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

#include <stdbool.h>

typedef struct H266_FRAME H266Frame;
typedef struct H266_PLANE H266Plane;
typedef struct H266_CONFIG H266Config;
typedef enum H266_STATUS H266Status;
typedef enum H266FRAME_SLICE_TYPE H266FrameSliceType;
typedef enum H266_VIDEO_FORMAT H266VideoFormat;

enum H266_VIDEO_FORMAT {
    H266_DEFAULT = 0,
    H266_VIDEO_FORMAT_I420 = 1,
    H266_VIDEO_FORMAT_I420_10LE = 2
};

enum H266_STATUS {
    H266_SUCCESS = 0,
    H266_PAYLOAD_AVAILABLE = 1,
    H266_ENCODING_DONE = 2,
    H266_ERR = -1
};

enum H266FRAME_SLICE_TYPE {
    H266_I_S = 0,
};

struct H266_CONFIG {
    unsigned int height;
    unsigned int width;
    unsigned int bitrate;
    int framerate;
    int qp;
    int logLevel;
    H266VideoFormat format;
    int depth[3];
};

struct H266_PLANE {
    void *payload;
    int width;
    int height;
    int stride;
    int payloadSize;
};

struct H266_FRAME {
    H266Plane input_planes[3];
    void *output_payload;
    unsigned int outputPayloadSize;
    H266FrameSliceType sliceType;
    bool ctsValid;
    int dts;
    unsigned long poc;
    bool silence;
    bool outputPayloadAvailable;
};
#endif // HEADER_H