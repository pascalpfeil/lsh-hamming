include(googletest)
include(GoogleTest)

file(GLOB TEST_CPP "${PROJECT_SOURCE_DIR}/test/*_test.cpp")

add_executable(lsh_tests test/tests.cpp ${TEST_CPP})
target_link_libraries(lsh_tests lsh gtest gmock)

gtest_discover_tests(lsh_tests)

enable_testing()