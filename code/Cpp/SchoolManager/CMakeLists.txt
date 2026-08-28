cmake_minimum_required(VERSION 3.28)
project(SchoolManager)

set(CMAKE_CXX_STANDARD 17)

add_executable(SchoolManager main.cpp
        User.cpp
        User.h
        Student.cpp
        Student.h
        Teacher.cpp
        Teacher.h
        Admin.cpp
        Admin.h
        Computer.cpp
        Computer.h
        Speech.cpp
        Speech.h
        Reservation.cpp
        Reservation.h
        CampusSystem.cpp
        CampusSystem.h)
