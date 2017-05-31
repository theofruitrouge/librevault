#============================================================================
# Sources & headers
#============================================================================

file(GLOB_RECURSE snapshot_SOURCES "*.cpp")
file(GLOB_RECURSE snapshot_HEADERS "*.h")

#============================================================================
# Compile targets
#============================================================================

add_library(snapshot STATIC ${snapshot_SOURCES} ${snapshot_HEADERS})
target_include_directories(snapshot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#============================================================================
# Third-party libraries
#============================================================================

target_link_libraries(snapshot PRIVATE aes)
target_link_libraries(snapshot PRIVATE secret)
target_link_libraries(snapshot PUBLIC protocol-protobuf)
target_link_libraries(snapshot PRIVATE Qt5::Core)
target_link_libraries(snapshot PRIVATE cryptopp)
