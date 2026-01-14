find_package(Boost REQUIRED COMPONENTS program_options)



set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(HEADERS)

add_executable(sndEventFilter sndEventFilter.cxx)
target_link_libraries(sndEventFilter ${ROOT_LIBRARIES} snd_analysis_cuts Boost::program_options)