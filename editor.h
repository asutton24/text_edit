#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

typedef struct {
	buffer_t* buffer;
	int screen_x;
	int screen_y;
	int pos_x;
	int pos_y;
	int scroll_x;
	int scroll_y;
	int cur_x;
	int cur_y;
	char* file_path;
	char* clip;
	char line_ending;
	int mode;
} editor_t;

int new_blank_editor(editor_t** ep, int screen_x, int screen_y, int pos_x, int pos_y);
int editor_insert(editor_t* e, char new_c);
int editor_delete(editor_t* e);
int free_editor(editor_t* e);

#endif
