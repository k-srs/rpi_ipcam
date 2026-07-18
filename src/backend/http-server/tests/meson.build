cpputest_dep = dependency(
  'cpputest',
  fallback : ['cpputest', 'cpputest_dep'],
)

http_server_tests = executable(
  'http-server-tests',
  'cpputest_integration_test.cpp',
  'config_manager_test.cpp',
  'test_main.cpp',
  dependencies : cpputest_dep,
)

test('http-server-tests', http_server_tests)
