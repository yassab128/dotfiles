#!/bin/sh -eu

pids=''
i=1

while [ ${i} -lt 255 ]; do
	timeout 0.5 ping -q -w1 -c1 -s1 "192.168.8.${i}" 2>&- |
	sed -n '
	s/^[^ ]\+ \([0-9.]\+\).*/\1/
	p
	t a
	q
	:a
	h
	n
	n
	n
	/100/ q
	g
	p
	' &
	pids="${pids} $!"
	i=$((i+1))
done

wait ${pids}
