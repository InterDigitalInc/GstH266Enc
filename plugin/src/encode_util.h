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
