#!/bin/sh

readonly PATH="/usr/local/jdk-11/bin:${PATH}"
readonly file="$(echo *.java)"

clear
rm -f *.class 2>&-
javac "${file}"
java "${file%.java}"
