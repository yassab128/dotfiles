#!/bin/sh

set -efu

# The script does not work. Windows is a pain.

cleanup() {
	umount "${iso_mount_directory}" "${disk_mount_directory}"
	rmdir "${iso_mount_directory}" "${disk_mount_directory}"
	exit
}
trap cleanup EXIT

iso_file="/mnt/windows/Users/Uller/Desktop/Win10_20H2_v2_English_x64.iso"
disk='/dev/sdc'
device="${disk}1"
iso_mount_directory=$(mktemp --directory)
disk_mount_directory=$(mktemp --directory)

format4windows() {
	sfdisk "${disk}" --quiet --label 'gpt' <<-EOF
		-, -, EBD0A0A2-B9E5-4433-87C0-68B6B72699C7
	EOF
	mkfs.ntfs --fast "${device}"
}

format4linux() {
	sfdisk "${disk}" --quiet --label 'gpt' <<-EOF
	EOF
	mkfs.ext4 "${device}"
}

format4linux

mount -o loop,ro,unhide "${iso_file}" "${iso_mount_directory}"
mount "${disk}1" "${disk_mount_directory}"
/usr/bin/cp -r "${iso_mount_directory}/." "${disk_mount_directory}"
