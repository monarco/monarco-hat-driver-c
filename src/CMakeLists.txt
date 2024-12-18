# Find all source files
file(GLOB LIB_SOURCES "*.c")
file(GLOB LIB_HEADERS "*.h")

# Define the library
add_library(monarco_lib ${LIB_SOURCES} ${LIB_HEADERS})

# Include paths for the library
target_include_directories(monarco_lib PRIVATE ${PROJECT_SOURCE_DIR}/platform/linux)