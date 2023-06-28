#include <openssl/ssl.h>
#include <stdio.h>

static void hex2bin(char *hex_str, unsigned char *raw_binary)
{
	BIGNUM *a = BN_new();
	BN_hex2bn(&a, hex_str);
	BN_bn2bin(a, raw_binary);
	BN_free(a);
}

// 706f556c4b535234456c696a365030464930395a6e554d596661436c696c4200
// 012345678901234567890123456789012345678901234567890123456789012370EkSVUKnUqa1lgO88pxApYlFPgclg7U
// 100

// ist 32

static void cyka()
{
	char salt[] =
	    "706f556c4b535234456c696a365030464930395a6e554d596661436c696c4200";

	unsigned char salt_bin[SHA256_DIGEST_LENGTH];
	hex2bin(salt, salt_bin);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		printf("%hhu ", salt_bin[i]);
	}
	putchar('\n');

	unsigned char insane[SHA256_DIGEST_LENGTH];
	sscanf(
	    salt,
	    "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
	    "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
	    "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
	    &insane[0], &insane[1], &insane[2], &insane[3], &insane[4],
	    &insane[5], &insane[6], &insane[7], &insane[8], &insane[9],
	    &insane[10], &insane[11], &insane[12], &insane[13], &insane[14],
	    &insane[15], &insane[16], &insane[17], &insane[18], &insane[19],
	    &insane[20], &insane[21], &insane[22], &insane[23], &insane[24],
	    &insane[25], &insane[26], &insane[27], &insane[28], &insane[29],
	    &insane[30], &insane[31]);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		printf("%hhu ", insane[i]);
	}
	putchar('\n');

	char tlas[65];

	snprintf(
	    tlas, 65,
	    "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
	    "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx"
	    "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
	    insane[0], insane[1], insane[2], insane[3], insane[4], insane[5],
	    insane[6], insane[7], insane[8], insane[9], insane[10], insane[11],
	    insane[12], insane[13], insane[14], insane[15], insane[16],
	    insane[17], insane[18], insane[19], insane[20], insane[21],
	    insane[22], insane[23], insane[24], insane[25], insane[26],
	    insane[27], insane[28], insane[29], insane[30], insane[31]);
	puts("--------------------------------------------+++-+--++++++--+-+++-++++++");
	puts(tlas);
	puts(salt);
}

int main()
{
	cyka();
}
