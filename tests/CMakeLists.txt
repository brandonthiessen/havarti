include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(havarti_tests
    test_orderbook.cpp
    test_price_levels.cpp
    test_ring_buffer.cpp
)

target_link_libraries(havarti_tests
    PRIVATE
        havarti_core
        havarti_support
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(havarti_tests)
