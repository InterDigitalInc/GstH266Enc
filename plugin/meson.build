plugin_c_args = ['-DHAVE_CONFIG_H']

vvenc_enc_inc = '../encoders/vvenc_util'
vvenc_common = '../common'

vvenc_enc_dep = declare_dependency(include_directories : [vvenc_enc_inc, vvenc_common, configuration_inc])

gst_version = meson.project_version()
version_arr = gst_version.split('.')
gst_version_major = version_arr[0].to_int()
gst_version_minor = version_arr[1].to_int()
gst_req = '>= @0@.@1@.0'.format(gst_version_major, gst_version_minor)

gstpbutils_dep = dependency('gstreamer-pbutils-1.0', version : gst_req,
    fallback : ['gst-plugins-base', 'pbutils_dep'])

# The h266enc Plugin
 gsth266enc_sources = [
  'src/gsth266enc.c',
  'src/encode_util.c'
  ]

gsth266encexample = library('gsth266enc',
  gsth266enc_sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep, gstbase_dep, gstpbutils_dep, vvenc_enc_dep],
  install : true,
  install_dir : plugins_install_dir,
)
