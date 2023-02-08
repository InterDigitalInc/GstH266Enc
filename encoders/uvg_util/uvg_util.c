
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "uvg_util.h"

#include "uvg266.h"
#include "uvg266_internal.h"


static uvg_api* api = NULL;
static uvg_config* config = NULL;
static uvg_encoder* enc = NULL;
uint8_t* output_payload = NULL;
static uvg_data_chunk* chunks_out = NULL;
static uvg_picture *img_rec = NULL;
static uvg_picture *img_src = NULL;

static unsigned long pts_util = 0;

int uvg_init(H266Config *h266_config) {

    //get api here
    api = uvg_api_get(h266_config->depth[0]);
    if(!api) {
        h266_log_error("Error in getting uvg api");
        return -1;
    }

    //init config here
    config = api->config_alloc();
    if(!config || !api->config_init(config)) {
        h266_log_error("Error in config init for uvg encoder");
        return -1;
    }

    if (config->vps_period < 0) {
    // Disabling parameter sets is only possible when using uvg266 as
    // a library.
        h266_log_error("Error in vps_period of config");
        return -1;
    }

    //set other important parameters here
    config->width = h266_config->width;
    config->height = h266_config->height;
    config->qp = h266_config->qp;
    config->framerate_num = h266_config->framerate;
    config->framerate_denom = 1;
    config->target_bitrate = h266_config->bitrate; //On setting this gives out error, "Bitrate set but rc-algorithm is turned off" if rc_algorithm not set
    config->input_bitdepth = h266_config->depth[0];
    config->file_format = UVG_FORMAT_YUV;

    //not sure what this is
    config->rc_algorithm = UVG_OBA;

    if(h266_config->format == H266_VIDEO_FORMAT_I420 || h266_config->format == H266_VIDEO_FORMAT_I420_10LE) {
        config->input_format = UVG_FORMAT_P420;
    }

    //open encoder here
    enc = api->encoder_open(config);
    if (!enc) {
        h266_log_error("Failed to open encoder");
        return -1;
    }

    h266_log_debug("uvg encoder opened\n");

    return 0;
}

int uvg_start() {    
    return 0;
}

int uvg_handle(H266Frame *frame) {
    //point the frame handles her
    uint32_t len_out = 0;
    uvg_frame_info info_out;
    H266Status status = H266_SUCCESS;

    //allocate memory for cur img here
    uvg_picture* cur_in_img = api->picture_alloc_csp(UVG_CSP_420, config->width, config->height);

    if(cur_in_img == NULL) {
        h266_log_error("Cannot allocate cur image");
        return H266_ERR;
    }

    //populate the frame pointers here
    memcpy(cur_in_img->y, frame->input_planes[0].payload, frame->input_planes[0].payloadSize);
    memcpy(cur_in_img->u, frame->input_planes[1].payload, frame->input_planes[1].payloadSize);
    memcpy(cur_in_img->v, frame->input_planes[2].payload, frame->input_planes[2].payloadSize);

    cur_in_img->pts = pts_util++;

    if (!api->encoder_encode(enc,
                            cur_in_img,
                            &chunks_out,
                            &len_out,
                            &img_rec,
                            &img_src,
                            &info_out)) {
        h266_log_error("Failed to encode image.");
        api->picture_free(cur_in_img);
        return H266_ERR;
    }

    if (chunks_out == NULL && cur_in_img == NULL) {
        // We are done since there is no more input and output left.
        return H266_ENCODING_DONE;
    }

    if (chunks_out != NULL) {

        h266_log_debug("UVG Payload Available");

        if(output_payload) {
            free(output_payload);
            output_payload = NULL;
        }

        output_payload = (uint8_t*)calloc(len_out, sizeof(uint8_t));

        uint64_t written = 0;
        // Write data into the output file.
        for (uvg_data_chunk *chunk = chunks_out; chunk != NULL; chunk = chunk->next) {
            assert(written + chunk->len <= len_out);
            memcpy(output_payload + written, chunk->data, chunk->len);
            written += chunk->len;
        }

        frame->output_payload = output_payload;
        frame->outputPayloadSize = len_out;
        frame->outputPayloadAvailable = true;

        status = H266_PAYLOAD_AVAILABLE;
    }

    api->picture_free(cur_in_img);
    api->chunk_free(chunks_out);
    api->picture_free(img_rec);
    api->picture_free(img_src);

    return status;
}

int uvg_stop() {
    return 0;
}

int uvg_flush() {
    return 0;
}

int uvg_close() {
    api->chunk_free(chunks_out);
    api->picture_free(img_rec);
    api->picture_free(img_src);

    if(output_payload) {
        free(output_payload);
        output_payload = NULL;
    }

    if (enc) api->encoder_close(enc);

    return 0;
}