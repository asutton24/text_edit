#ifndef BUFFER_H
#define BUFFER_H

typedef struct line_t {
	char* chars;
	int len;
	int buf_len;
	struct line_t* next;
	struct line_t* prev;
} line_t;

typedef struct {
	line_t* first;
	int len;
	line_t* last_access;
	int la_line_num;
} buffer_t;

int get_line(buffer_t* b, int line_num, line_t** ret);
int new_buffer(buffer_t** b);
int free_buffer(buffer_t* b);
int replace_char(buffer_t* b, int line, int pos, char new_c);
int insert_char(buffer_t* b, int line, int pos, char new_c);

#endif
