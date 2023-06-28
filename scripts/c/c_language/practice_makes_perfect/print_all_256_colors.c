#if 0
true; exec ccompile "$0" -- "$@"
#endif

#include <stdio.h>

/* I am not satisfied with this. */

int main()
{
	for (unsigned char i = 0; i < 9; ++i) {
		for (unsigned char j = 30; j < 38; ++j) {
			for (unsigned char k = 40; k < 48; ++k) {
				printf("\x1B[%d;%d;%dmHello, World!\x1B[m\n",
					i, j, k);
			}
		}
	}
}
