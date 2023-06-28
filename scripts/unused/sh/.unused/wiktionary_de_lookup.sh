#!/bin/sh

set -efu

curl --silent "https://de.wiktionary.org/wiki/$1?action=raw" |
	sed '
	# NOTE the order of the next sed commands is important

	# remove the left braces
	s/{//g

	# remove the right braces
	s/}//g

	# match what is between the translation section and the EOF
	/^.-Tabelle|.-links=$/, $ {

		# delete everything other than the English translation
		/^\*en/! d

		# match lines containing the English translation, they are almost
		# always just one line, but they can span over two or three lines
		/^\*en/ {

			# clean-up what is around the first English translation
			s/:[^|]\{1,\}|en|/ /

			# clean-up the rest, while separating them with a semicolon (both
			# next commands are needed)
			s/;[^|]\{1,\}|en|/; /g
			s/,[^|]\{1,\}|en|/; /g

			# rarely, some German loanwords that have no direct translation will
			# show with varied accepted spellings like Fuehrer, Fuhrer etc we
			# got to handle that case too
			s/\/.|en|/; /g

			# replace "*en" (in the beginning of the line) with a tab
			s/\*en/\t/
		}

		# remove any additional "en" that shows up some times (as mentioned
		# earlier), if it contains no translation (rarely happens, but it is
		# better to handle all the scenarios)
		/^\*en $/ d
	 }

	 # delete the pronunciation section (useless outside a browser)
	/^Aussprache$/,/^$/d

	 # delete the section showing emojis
	/^Symbole$/,/^$/d

	# remove references to pictures (same logic here)
	/^|Bild/d

	# replace "^|" with a tab
	s/^|/\t/

	# delete the double left brackets
	s/\[\[//g

	# NOTE I could have just used s/\[//g, but I want to keep strings like "[2]"
	# alone since they are always correlated with enumeration, which makes
	# things easier to read; the same goes with the double right brackets

	# delete the double right brackets
	s/\]\]//g

	# remove all the single quotes (part of wikitext syntax clean-up)
	s/\o47//g

	# replace "^:" with a tab
	s/^:/\t/

	# remove references (I would like to keep them, but they are a chore to
	# clean-up and are *again* almost useless outside a browser)
	s/<ref.*//g

	# space up what is between "=" while avoiding wikitext section headings
	s/\([[:alnum:]]\)=\([[:alnum:]]\)/\1 = \2/g
' |
	# PAGER environmental variable will fall back to less when not set
	${PAGER-less}
