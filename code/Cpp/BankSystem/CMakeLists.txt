cmake_minimum_required(VERSION 3.28)
project(BankSystem)

set(CMAKE_CXX_STANDARD 17)

add_executable(BankSystem main.cpp
        main.cpp
        Account.cpp
        Account.h
        NormalAccount.cpp
        NormalAccount.h
        VipAccount.cpp
        VipAccount.h
        SavingAccount.cpp
        SavingAccount.h
        Bank.cpp
        Bank.h
        FileManager.cpp
        FileManager.h)
