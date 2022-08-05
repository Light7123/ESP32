# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/esp-idf/examples/common_components/protocol_examples_common)

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS "."
EMBED_TXTFILES web/index.html)



target_add_binary_data(${COMPONENT_TARGET} "chart.html" TEXT)