#!/bin/sh

clang-tidy --checks='*' ./search.cpp -- -std=c++17
