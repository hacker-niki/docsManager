cmake_minimum_required(VERSION 3.5)
project(docsManager)

set(CMAKE_CXX_STANDARD 23)

add_executable(docsManager main.cpp
        Entities/Document.cpp
        Entities/Document.h
        Entities/IDocument.h
        Interfaces/IGenerator.h
        Interfaces/IRepository.h
        Manager.cpp
        Manager.h
        Generator.cpp
        Generator.h
        Entities/Template.cpp
        Entities/Template.h
        Repository/Repository.cpp
        Repository/Repository.h
        Entities/Field.cpp
        Entities/Field.h)

find_package(duckx CONFIG REQUIRED)
target_link_libraries(docsManager PRIVATE duckx::duckx)
