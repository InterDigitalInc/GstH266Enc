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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gsth266enc.h"

#include <gst/pbutils/pbutils.h>
#include <gst/video/video.h>
#include <gst/video/gstvideometa.h>
#include <gst/video/gstvideopool.h>

#include "encode_util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

GST_DEBUG_CATEGORY_STATIC (gst_h266enc_debug);
#define GST_CAT_DEFAULT gst_h266enc_debug

enum
{
  PROP_0,
  PROP_SILENT,
  PROP_BITRATE,
  PROP_QP
};

#define PROP_BITRATE_DEFAULT            (10 * 1000000)
#define PROP_QP_DEFAULT                 32

static int video_width = 0;
static int video_height = 0;
static int video_frame_rate = 0;

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-h266, "
        "framerate = (fraction) [0/1, MAX], "
        "width = (int) [ 16, MAX ], " "height = (int) [ 16, MAX ], "
        "stream-format = (string) byte-stream, "
        "alignment = (string) au, "
        "profile = (string) { main-10 }") //Main 10 profile: monochrome and 4:2:0, bit depth of 8 to 10 bits, 1 layer only
    );

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static gboolean gst_h266_enc_stop (GstVideoEncoder * encoder);
static gboolean gst_h266_enc_init_encoder (Gsth266enc * encoder);
static gboolean gst_h266_enc_set_src_caps(Gsth266enc * encoder, GstCaps * caps);
static void gst_h266_enc_close_encoder (Gsth266enc * encoder);

static GstFlowReturn gst_h266_enc_finish (GstVideoEncoder * encoder);
static GstFlowReturn gst_h266_enc_handle_frame (GstVideoEncoder * encoder,
    GstVideoCodecFrame * frame);

static gboolean gst_h266_enc_set_format (GstVideoEncoder * video_enc,
    GstVideoCodecState * state);

static gboolean h266enc_element_init (GstPlugin * plugin);

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
#define gst_h266enc_parent_class parent_class
G_DEFINE_TYPE (Gsth266enc, gst_h266enc, GST_TYPE_VIDEO_ENCODER);

GST_ELEMENT_REGISTER_DEFINE (h266enc, "h266enc", GST_RANK_NONE,
    GST_TYPE_H266ENC);

static void gst_h266enc_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_h266enc_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_h266enc_sink_event (GstPad * pad,
    GstObject * parent, GstEvent * event);


// Set the input chroma format to I420 and I420_10LE
// TODO: get the supported chroma format from the encoder library
static void 
gst_h266enc_set_input_chroma_format(GstStructure *gststructure)
{  
  GValue formats = G_VALUE_INIT;
  GValue format = G_VALUE_INIT;  

  g_value_init (&formats, GST_TYPE_LIST);
  g_value_init (&format, G_TYPE_STRING);

  g_value_set_string (&format, "I420");
  gst_value_list_append_value (&formats, &format);

  g_value_set_string (&format, "I420_10LE");
  gst_value_list_append_value (&formats, &format);

  if (gst_value_list_get_size (&formats) != 0) 
  {
    gst_structure_take_value (gststructure, "format", &formats);    
  }  
}

/* GObject vmethod implementations */

/* initialize the h266enc's class */
static void
gst_h266enc_class_init (Gsth266encClass * klass)
{
    GST_DEBUG_CATEGORY_INIT (gst_h266enc_debug, "gst_h266enc",
      0, "Template h266enc");

  GST_INFO ("Inside gst_h266enc_class_init.");

  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  GstVideoEncoderClass *gstencoder_class;
  GstPadTemplate *sink_templ;
  GstCaps *supported_sinkcaps;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = GST_ELEMENT_CLASS (klass);
  gstencoder_class = GST_VIDEO_ENCODER_CLASS(klass);

  gst_element_class_set_details_simple (gstelement_class,
      "h266enc",
      "FIXME:Generic",
      "FIXME:Generic Template Element", " <>");

  gst_element_class_set_static_metadata (gstelement_class,
    "h266enc", "Codec/Encoder/Video", "H266 Encoder",
    "Advaiit Rajjvaed <adwait.sambare@interdigital.com>");

  gobject_class->set_property = gst_h266enc_set_property;
  gobject_class->get_property = gst_h266enc_get_property;

  gstencoder_class->set_format = GST_DEBUG_FUNCPTR (gst_h266_enc_set_format);
  gstencoder_class->handle_frame = GST_DEBUG_FUNCPTR (gst_h266_enc_handle_frame);
  gstencoder_class->stop = GST_DEBUG_FUNCPTR (gst_h266_enc_stop);
  gstencoder_class->finish = GST_DEBUG_FUNCPTR (gst_h266_enc_finish);

  g_object_class_install_property (gobject_class, PROP_BITRATE,
      g_param_spec_uint ("bitrate", "Bitrate", "Bitrate in kbit/sec", 1,
          100000 * 1024, PROP_BITRATE_DEFAULT,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_PLAYING));

  g_object_class_install_property (gobject_class, PROP_QP,
      g_param_spec_int ("qp", "Quantization parameter",
          "QP for P slices in (implied) CQP mode (-1 = disabled)", -1,
          51, PROP_QP_DEFAULT, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  supported_sinkcaps = gst_caps_new_simple ("video/x-raw",
    "framerate", GST_TYPE_FRACTION_RANGE, 0, 1, G_MAXINT, 1,
    "width", GST_TYPE_INT_RANGE, 1, G_MAXINT,
    "height", GST_TYPE_INT_RANGE, 1, G_MAXINT,
    "format", G_TYPE_STRING, "I420",
    NULL);

  gst_h266enc_set_input_chroma_format(gst_caps_get_structure(supported_sinkcaps, 0));

  sink_templ = gst_pad_template_new ("sink",
    GST_PAD_SINK, GST_PAD_ALWAYS, supported_sinkcaps);
  gst_caps_unref (supported_sinkcaps);
  gst_element_class_add_pad_template (gstelement_class, sink_templ);
  gst_element_class_add_static_pad_template (gstelement_class, &src_factory);

  GST_INFO("Inside gst_h266enc_class_init done.");
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad callback functions
 * initialize instance structure
 * @TODO: H266 initialization code here
 */
static void
gst_h266enc_init (Gsth266enc * encoder)
{
  encoder->qp = PROP_QP_DEFAULT;
  encoder->bitrate = PROP_BITRATE_DEFAULT;
}

static gboolean
gst_h266_enc_stop (GstVideoEncoder * encoder)
{
  GST_INFO ("gst_h266_enc_stop called");

  //stop, flush and close h266 encoder here
  gst_h266_enc_close_encoder(GST_H266ENC(encoder));

  return TRUE;
}


/*
 * gst_h266_enc_init_encoder
 * @encoder:  Encoder which should be initialized.
 *
 * Initialize h266 encoder.
 *
 */
static gboolean
gst_h266_enc_init_encoder (Gsth266enc * encoder)
{
  GST_INFO ("gst_h266_enc_init_encoder called"); 

  H266Config h266Config;
  H266Status status;
  int frame_rate = encoder->input_state->info.fps_n / encoder->input_state->info.fps_d;
  h266Config.framerate = frame_rate;
  h266Config.width = encoder->input_state->info.width;
  h266Config.height = encoder->input_state->info.height;
  h266Config.bitrate = encoder->bitrate;
  h266Config.qp = encoder->qp;
  h266Config.depth[0] = encoder->input_state->info.finfo->depth[0];
  h266Config.depth[1] = encoder->input_state->info.finfo->depth[1];
  h266Config.depth[2] = encoder->input_state->info.finfo->depth[2];
  if(encoder->input_state->info.finfo->format == GST_VIDEO_FORMAT_I420_10LE)
    h266Config.format = H266_VIDEO_FORMAT_I420_10LE;
  if( strcmp(ENCODER_TYPE, "VVENC_ENCODER") == 0) {
    status = initEncoder(VVENC_ENCODER, &h266Config);
  }
  else if (strcmp(ENCODER_TYPE, "UVG_ENCODER") == 0) {
    status = initEncoder(UVG_ENCODER, &h266Config);
  }
  else {
    status = H266_ERR;
  }
  if(status != H266_SUCCESS) {
    GST_ERROR("initEncoder failed with %d", status);
    return FALSE;
  }

  return TRUE;
}

/* gst_h266_enc_close_encoder
 * @encoder:  Encoder which should close.
 *
 * Close h266 encoder.
 */
static void
gst_h266_enc_close_encoder (Gsth266enc * encoder)
{
  GST_INFO("gst_h266_enc_close_encoder called\n");

  //close encoder here

  H266Status status = closeEncoder();
  if(status != H266_SUCCESS) {
    GST_ERROR("Error closing encoder: %d", status);
  }

  GST_INFO("Encoder closed and access unit freed\n");
}

static gboolean gst_h266_enc_set_src_caps(Gsth266enc * encoder, GstCaps * caps)
{
  GST_INFO("gst_h266_enc_set_src_caps called");

  //set src caps here
  GstCaps *outcaps;
  GstStructure *structure;
  GstVideoCodecState *state;
  outcaps = gst_caps_new_empty_simple ("video/x-h266");
  structure = gst_caps_get_structure (outcaps, 0);

  gst_structure_set (structure, "stream-format", G_TYPE_STRING, "byte-stream",
      NULL);
  gst_structure_set (structure, "alignment", G_TYPE_STRING, "au", NULL);

  gst_structure_set(structure, "profile", G_TYPE_STRING, "main-10", NULL);
  state = gst_video_encoder_set_output_state (GST_VIDEO_ENCODER (encoder),
      outcaps, encoder->input_state);

  GST_DEBUG_OBJECT (encoder, "output caps: %" GST_PTR_FORMAT, state->caps);
  gst_video_codec_state_unref (state);

  return TRUE;
}

static GstFlowReturn
gst_h266_enc_finish (GstVideoEncoder * encoder)
{
  GST_INFO("gst_h266_enc_finish called");
    //flush the encoder here
  Gsth266enc *enc = GST_H266ENC(encoder);
  H266Frame h266_frame;

  h266_frame.silence = true;
  h266_frame.outputPayloadAvailable = false;
  H266Status status = H266_SUCCESS;

  do {
    status = encodeFrame(&h266_frame);
  }while(status != H266_ENCODING_DONE);

  GST_INFO("Done flushing frames");

  return GST_FLOW_OK;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_h266_enc_handle_frame (GstVideoEncoder * video_enc,
    GstVideoCodecFrame * frame)
{
  GST_INFO("gst_h266_enc_handle_frame called");

  static int poc = 0;
  Gsth266enc *encoder = GST_H266ENC(video_enc);
  GstBuffer *gInputBuffer = frame->input_buffer;
  GstMapInfo info;

  gst_buffer_map(gInputBuffer, &info, GST_MAP_READ);

  int iSizeComponent0 = video_width * video_height * 2;
  int iSizeComponent1 = video_width * video_height / 2;
  int iSizeComponent2 = video_width * video_height / 2;

  H266Frame h266_frame;

  h266_frame.input_planes[0].payload = info.data;
  h266_frame.input_planes[1].payload = info.data + iSizeComponent0;
  h266_frame.input_planes[2].payload = info.data + iSizeComponent0 + iSizeComponent1;

  h266_frame.input_planes[0].payloadSize = iSizeComponent0;
  h266_frame.input_planes[1].payloadSize = iSizeComponent1;
  h266_frame.input_planes[2].payloadSize = iSizeComponent2;

  h266_frame.input_planes[0].width = video_width;
  h266_frame.input_planes[0].height = video_height;
  h266_frame.input_planes[0].stride = video_width;

  h266_frame.input_planes[1].width = video_width >> 1;
  h266_frame.input_planes[1].height = video_height >> 1;
  h266_frame.input_planes[1].stride = video_width >> 1;

  h266_frame.input_planes[2].width = video_width >> 1;
  h266_frame.input_planes[2].height = video_height >> 1;
  h266_frame.input_planes[2].stride = video_width >> 1;

  h266_frame.ctsValid = false;
  h266_frame.silence = false;
  h266_frame.outputPayloadAvailable = false;

  H266Status status = encodeFrame(&h266_frame);

  if (frame)
  {
    gst_video_codec_frame_unref(frame);
  }

  if(status == H266_ERR) {
    GST_ERROR("encode Frame failed, error: %d", status); 
  }
  else if(status == H266_PAYLOAD_AVAILABLE) {
    poc++;
    GST_INFO("Payload available, poc: %d", poc);
    GstVideoCodecFrame *opframe = NULL;
    opframe = gst_video_encoder_get_frame (GST_VIDEO_ENCODER (encoder),
     GPOINTER_TO_INT (h266_frame.poc));

    if(opframe == NULL) {
      opframe = (GstVideoCodecFrame *)calloc(1, sizeof(GstVideoCodecFrame));
    }

    g_assert (opframe);

    int i_size = 0;
    int offset = 0;
    GstBuffer *out_buf = NULL;
    out_buf = gst_buffer_new_allocate (NULL, h266_frame.outputPayloadSize, NULL);
    gst_buffer_fill (out_buf, offset, h266_frame.output_payload, h266_frame.outputPayloadSize);
    opframe->output_buffer = out_buf;

    if (h266_frame.sliceType == H266_I_S) {
     GST_VIDEO_CODEC_FRAME_SET_SYNC_POINT (opframe);
    }

    opframe->dts = h266_frame.dts + 0; //encoder->dts_offset;

    if (opframe) {   
      int ret = gst_video_encoder_finish_frame (video_enc, opframe);
      if(ret != 0) {
        GST_ERROR("error in finish frame returns: %d", ret);
      }
    }
  }

  return GST_FLOW_OK;
}

static void
gst_h266enc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GST_INFO("gst_h266enc_set_property called");

  Gsth266enc *encoder = GST_H266ENC (object);

  switch (prop_id) {
    case PROP_SILENT:
      encoder->silent = g_value_get_boolean (value);
      break;
    case PROP_BITRATE:
      encoder->bitrate = g_value_get_uint (value);
      break;
    case PROP_QP:
      encoder->qp = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_h266enc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GST_INFO("gst_h266enc_get_property called");

  Gsth266enc *encoder = GST_H266ENC (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, encoder->silent);
      break;
    case PROP_BITRATE:
      g_value_set_uint (value, encoder->bitrate);
      break;
    case PROP_QP:
      g_value_set_int (value, encoder->qp);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_h266enc_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GST_INFO("gst_h266enc_sink_event called");

  Gsth266enc *encoder;
  gboolean ret;

  encoder = GST_H266ENC (parent);

  GST_LOG_OBJECT (encoder, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */

      /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

static gboolean gst_h266_enc_set_format (GstVideoEncoder * video_enc,
    GstVideoCodecState * state) {

  GST_INFO("gst_h266_enc_set_format called");
  Gsth266enc *encoder = GST_H266ENC (video_enc);
  encoder->input_state = gst_video_codec_state_ref (state);

  if (encoder->input_state)
    gst_video_codec_state_unref (encoder->input_state);
  
  encoder->input_state = gst_video_codec_state_ref (state);

  video_width = encoder->input_state->info.width;
  video_height = encoder->input_state->info.height;

  if(!gst_h266_enc_init_encoder(encoder)) {
    gst_h266_enc_close_encoder (encoder);
    return FALSE;
  }

  if (!gst_h266_enc_set_src_caps (encoder, state->caps)) {
    gst_h266_enc_close_encoder (encoder);
    return FALSE;
  }

  return TRUE;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
h266enc_init (GstPlugin * h266enc)
{
  return gst_element_register (h266enc, "h266enc",
      GST_RANK_PRIMARY, GST_TYPE_H266ENC);
}

#ifndef PACKAGE
#define PACKAGE "gst-h266enc-plugin"
#endif

static gboolean
plugin_init (GstPlugin * plugin)
{
  return GST_ELEMENT_REGISTER (h266enc, plugin);
}

// gstreamer looks for this structure to register h266enc
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    h266enc,
    "A GStreamer plugin for VVC Encoder",
    plugin_init,
    PACKAGE_VERSION, GST_LICENSE_UNKNOWN, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
