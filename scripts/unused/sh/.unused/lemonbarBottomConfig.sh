# no need for a shebang, it might confuse lemonbar

set -efu
while true; do
    fajitaInfo="$(ssh -q -c aes128-ctr user@"$TERMUX" -p 8022 "cat /storage/emulated/0/Android/obb/misc/fajitaInfo.txt")"
    batteryStatus="$(awk '{if ($1 == 0) print "DISCHARGING"}' /sys/class/power_supply/AC/online)"

    printf '%%{A:/home/pactrag/Programme/usw/shell/layoutSwitch.sh:}DE: %%{A} %s %%{r} %s\n' "$bspwmStatus" "$ariaProgress"
    sleep 60
done
