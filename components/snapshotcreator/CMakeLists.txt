#============================================================================
# Internal compiler options
#============================================================================

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#============================================================================
# Sources & headers
#============================================================================

file(GLOB_RECURSE snapshot_SOURCES "*.cpp")
file(GLOB_RECURSE snapshot_HEADERS "*.h")

#============================================================================
# Compile targets
#============================================================================

add_library(snapshotcreator STATIC ${snapshot_SOURCES} ${snapshot_HEADERS})
target_include_directories(snapshotcreator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(snapshotcreator_exe null.cpp)

#============================================================================
# Third-party libraries
#============================================================================

target_link_libraries(snapshotcreator PRIVATE aes)
target_link_libraries(snapshotcreator PRIVATE secret)
target_link_libraries(snapshotcreator PRIVATE Qt5::Core)
target_link_libraries(snapshotcreator PRIVATE cryptopp)
target_link_libraries(snapshotcreator PRIVATE rabin)

target_link_libraries(snapshotcreator PRIVATE pathnormalizer)

target_link_libraries(snapshotcreator PUBLIC snapshot)
target_link_libraries(snapshotcreator PUBLIC Boost::system)
target_link_libraries(snapshotcreator PUBLIC Boost::filesystem)

target_link_libraries(snapshotcreator_exe snapshotcreator)
