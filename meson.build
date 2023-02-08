project('gst-vvenc-plugin', 'c', version : '1.19.0.1')

# setup project configurations
plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')
cc = meson.get_compiler('c')
gst_version = meson.project_version()
api_version = '1.0'
gst_dep = dependency('gstreamer-1.0', version : '>=1.19', required : true, fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : '>=1.19', fallback : ['gstreamer', 'gst_base_dep'])

# Setup encoder configurations
if get_option('ENCODER_TYPE') == 'VVENC'
encoder_type = 'VVENC_ENCODER'
ENCODER_LIB_PATH=get_option('ENCODER_LIB_PATH')
ENCODER_INCLUDE_PATH=get_option('ENCODER_INCLUDE_PATH')
endif
if get_option('ENCODER_TYPE') == 'UVG'
encoder_type = 'UVG_ENCODER'
ENCODER_LIB_PATH=get_option('ENCODER_LIB_PATH')
ENCODER_INCLUDE_PATH=get_option('ENCODER_INCLUDE_PATH')
endif

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', gst_version)
cdata.set_quoted('PACKAGE', 'gst-h266enc-plugin')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'GStreamer H266 Encoder Plug-ins')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://github.com/InterDigitalInc')
cdata.set_quoted('ENCODER_TYPE', encoder_type)
configure_file(output : 'config.h', configuration : cdata)
configuration_inc = include_directories('.')

subdir('plugin')
if encoder_type == 'VVENC_ENCODER'
  subdir('encoders/vvenc_util')
endif
if encoder_type == 'UVG_ENCODER'
  subdir('encoders/uvg_util')
endif
