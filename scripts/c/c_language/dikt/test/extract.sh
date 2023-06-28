#!/bin/sh

t='Apfel'

way=1
case "$way" in
1)
curl -s "https://de.wiktionary.org/wiki/$t?action=raw&explaintext&printable=yes"
;;

2)
curl -s "https://de.wiktionary.org//w/api.php?"\
"action=parse&"\
"format=json&"\
"printable=yes&"\
"prop=wikitext&"\
"disabletoc=&page=$t"
;;

3)
curl -s "https://de.wiktionary.org//w/api.php?"\
"action=parse&"\
"explaintext&"\
"format=json&"\
"&page=$t"
;;

4)
curl -s "https://de.wiktionary.org/w/api.php?action=query&"\
"prop=extracts&"\
"exintro&"\
"explaintext&"\
"format=json&"\
"redirects&"\
"titles=$t"
;;

*)
;;
esac
