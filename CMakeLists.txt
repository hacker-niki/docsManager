cmake_minimum_required(VERSION 3.22)
project(docsManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)
include(CTest)

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP REQUIRED IMPORTED_TARGET libzip)
add_subdirectory(Manager)

add_library(docsManagerUi STATIC
    app/MainWindow.cpp app/MainWindow.ui
    app/InfoWindow.cpp app/DocumentWindow.cpp
)
set_target_properties(docsManagerUi PROPERTIES AUTOMOC ON AUTOUIC ON)
target_include_directories(docsManagerUi PUBLIC app)
target_link_libraries(docsManagerUi PUBLIC docsManagerLib Qt6::Widgets)

add_executable(docsManager app/main.cpp)
target_link_libraries(docsManager PRIVATE docsManagerUi)
target_compile_definitions(docsManager PRIVATE DOCSMANAGER_VERSION="${PROJECT_VERSION}")

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS docsManager RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
if(UNIX AND NOT APPLE)
    install(FILES packaging/docsManager.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()
install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

set(CPACK_PACKAGE_NAME "docsmanager")
set(CPACK_PACKAGE_VENDOR "Nikita Senko")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Local document library and DOCX template generator")
set(CPACK_PACKAGE_CONTACT "Nikita Senko <62511860+hacker-niki@users.noreply.github.com>")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/hacker-niki/docsManager")
set(CPACK_GENERATOR "TGZ")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt6-qpa-plugins, libsqlite3-0 (>= 3.38.0)")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "qt6-translations-l10n")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
include(CPack)
