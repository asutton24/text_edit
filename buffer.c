#include <stdlib.h>
#include "buffer.h"

static int powof2(int x){
    int best = 0x40000000;
    while ((best & x) == 0){
        best >>= 1;
    }
    return best << 1;
}

static int max(int x, int y){
	if (x > y) return x;
	return y;
}

int get_line(buffer_t* b, int line_num, line_t** ret){
	line_t* cur;
	if (line_num > b->len) return 2;
	if (b->last_access == 0){
		goto TRAVERSE_FROM_START;
	}
	if (abs(b->la_line_num - line_num) < line_num - 1){
		cur = b->last_access;
		if (b->la_line_num - line_num < 0){
			for (int i = b->la_line_num; i < line_num; i++){
				 cur = cur->next;
				 if (cur == 0) return 3;
			}
		} else {
			for (int i = b->la_line_num; i > line_num; i--){
				cur = cur->prev;
				if (cur == 0) return 4;
			}
		}
		b->last_access = cur;
		b->la_line_num = line_num;
		*ret = cur;
		return 0;
	}
TRAVERSE_FROM_START:
	cur = b->first;
	for (int i = 1; i < line_num; i++){
		cur = cur->next;
		if (cur == 0) return 1;
	}
	*ret = cur;
	b->last_access = cur;
	b->la_line_num = line_num;
	return 0;
}

int new_line(line_t** lp){
	line_t* l;
	l = (line_t*)malloc(sizeof(line_t));
	if (l == 0) return 1;
	l->chars = (char*)malloc(sizeof(char) * 16);
	if (l->chars == 0) return 1;
	l->chars[0] = 0;
	l->len = 1;
	l->buf_len = 16;
	l->prev = 0;
	l->next = 0;
	*lp = l;
	return 0;
}

int free_lines(line_t* l){
	free(l->chars);
	line_t* next = l->next;
	free(l);
	if (next == 0) return 0;
	else return free_lines(next);
}

int new_buffer(buffer_t** bp){
	buffer_t* b;
	b = (buffer_t*)malloc(sizeof(buffer_t));
	if (b == 0 || new_line(&b->first)) return 1;
	b->len = 1;
	b->last_access = 0;
	b->la_line_num = 0;
	*bp = b;
	return 0;
}

int free_buffer(buffer_t* b){
	free_lines(b->first);
	free(b);
	return 0;
}

int replace_char(buffer_t* b, int line, int pos, char new_c){
	if (b == 0) return 1;
	line_t* cur;
	if (get_line(b, line, &cur) || pos >= cur->len) return 1;
	cur->chars[pos] = new_c;
	return 0;
}

static int insert_newline(buffer_t* b, line_t* l, int pos){
	int newline_len = l->len - pos;
	line_t* old_next = l->next;
	if (new_line(&l->next)) return 1;
	if (newline_len >= 16){
		l->next->chars = (char*)realloc(l->next->chars, powof2(newline_len) * sizeof(char));
		if (l->next->chars == 0) return 1;
		l->next->buf_len = powof2(newline_len);
	}
	l->next->len = newline_len;
	for (int i = pos; i < l->len; i++){
		l->next->chars[i - pos] = l->chars[i];
	}
	l->chars[pos] = '\n';
	l->next->prev = l;
	l->next->next = old_next;
	b->len++;
	return 0;
}

int insert_char(buffer_t* b, int line, int pos, char new_c){
	if (b == 0) return 1;
	line_t* l;
	if (get_line(b, line, &l) || pos >= l->len) return 2;
	if (new_c == '\n') return insert_newline(b, l, pos);
	l->len++;
	if (l->len >= l->buf_len){
		l->chars = (char*)realloc(l->chars, l->buf_len * 2 * sizeof(char));
		l->buf_len *= 2;
		if (l->chars == 0) return 3;
	}
	for (int i = l->len - 1; i > pos; i--){
		l->chars[i] = l->chars[i - 1];
	}
	l->chars[pos] = new_c;
	return 0;
}

static int join_lines(buffer_t* b, line_t* l){
	if (l->prev == 0) return 0;
	line_t* p = l->prev;
	p->next = l->next;
	if (p->buf_len <= l->len + p->len){
		p->chars = (char*)realloc(p->chars, powof2(l->len + p->len) * sizeof(char));
		p->buf_len = powof2(l->len + p->len);
		if (l->chars == 0) return 3;
	}
	for (int i = p->len - 1; i < l->len + p->len - 1; i++){
		p->chars[i] = l->chars[i - p->len - 1];
	}
	p->len += l->len - 1;
	l->next = 0;
	b->len--;
	free_lines(l);
	return 0;
}

int delete_char(buffer_t* b, int line, int pos){
	if (b == 0) return 1;
	line_t* l;
	if (get_line(b, line, &l) || pos > l->len) return 2;
	if (l->len == 0 || (l->len == 1 && l->chars[0] == '\n')){
		l->prev->next = l->next;
		l->next = 0;
		free_lines(l);
		b->len--;
		return 0;
	}
	pos--;
	if (pos == -1) return join_lines(b, l);
	for (int i = pos; i < l->len; i++) l->chars[i] = l->chars[i + 1];
	l->len--;
	return 0;
}

