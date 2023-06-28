#!/bin/sh

set -efu

# change 'save-buffers-kill-emacs to nil if you want totally unsert q
# k exists from Man and goes back to normal emacs, so I set it to k
LC_ALL=C emacs -nw -Q --execute="(progn (man \"$1\")
(delete-window)
(define-key Man-mode-map \"q\" 'save-buffers-kill-emacs)
(define-key Man-mode-map \"k\" nil))"
