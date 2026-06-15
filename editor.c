#include <stdlib.h>
#include "editor.h"
#include "buffer.h"

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
	if (new_c == '\n'){
		e->cur_x = 0;
		e->cur_y++;
		if (e->cur_y >= e->scroll_y + e->screen_y) e->scroll_y++;
	} else {
		e->cur_x++;
	}
	return 0;
}

int free_editor(editor_t* e){
	free_buffer(e->buffer);
	free(e);
	return 0;
} 
