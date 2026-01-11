find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(sndEventFilter sndEventFilter.cxx)

target_link_libraries(sndEventFilter
    ${ROOT_LIBRARIES}
    Boost::program_options
)