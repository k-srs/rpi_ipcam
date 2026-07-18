project('http-server', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

exe = executable('http-server', 'http_server.cpp',
  install : true)

test('basic', exe)
