#uvg_inc = '../../../../uvg266/src'
#uvg_inc_extras = '../../../../uvg266/src/extras'
#uvg_inc_strategies = '../../../../uvg266/src/strategies'
#uvg_inc_threadwrapper = '../../../../uvg266/src/threadwrapper/include'

uvg_lib = 'uvg266'
uvg_lib_dir = ENCODER_LIB_PATH
uvg_inc_dir = ENCODER_INCLUDE_PATH

uvg_util_inc = '.'
h266_common_inc = '../../../common'

uvg_library = static_library(uvg_lib_dir + uvg_lib, name_prefix : '', name_suffix : 'lib')
uvg_dependency = declare_dependency(link_with : [uvg_library], include_directories : [uvg_util_inc, h266_common_inc, uvg_inc_dir, configuration_inc])

# The h266enc Plugin
 uvg_util_sources = [
  'uvg_util.c',
  '../../common/log.c',
  ]

vvvenc_util_project = shared_library('uvg_lib',
  uvg_util_sources,
  c_args: plugin_c_args,
  dependencies : [uvg_dependency],
  install : true,
)
