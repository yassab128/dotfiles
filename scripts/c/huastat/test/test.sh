#!/bin/sh

cookie='SessionID=8AO+o8uB5o7S2tWAYCoYiXoUCPwnTZDpmjOU22BUyBGBAAG+tU4aijxkYdUtvuHO28Wau7nHuS1NgUlWc9dg3teb/f/F+v8a17RfKfqTE3AWBz517Ld9uyWQZE0hoimA'

curl --verbose --cookie "${cookie}" '192.168.8.7/api/monitoring/status'
