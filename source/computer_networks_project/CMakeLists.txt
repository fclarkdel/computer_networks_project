add_library(
        computer_networks_project_library
        bridge/bridge.cpp
        bridge/bridge.hpp
)
if (PROJECT_IS_TOP_LEVEL)
    add_executable(
            computer_networks_project_tests
            bridge/bridge.test.cpp
    )
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    target_link_libraries(
            computer_networks_project_tests
            PRIVATE
            fluent_curl_library
            gmock_main
    )
    enable_testing()
endif ()