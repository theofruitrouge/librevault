#============================================================================
# Internal compiler options
#============================================================================

#============================================================================
# Sources & headers
#============================================================================

file(GLOB_RECURSE PROTO_LIST "*.proto")
protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS ${PROTO_LIST})

#============================================================================
# Compile targets
#============================================================================

add_library(protocol-protobuf STATIC ${PROTO_LIST} ${PROTO_SOURCES} ${PROTO_HEADERS})
target_include_directories(protocol-protobuf PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#============================================================================
# Third-party libraries
#============================================================================

target_link_libraries(protocol-protobuf PUBLIC protobuf)
