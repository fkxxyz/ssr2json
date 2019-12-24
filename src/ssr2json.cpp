
#include <ctype.h>
#include <cstring>
#include <iostream>
#include "base64.h"
using namespace std;


const char default_local_address[] = "127.0.0.1";
const unsigned long default_local_port = 1080;


int isdigitstr(char *str){
	if (!*str) return 0;
	for (char *p = str; *p; p++)
		if (!isdigit(*p))
			return 0;
	return 1;
}

int main(int argc, char *argv[]){

	// 检查参数个数
	if (argc != 2) return 1;

	// 检查链接类型
	if (strncmp(argv[1], "ssr://", 6) != 0) return 1;

	char *encoded_link_data = argv[1] + 6;
	char decoded_link_data[1024];

	// 解码链接
	base64_decode_str(encoded_link_data, decoded_link_data, base64_decode_table_s);

	union {
		char *param[6];
		struct {
			char *server;
			char *server_port;
			char *protocol;
			char *method;
			char *obfs;
			char *password_encoded;
		} d;
	} ssr_param;

	struct {
		char **server;
		char **server_port;
		const char *local_address;
		const unsigned long local_port;
		char *password;
		char **method;
		char **protocol;
		char **obfs;
		char *key[32];
		bool isdigit[32];
		char *value[32];
	} ssr_data = {
		&ssr_param.d.server,
		&ssr_param.d.server_port,
		default_local_address,
		default_local_port,
		nullptr,
		&ssr_param.d.method,
		&ssr_param.d.protocol,
		&ssr_param.d.obfs
	};

	// 根据 ':' 分割，得到 6 个大参数
	char *p = ssr_param.param[0] = decoded_link_data;
	for (int i = 1; i < 6; i++){
		p = strchr(p, ':');
		if (!p) {
			cerr<<"Invalid ssr link."<<endl;
			return 1;
		}
		*p = '\0';
		ssr_param.param[i] = ++p;
	}

	// 将最后一个参数的 '/' 替换成 '\0' ，得出编码后的 password
	p = strchr(ssr_param.param[5], '/');
	if (p)
		*(p++) = '\0';

	// 用来存放已经解码过的数据（可以存多个字符串参数）
	char decval[1024];
	char *p_decval = decval;
	long dec_len;

	// 解码 password
	base64_decode(ssr_param.d.password_encoded, p_decval, &dec_len, base64_decode_table_s);
	p_decval[dec_len] = '\0';
	ssr_data.password = p_decval;
	p_decval += dec_len + 1;

	// 对 '?' 之后的数据根据 & 分割，得到所有其他参数
	if (p){
		p = strchr(p, '?');
		char **pkey = ssr_data.key;
		for (; p; p = strchr(p, '&')){
			*(p++) = '\0';
			*(pkey++) = p;
		}

		// 对所有其他参数进行解码
		*pkey = nullptr;
		for (int i = 0; ssr_data.key[i]; i++){
			p = strchr(ssr_data.key[i], '=');
			*(p++) = '\0';
			if (isdigitstr(p)){
				ssr_data.isdigit[i] = true;
				ssr_data.value[i] = p;
			} else {
				ssr_data.isdigit[i] = false;
				base64_decode(p, p_decval, &dec_len, base64_decode_table_s);
				p_decval[dec_len] = '\0';
				ssr_data.value[i] = p_decval;
				p_decval += dec_len + 1;
			}
		}
	}

	// 输出结果
	cout<<"{"<<endl<<
		"  \"server\": \""<<*ssr_data.server<<"\","<<endl<<
		"  \"server_port\": \""<<*ssr_data.server_port<<"\","<<endl<<
		"  \"local_address\": \""<<ssr_data.local_address<<"\","<<endl<<
		"  \"local_port\": "<<ssr_data.local_port<<","<<endl<<
		"  \"password\": \""<<ssr_data.password<<"\","<<endl<<
		"  \"method\": \""<<*ssr_data.method<<"\","<<endl<<
		"  \"protocol\": \""<<*ssr_data.protocol<<"\","<<endl<<
		"  \"obfs\": \""<<*ssr_data.obfs<<"\"";
	for (int i = 0; ssr_data.key[i]; i++){
		cout<<","<<endl<<"  \""<<ssr_data.key[i]<<"\": ";
		if (ssr_data.isdigit[i])
			cout<<ssr_data.value[i];
		else
			cout<<"\""<<ssr_data.value[i]<<"\"";
	}
	cout<<endl<<
		"}"<<endl;

	return 0;

}

