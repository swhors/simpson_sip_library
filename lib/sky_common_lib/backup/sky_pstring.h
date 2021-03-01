#pragma once
//메소드별 구현 함수.
#define MAX_PS_BUF_LEN	256

class str_buf
{
public:
	str_buf();
    int  len;
    char *str;
    void *p_reserve;
	str_buf &operator=(str_buf const &other);
	str_buf &operator=(char const *other);
	int		operator==(str_buf const &other);
	int		operator>(str_buf const &other);
	int		operator<(str_buf const &other);
	int		operator==(char const *other);
	int		operator>(char const *other);
	int		operator<(char const *other);
	
};

typedef class str_buf STRING;
typedef class str_buf * PSTRING;
//STRING, *PSTRING;

int sip_parse_string(unsigned char *dump, PSTRING string, int len);
void free_str(PSTRING buff);

int copy_n_str(PSTRING str, int isize, unsigned char * buff);
int copy_str(PSTRING str, char *buff);

int pstrcpy(PSTRING des_str, PSTRING src_str);

void init_string(PSTRING str);

