cmake_minimum_required(VERSION 3.5)
project(docsManager)

set(CMAKE_CXX_STANDARD 23)

add_executable(docsManager main.cpp)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_library(lib STATIC Entities/Document.cpp
        Entities/Document.h
        Interfaces/IDocument.h
        Interfaces/IGenerator.h
        Interfaces/IRepository.h
        Manager.cpp
        Manager.h
        Generator.cpp
        Generator.h
        Entities/Template.cpp
        Entities/Template.h
        Repository/TemplateRepository.cpp
        Repository/TemplateRepository.h
        Entities/Field.h
        FileManager.cpp
        FileManager.h
        Repository/DocumentRepository.cpp
        Repository/DocumentRepository.h
        Repository/RepMaster.cpp
        Repository/RepMaster.h
        Interfaces/ISerializer.h)

#target_compile_definitions(lib PUBLIC lib)

find_package(duckx CONFIG REQUIRED)

target_link_libraries(lib PUBLIC duckx::duckx)
target_link_libraries(lib PUBLIC leveldb)

target_link_libraries(docsManager LINK_PUBLIC lib)
