vvenc_lib = 'vvenc'
vvenc_inc_dir = ENCODER_INCLUDE_PATH
vvenc_lib_dir = ENCODER_LIB_PATH

apputils_lib = 'apputils'
apputils_lib_dir = ENCODER_LIB_PATH

vvenc_util_inc = '.'
h266_common_inc = '../../../common'

vvenc_library = static_library(vvenc_lib_dir + vvenc_lib, name_prefix : '', name_suffix : 'lib')
apputils_library = static_library(apputils_lib_dir + apputils_lib, name_prefix : '', name_suffix : 'lib')

vvenc_dependency = declare_dependency(link_with: [vvenc_library, apputils_library], 
include_directories : [vvenc_util_inc, h266_common_inc, vvenc_inc_dir])

# The h266enc Plugin
 vvenc_util_sources = [
  './vvenc_util.c',
  '../../common/log.c',
  ]

vvvenc_util_project = shared_library('vvenc_lib',
  vvenc_util_sources,
  c_args: plugin_c_args,
  dependencies : [vvenc_dependency],
  install : true,
)
