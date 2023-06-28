TARGET = hello

# QMAKE_CXXFLAGS += -Weverything -std=c++17
QMAKE_CXXFLAGS += -pedantic -std=c++17
QT += widgets
# QT += networksettings

# Input
SOURCES += hello.cpp
