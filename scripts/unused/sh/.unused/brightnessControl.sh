#!/usr/bin/bash

set -efu

case "$1" in
brightnessIncrease)
    /usr/bin/brightnessctl --quiet set 5%+
    ;;
brightnessDecrease)
    /usr/bin/brightnessctl --quiet set 5%-
    ;;
esac

/usr/bin/dunstify \
    --replace=991049 \
    --timeout=1000 \
    --urgency low "$(/usr/bin/brightnessctl | /usr/bin/awk 'BEGIN{ FS="[()]" } NR==2 {print $2}')"
