cmake_minimum_required(VERSION 3.28)
project(JsonManager)

set(CMAKE_CXX_STANDARD 17)

add_executable(JsonManager main.cpp
        JsonExceptions.cpp
        JsonExceptions.h
        JsonNode.cpp
        JsonNode.h
        JsonParser.cpp
        JsonParser.h
        JsonWriter.cpp
        JsonWriter.h
        KvDatabase.cpp
        KvDatabase.h)
