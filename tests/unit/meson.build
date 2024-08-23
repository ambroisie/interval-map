gtest_dep = dependency(
  'gtest',
  main : true,
  required : false,
)

dummy_test = executable(
  'unit_test',
  'unit_test.cc',
  dependencies : [
    gtest_dep,
    interval_map_dep
  ],
)

test('unit test', dummy_test)
