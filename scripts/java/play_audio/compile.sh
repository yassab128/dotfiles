#!/bin/sh

readonly PATH="/usr/local/jdk-11/bin:${PATH}
"
readonly file="$(echo *.java)"

clear
javac "${file}" &&
java "${file%.java}"
