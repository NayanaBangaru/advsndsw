set(INCLUDE_DIRECTORIES
    ${BASE_INCLUDE_DIRECTORIES} ${VMC_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/shipdata ${CMAKE_SOURCE_DIR}/shipLHC
    ${CMAKE_SOURCE_DIR}/analysis/core
    ${CMAKE_SOURCE_DIR}/analysis/tools)

include_directories(${INCLUDE_DIRECTORIES})

add_subdirectory(core)
add_subdirectory(tools)

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR})

link_directories(${LINK_DIRECTORIES})
