add_library(
        computer_networks_project_library
        bridge/bridge.cpp
        bridge/bridge.hpp
        link/link.cpp
        link/link.hpp
        packet/packet.cpp
        packet/packet.hpp
        utils/utils.cpp
        utils/utils.hpp
)
target_include_directories(
        computer_networks_project_library
        PUBLIC
        ../computer_networks_project
)
add_executable(
        bridge
        cli/bridge.cpp
        cli/bridge.hpp
)
target_link_libraries(
        bridge
        PRIVATE
        computer_networks_project_library
)
if (PROJECT_IS_TOP_LEVEL)
    add_executable(
            computer_networks_project_tests
            bridge/bridge.test.cpp
            link/link.test.cpp
            packet/packet.test.cpp
    )
    include(FetchContent)

    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(
            computer_networks_project_tests
            PRIVATE
            computer_networks_project_library
            gmock_main
    )
    enable_testing()
endif ()
