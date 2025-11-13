include(AddCatch2)


add_executable(
        create_init_test
        create_init_test.cpp
)
target_link_libraries(
        create_init_test PRIVATE LEAF Catch2::Catch2WithMain
)
#target_link_libraries(tests unit_tests)
#target_link_libraries(tests Catch2::Catch2WithMain)

include(CTest)
include(Catch)
## Optional: set Valgrind specific options
#find_program(MEMORYCHECK_COMMAND valgrind) # Locate the Valgrind executable
#if(MEMORYCHECK_COMMAND)
#    set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --show-reachable=yes")
#    # Optional: specify a suppression file for known issues
#    set(MEMORYCHECK_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/valgrind_suppress.txt")
#endif()

#catch_discover_tests(tests)
add_test(NAME create_init_test COMMAND create_init_test)
#add_test(NAME analysis_memchecked_test
#        COMMAND valgrind
#        --error-exitcode=1
#        --tool=memcheck
#        --leak-check=full
#        --errors-for-leak-kinds=definite
#        --show-leak-kinds=definite $<TARGET_FILE:analysis_test>
#        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
add_test(NAME analysis_memchecked_test
        COMMAND env MALLOC_STACK_LOGGING=1
        leaks --atExit -- $<TARGET_FILE:create_init_test>
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})