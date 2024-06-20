include(AddCatch2)


add_executable(
        tests
        test.cpp
        other_test.cpp
        oscillators_test.cpp
        another_test.cpp
)
target_link_libraries(
        tests PRIVATE LEAF Catch2::Catch2WithMain
)
#target_link_libraries(tests unit_tests)
#target_link_libraries(tests Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)
