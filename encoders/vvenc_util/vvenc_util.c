
#include <stdio.h>
#include "vvenc_util.h"

#include "vvenc/vvenc.h"
#include "vvenc/vvencCfg.h"
#include <windows.h>

static HINSTANCE hLibrary;

static vvencEncoder *encoder;
static vvenc_config vvenccfg;
static bool bEncodeDone;
static vvencAccessUnit AU;
static vvencYUVBuffer cYUVInputBuffer;
static vvencEncoder *enc;

// typedef vvencYUVBuffer* (*VVENC_BUFFER_ALLOC)(void);
// typedef void (*VVENC_BUFFER_FREE)(vvencYUVBuffer*, bool);
// typedef void (*VVENC_BUFFER_DEFAULT)(vvencYUVBuffer*);
// typedef void (*VVENC_ALLOC_BUFFER)(vvencYUVBuffer*, vvencChromaFormat, int, int);
// typedef void (*VVENC_FREE_BUFFER)(vvencYUVBuffer*);

// typedef vvencAccessUnit* (VVENC_ACCESSUNIT_ALLOC)(void);
// typedef void (VVENC_ACCESSUNIT_FREE)(vvencAccessUnit*, bool);


int vvenc_init(H266Config *config) {
    h266_log_debug("vvenc_init called\n");

    bEncodeDone = false;
    enc = vvenc_encoder_create();

    if(enc == NULL) {
        h266_log_error("Cannot create encoder\n");
        return -1;
    }

    vvenc_init_default(&vvenccfg, config->width, \
    config->height, config->framerate, config->bitrate, config->qp, 0);//vvencPresetMode::VVENC_FASTER

    // vvenc_set_msg_callback(&encoder->vvenccfg, NULL, &::msgFnc);  // register local (thread safe) logger (global logger is overwritten )

    //@TODO: Validate the vvenc config here using a configuration parser Ref: parseCfg
    //@TODO: Refine setting bit depth as per the video format
    if(config->format == H266_VIDEO_FORMAT_I420_10LE)
        vvenccfg.m_internChromaFormat = VVENC_CHROMA_420;
    
    vvenccfg.m_inputBitDepth[0] = config->depth[0];
    vvenccfg.m_inputBitDepth[1] = config->depth[1];
    vvenccfg.m_inputBitDepth[2] = config->depth[2];
    vvenccfg.m_verbosity = 5;

    int iRet = vvenc_encoder_open(enc, &vvenccfg);
	if (0 != iRet)
	{
		h266_log_error("vvencapp cannot create encoder, error: %d %d",iRet, vvenc_get_last_error(enc));
		vvenc_encoder_close(enc);
		return -1;
	}

	// get the adapted config
	vvenc_get_config(enc, &vvenccfg);

    vvenc_accessUnit_default(&AU);
	const int auSizeScale = vvenccfg.m_internChromaFormat <= VVENC_CHROMA_420 ? 2 : 3;
	vvenc_accessUnit_alloc_payload(&AU, auSizeScale * vvenccfg.m_SourceWidth * vvenccfg.m_SourceHeight + 1024);

    return 0;
}

int vvenc_start() {    
    return 0;
}

int vvenc_handle(H266Frame *frame) {

  vvencYUVBuffer* ptrYUVInputBuffer = &cYUVInputBuffer;
  H266Status status = H266_SUCCESS;
  if(frame == NULL || frame->silence) {
      ptrYUVInputBuffer = NULL;
      goto encode;
  }

  ptrYUVInputBuffer->ctsValid = frame->ctsValid;

  ptrYUVInputBuffer->planes[0].ptr = (int16_t*)frame->input_planes[0].payload;
  ptrYUVInputBuffer->planes[1].ptr = (int16_t*)frame->input_planes[1].payload;
  ptrYUVInputBuffer->planes[2].ptr = (int16_t*)frame->input_planes[2].payload;

  ptrYUVInputBuffer->planes[0].width = frame->input_planes[0].width;
  ptrYUVInputBuffer->planes[0].height = frame->input_planes[0].height;
  ptrYUVInputBuffer->planes[0].stride = frame->input_planes[0].stride;
  
  ptrYUVInputBuffer->planes[1].width = frame->input_planes[1].width;
  ptrYUVInputBuffer->planes[1].height = frame->input_planes[1].height;
  ptrYUVInputBuffer->planes[1].stride = frame->input_planes[1].stride;
  
  ptrYUVInputBuffer->planes[2].width = frame->input_planes[2].width;
  ptrYUVInputBuffer->planes[2].height = frame->input_planes[2].height;
  ptrYUVInputBuffer->planes[2].stride = frame->input_planes[2].stride;
   
  //ptrYUVInputBuffer->cts = frame->pts;  
  ptrYUVInputBuffer->ctsValid = frame->ctsValid;

encode:
  // // call encode
  int iRet = vvenc_encode(enc, ptrYUVInputBuffer, &AU, &bEncodeDone);
  if (0 != iRet)
  {
    h266_log_error("encode failed %d", iRet);
    vvenc_YUVBuffer_free_buffer(&cYUVInputBuffer);
    vvenc_accessUnit_free_payload(&AU);
    vvenc_encoder_close(enc);
    return -1;
  }

  h266_log_debug("Encoder returns: %d\n", iRet);

  if (AU.payloadUsedSize > 0)
  {
    h266_log_info("\nEncode frame: %d", AU.poc);
    frame->outputPayloadAvailable;
    frame->output_payload = AU.payload;
    frame->outputPayloadSize = AU.payloadUsedSize;
    if(AU.sliceType == VVENC_I_SLICE)
        frame->sliceType = H266_I_S;

    status = H266_PAYLOAD_AVAILABLE;

    frame->dts = AU.dts + 0; //encoder->dts_offset;
    frame->poc = AU.poc;
  }

  if(bEncodeDone == true) {
    status = H266_ENCODING_DONE;
  }

   return status;
}

int vvenc_stop() {
    return 0;
}

int vvenc_flush() {
    return 0;
}

int vvenc_close() {
    h266_log_debug("gst_h266_enc_close_encoder called\n");

    //close encoder here
    int iret = vvenc_encoder_close(enc);
    if (0 != iret)
    {
        h266_log_error("cannot close encoder: %d\n", iret);;
    }

    vvenc_accessUnit_free_payload(&AU);

    h266_log_debug("Encoder closed and access unit freed\n");
    
    return 0;
}