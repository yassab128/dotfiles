#!/bin/sh

sqlite3 ./nouns.sqlite <<EOF
.output nouns.h
.separator ''
select '"', word, ',', case article_mask
	when 1 then 'der'
	when 2 then 'die'
	when 3 then 'der/die'
	when 4 then 'das'
	when 5 then 'der/das'
	when 6 then 'die/das'
	else 'der/die/das'
end, '",' from noun_2 order by word
EOF
