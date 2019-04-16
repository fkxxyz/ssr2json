

#include "base64.h"
#include <cstring>

const char base64_encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char base64_decode_table[] =
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\x3E\0\0\0\x3F"
		"\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\0\0\0\0\0\0"
		"\0\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC\xD\xE"
		"\xF\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\0\0\0\0\0"
		"\0\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23\x24\x25\x26\x27\x28"
		"\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		;

const char base64_encode_table_s[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
const char base64_decode_table_s[] =
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\x3E\0\0"
		"\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\0\0\0\0\0\0"
		"\0\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\xA\xB\xC\xD\xE"
		"\xF\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\0\0\0\0\x3F"
		"\0\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23\x24\x25\x26\x27\x28"
		"\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		;

void base64_encode(char *data, long len, char *output, const char *table){
	char *d = output;
	if (table == nullptr) table = base64_encode_table;
	long end = len - 2;
	int i;
	for (i = 0; i < end; i += 3){
		char *p = data + i;
		*(d++) = table[p[0]>>2];
		*(d++) = table[((p[0]<<4)&0x3F) | (p[1]>>4)];
		*(d++) = table[((p[1]<<2)&0x3F) | (p[2]>>6)];
		*(d++) = table[(p[2]&0x3F)];
	}
	char *p = data + i;
	switch (len - i){
	case 1:
		*(d++) = table[p[0]>>2];
		*(d++) = table[((p[0]<<4)&0x3F)];
		*(d++) = '=';
		*(d++) = '=';
		break;
	case 2:
		*(d++) = table[p[0]>>2];
		*(d++) = table[((p[0]<<4)&0x3F) | (p[1]>>4)];
		*(d++) = table[((p[1]<<2)&0x3F)];
		*(d++) = '=';
		break;
	}
	*(d++) = '\0';
}

void base64_decode(char *str, char *data, long *len, const char *table){
	char *d = data;
	if (table == nullptr) table = base64_decode_table;
	for (char *p = str; *p; p += 4){
		char b = p[1];
		*(d++) = (table[p[0]]<<2) | (table[b]>>4);
		if (!b) break;
		b = p[2];
		*(d++) = (table[p[1]]<<4) | (table[b]>>2);
		if (!b) break;
		b = p[3];
		*(d++) = (table[p[2]]<<6) | (table[b]);
		if (!b) break;
	}
	*len = d - data;
}

void base64_encode_str(char *str, char *output, const char *table){
	base64_encode(str, (long)strlen(str), output, table);
}

void base64_decode_str(char *str, char *output, const char *table){
	long len;
	base64_decode(str, output, &len, table);
	output[len] = '\0';
}

