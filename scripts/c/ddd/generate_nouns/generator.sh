#!/bin/sh

sqlite3 ./nouns.sqlite <<EOF
.output ../nouns.h
.separator ''
select '"', case article_mask
	when 1 then 'Der'
	when 2 then 'Die'
	when 3 then 'Der/Die'
	when 4 then 'Das'
	when 5 then 'Der/Das'
	when 6 then 'Die/Das'
	else 'Der/Die/Das'
end, ' ', word, '",' from noun_2 order by word
EOF
