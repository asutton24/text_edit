#include <stdlib.h>
#include "editor.h"
#include "buffer.h"

static int retab(line_t* l, int split){
	int insert = 0;
	char* new = (char*)malloc(sizeof(char)  * l->buf_len);
	char* old = l->chars;
	char skiptab = 0;
	int newbuf = l->buf_len;
	int j = 0;
	for (int i = 0; i < l->len; i++){
		if (i == split){
			insert = split % 8;
			skiptab = 0;
		}
		if (skiptab){
			if (old[i] == '\t' && i % 8 != insert) continue;
			skiptab = 0;
		}
		do {
			new[j] = old[i];
			j++;
			if (j == newbuf){
				new = (char*)realloc(new, sizeof(char) * 2 * newbuf);
				newbuf *= 2;
				if (new == 0) return 1;
			}
		} while (old[i] == '\t' && j % 8 != 0);
		if (old[i] == '\t') skiptab = 1;
	}
	free(old);
	l->chars = new;
	l->len = j;
	l->buf_len = newbuf;
	return 0;
}

int new_blank_editor(editor_t** ep, int screen_x, int screen_y, int pos_x, int pos_y){
	editor_t* e;
	e = (editor_t*)malloc(sizeof(editor_t));
	if (e == 0) return 1;
	int status = new_buffer(&e->buffer);
	if (status) return 1;
	e->screen_x = screen_x;
	e->screen_y = screen_y;
	e->pos_x = pos_x;
	e->pos_y = pos_y;
	e->scroll_x = 0;
	e->scroll_y = 1;
	e->cur_x = 0;
	e->cur_y = 1;
	e->file_path = 0;
	e->clip = 0;
	#ifdef _WIN32
	e->line_ending = 'w';
	#else
	e->line_ending = 'u';
	#endif
	e->mode = 0;
	*ep = e;
	return 0;
}

int editor_insert(editor_t* e, char new_c){
	int status = insert_char(e->buffer, e->cur_y, e->cur_x, new_c);
	if (status) return status;
	line_t* l;
	if (new_c == '\n'){
		e->cur_x = 0;
		e->cur_y++;
		if (e->cur_y >= e->scroll_y + e->screen_y - 1) e->scroll_y++;
	} else {
		e->cur_x++;
		if (e->cur_x >= e->scroll_x + e->screen_x) e->scroll_x++;
	}
	if (new_c == '\t' && e->cur_x % 8 != 0) editor_insert(e, '\t');
	if (status = get_line(e->buffer, e->cur_y, &l)) return status;
	retab(l, e->cur_x);
	return 0;
}

int editor_delete(editor_t* e){
	line_t* l;
	int nx, ny;
	int status;
	char deleted = 0;
	if (status = get_line(e->buffer, e->cur_y, &l)) return status;
	if ((e->cur_x == 0 || (e->cur_x == 1 && l->chars[0] == '\n')) && l->prev != 0){
		ny = e->cur_y - 1;
		nx = l->prev->len - 1;
	} else {
		nx = e->cur_x - !(l->prev == 0 && e->cur_x == 0);
		ny = e->cur_y;
	}
	if (e->cur_x != 0) deleted = l->chars[e->cur_x - 1];
	status = delete_char(e->buffer, e->cur_y, e->cur_x);
	if (status) return status;
	e->cur_x = nx;
	e->cur_y = ny;
	if (ny < e->scroll_y) e->scroll_y--;
	if (deleted == '\t' && l->chars[e->cur_x - 1] == '\t' && e->cur_x % 8 != 0) editor_delete(e);
	if (status = get_line(e->buffer, e->cur_y, &l)) return status;
	retab(l, e->cur_x);
	return 0;
}

int free_editor(editor_t* e){
	free_buffer(e->buffer);
	free(e);
	return 0;
}

int editor_left(editor_t* e){
	if (e->cur_x == 0) return 0;
	line_t* l;
	int status = get_line(e->buffer, e->cur_y, &l);
	if (status) return status;
	e->cur_x--;
	if (e->cur_x <= e->scroll_x) e->scroll_x --;
	if (e->cur_x != 0 && l->chars[e->cur_x - 1] == '\t' && e->cur_x % 8 != 0) return editor_left(e);
	return 0;
}

int editor_right(editor_t* e){
	line_t* l;
	int status = get_line(e->buffer, e->cur_y, &l);
	if (status) return status;
	if (e->cur_x == l->len - 1) return 0;
	e->cur_x++;
	if (e->cur_x >= e->scroll_x + e->screen_x) e->scroll_x++;
	if (l->chars[e->cur_x] == '\t' && e->cur_x % 8 != 0) return editor_right(e);
	return 0;
}

int editor_up(editor_t* e){
	if (e->cur_y == 1) return 0;
	e->cur_y--;
	if (e->cur_y < e->scroll_y) e->scroll_y--;
	line_t* l;
	int status = get_line(e->buffer, e->cur_y, &l);
	if (status) return status;
	if (e->cur_x > l->len - 1){
		e->cur_x = l->len - 1;
	}
	if (e->cur_x != 0 && l->chars[e->cur_x] == '\t' && l->chars[e->cur_x - 1] == '\t' && e->cur_x % 8 != 0) return editor_left(e);
	return 0;
}

int editor_down(editor_t* e){
	if (e->cur_y == e->buffer->len) return 0;
	e->cur_y++;
	if (e->cur_y >= e->scroll_y + e->screen_y - 1) e->scroll_y++;
	line_t* l;
	int status = get_line(e->buffer, e->cur_y, &l);
	if (status) return status;
	if (e->cur_x > l->len - 1){
		e->cur_x = l->len - 1;
	}
	if (e->cur_x != 0 && l->chars[e->cur_x] == '\t' && l->chars[e->cur_x - 1] == '\t' && e->cur_x % 8 != 0) return editor_left(e);
	return 0;
} 
