
#pragma once

void base64_encode(char *data, long len, char *output, const char *table);
void base64_decode(char *str, char *data, long *len, const char *table);
void base64_encode_str(char *str, char *output, const char *table);
void base64_decode_str(char *str, char *output, const char *table);

extern const char base64_encode_table_s[];
extern const char base64_decode_table_s[];


