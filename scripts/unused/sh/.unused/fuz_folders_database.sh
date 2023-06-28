find . -type d -not -path \

set -efu
    './Dokumente/*' -not -path \
    './.nix-profile/*' -not -path \
    './.nix-defexpr/*' -not -path \
    './.cache/*' -not -path \
    '*/\.git/*' -not -path \
    './.config/yadm/*' -not -path \
    './.config/chromium/*' -not -path \
    './.local/share/nvim/undo/*' -not -path \
    './.local/share/torbrowser/*' -not -path \
    './Programme/usw/bakup/user/dumpster/*' -not -path \
    './Programme/contributing/*'
