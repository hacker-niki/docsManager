find_package(SQLite3 3.38 REQUIRED)
find_package(LibXml2 REQUIRED)
if(NOT TARGET SQLite3::SQLite3)
    add_library(SQLite3::SQLite3 ALIAS SQLite::SQLite3)
endif()

add_library(docsManagerLib STATIC
    Manager.cpp Generator.cpp DocxPackage.cpp FileManager.cpp
    Entities/Document.cpp Entities/Template.cpp Entities/RecordJson.cpp
    Repository/DocumentRepository.cpp Repository/TemplateRepository.cpp
    Repository/RepMaster.cpp
)
target_include_directories(docsManagerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(docsManagerLib
    PUBLIC Qt6::Core SQLite3::SQLite3
    PRIVATE LibXml2::LibXml2 PkgConfig::LIBZIP
)
