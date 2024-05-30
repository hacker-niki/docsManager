cmake_minimum_required(VERSION 3.5)
project(docsManagerLib)

set(CMAKE_CXX_STANDARD 23)

set(THIRD_PARTY_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/thirdparty/pugixml/pugixml.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/thirdparty/pugixml/pugiconfig.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/thirdparty/zip/zip.h")
set(THIRD_PARTY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/thirdparty/zip/zip.c")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/DuckX/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/thirdparty/pugixml"
        "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/thirdparty/zip")

add_library(duckx STATIC ${SOURCES} ${THIRD_PARTY_SRC})
add_library(duckx::duckx ALIAS duckx)

set(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/include/duckx.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/include/constants.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/DuckX/include/duckxiterator.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Entities/Document.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Interfaces/IDocument.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Interfaces/IGenerator.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Interfaces/IRepository.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Manager.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Generator.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Entities/Template.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Repository/TemplateRepository.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Entities/Field.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/FileManager.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Repository/DocumentRepository.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Repository/RepMaster.h")

add_library(sqlite STATIC sqlite/sqlite3.c)

add_library(docsManagerLib STATIC Entities/Document.cpp
        Manager.cpp
        Generator.cpp
        Entities/Template.cpp
        Repository/TemplateRepository.cpp
        FileManager.cpp
        Repository/DocumentRepository.cpp
        Repository/RepMaster.cpp
)
#target_link_libraries(docsManagerLib PUBLIC )

target_link_libraries(docsManagerLib duckx::duckx sqlite)

#find_package(Qt6 REQUIRED COMPONENTS Widgets)
#
#qt_standard_project_setup()
#
#qt_add_executable(docsManager main.cpp)
#
#target_link_libraries(docsManager PRIVATE Qt6::Widgets)
#
#target_link_libraries(docsManager LINK_PUBLIC docsManagerLib)
#
#set_target_properties(docsManager PROPERTIES
#        WIN32_EXECUTABLE ON
#        MACOSX_BUNDLE ON
#)