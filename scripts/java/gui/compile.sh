#!/bin/sh

readonly PATH="/usr/local/jdk-11/bin:${PATH}"
readonly file="$(echo *.java)"

clear

# This does not work.
# export CLASSPATH='/usr/lib/jvm/java-17-openjdk/lib'

# export PATH_TO_FX='/usr/lib/jvm/java-17-openjdk/lib'
# javac --module-path='/usr/lib/jvm/java-17-openjdk/lib' "${file}"
javac "${file}" &&

java "${file%.java}"
