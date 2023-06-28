#!/bin/sh -eu

clear
[ -n ${TMUX} ] && tmux clear-history

readonly PATH="${PATH}:\
${HOME}/Downloads/android-11:\
/home/aptrug/Downloads/android-studio/jre/bin"

readonly java_file="$(echo *.java)"

clear
javac -classpath "${HOME}/Downloads/android-studio/plugins/android/lib/android.jar"\
exit
# 	"${java_file}"
# readonly basename="${java_file%.java}"
# java "${basename}"

# dx --dex --verbose --output="${basename}.dex" ${basename}.class

# /home/aptrug/Android/Sdk/build-tools/32.0.0/aapt
aapt package\
	-f\
	-M AndroidManifest.xml\
	-I "${HOME}/Downloads/android-studio/plugins/android/lib/android.jar"\
	-S res
	-J src\
	-m\
