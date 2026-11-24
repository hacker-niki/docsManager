foreach(suite IN ITEMS storage generation manager)
    add_executable(${suite}_tests ${suite}_tests.cpp)
    target_link_libraries(${suite}_tests PRIVATE docsManagerLib)
    add_test(NAME ${suite} COMMAND ${suite}_tests)
endforeach()

target_link_libraries(generation_tests PRIVATE PkgConfig::LIBZIP)
target_link_libraries(manager_tests PRIVATE PkgConfig::LIBZIP)

add_executable(desktop_tests desktop_tests.cpp)
target_link_libraries(desktop_tests PRIVATE docsManagerUi PkgConfig::LIBZIP)
add_test(NAME desktop COMMAND desktop_tests)
add_test(NAME startup_help COMMAND docsManager --help)
add_test(NAME startup_version COMMAND docsManager --version)
set_tests_properties(desktop startup_help startup_version PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 30
)
set_tests_properties(startup_help PROPERTIES PASS_REGULAR_EXPRESSION "data-dir")
set_tests_properties(startup_version PROPERTIES PASS_REGULAR_EXPRESSION "${PROJECT_VERSION}")

add_executable(document_window_tests document_window_tests.cpp)
set_target_properties(document_window_tests PROPERTIES AUTOMOC ON)
target_link_libraries(document_window_tests PRIVATE docsManagerUi)
add_test(NAME document_window COMMAND document_window_tests)
set_tests_properties(document_window PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 30)

add_executable(startup_tests startup_tests.cpp)
target_link_libraries(startup_tests PRIVATE Qt6::Core)
add_test(NAME library_process COMMAND startup_tests $<TARGET_FILE:docsManager>)
set_tests_properties(library_process PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 30)
