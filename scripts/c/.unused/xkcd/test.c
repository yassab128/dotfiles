#if 0
printf "[1;1H\x1B[2J"
tmux clear-history
f="${0%.c}"
[ -f "$f" ] && : > "$f"
clang -Weverything -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE \
	-fsanitize=address,undefined,leak \
	-g -o "$f" "$0"
clang-tidy --checks='*' "$0"
exec "$f"
#endif

#include <apt-pkg/cachefile.h>
#include <apt-pkg/pkgcache.h>
#include "stdio.h"
#include "stdlib.h"

int main() {
    pkgInitConfig(*_config);
    pkgInitSystem(*_config, _system);

    pkgCacheFile cache_file;
    pkgCache* cache = cache_file.GetPkgCache();

    for (pkgCache::PkgIterator package = cache->PkgBegin(); !package.end(); package++) {
        puts(package.Name());
    }

    return 0;
}
