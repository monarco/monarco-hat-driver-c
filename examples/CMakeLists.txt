# Blink example
add_executable(monarco-blink-demo main-blink-demo.c)
target_link_libraries(monarco-blink-demo monarco_lib m)

# Complex example
add_executable(monarco-complex-demo main-complex-demo.c)
target_link_libraries(monarco-complex-demo monarco_lib m)

target_include_directories(monarco-blink-demo PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(monarco-blink-demo PRIVATE ${PROJECT_SOURCE_DIR}/platform/linux)

target_include_directories(monarco-complex-demo PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(monarco-complex-demo PRIVATE ${PROJECT_SOURCE_DIR}/platform/linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wno-write-strings -fmessage-length=0 -Wno-uninitialized -Werror=uninitialized -Wno-sign-compare -Werror=strict-aliasing -fvisibility=hidden -Wno-maybe-uninitialized -Wno-strict-aliasing")
