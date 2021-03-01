#pragma once

void back_file(char * file, char *ext, int text);
int write_file_by_date(char *file, char *ext, char *wbuf, int wlen, int text);
int write_file_txt(char *file, char *wbuf, int opt);
int file_name_by_date(char *file, char *ext, char *dest);
