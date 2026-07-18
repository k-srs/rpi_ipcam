project('http-server', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17',
                     'force_fallback_for=cpputest'])

cpp = meson.get_compiler('cpp')

http_server_dependencies = [
  cpp.find_library('drogon', required : true),
  cpp.find_library('trantor', required : true),
  dependency('jsoncpp'),
  dependency('threads'),
]

config_manager_sources = files(
  'src/config/config_manager.cpp',
)

internal_include_directories = include_directories('src')

exe = executable(
  'http-server',
  'src/main.cpp',
  config_manager_sources,
  include_directories : internal_include_directories,
  dependencies : http_server_dependencies,
  install : true,
)

subdir('tests')
