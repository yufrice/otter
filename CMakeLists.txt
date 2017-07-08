project (otter)
cmake_minimum_required(VERSION 3.7)

add_subdirectory(otterc)
add_subdirectory(test)

enable_testing()
add_test(NAME OtterTest COMMAND test)
