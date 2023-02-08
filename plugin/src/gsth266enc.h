/*
 * GStreamer Plugin for VVC
 * Copyright (C) 2022 Advaiit Sambare <adwait.sambare@interdigital.com>
 * Copyright (C) 2022 Saurabh Puri <saurabh.puri@interdigital.com> 
 */

#ifndef __GST_H266ENC_H__
#define __GST_H266ENC_H__

#include <gst/gst.h>
#include <stdbool.h>
#include <gst/video/video.h>
#include <gst/video/gstvideoencoder.h>

G_BEGIN_DECLS

#define GST_TYPE_H266ENC \
  (gst_h266enc_get_type())
#define GST_H266ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_H266ENC,Gsth266enc))
#define GST_H266ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_H266ENC,Gsth266encClass))
#define GST_IS_H266ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_H266ENC))
#define GST_IS_H266ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_H266ENC))

typedef struct _Gsth266encClass Gsth266encClass;
typedef struct _Gsth266enc Gsth266enc;

struct _Gsth266enc
{
  GstVideoEncoder element;

  gboolean silent;

  bool bEncodeDone;

  guint bitrate;
  gint qp;
  gint logLevel;

  /* input description */
  GstVideoCodecState *input_state;
};



struct _Gsth266encClass
{
  GstVideoEncoderClass parent_class;
};

GType gst_h266enc_get_type (void);

GST_ELEMENT_REGISTER_DECLARE (h266enc);

G_END_DECLS

#endif /* __GST_H266ENC_H__ */
