#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "manager.h"
#include "editor.h"
#include "dialog.h"

editor_t* editors[16];
int screen_map[16];
int active_editor;
int total_editors;
dialog_t dialog;

static char char_translate(char c){
	if (c == '\t' || c == '\n') return ' ';
	return c;
}
static int find_new_line(char* s){
	int len = 0;
	while (*s != '\n' && *s != 0){
		s++;
		len++;
	}
	return len;
}

int init_manager(char* file_name){
	int row, col, status;
	char* stream;
	FILE* file;
	int sz;
	active_editor = 0;
	total_editors = 1;
	dialog.active = 0;
	initscr();
	raw();
	scrollok(stdscr, FALSE);
	curs_set(0);
	noecho();
    keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	if (file_name == 0){
		status = new_blank_editor(&editors[0], col, row, 0, 0);
		if (status) return 1;
	} else {
		file = fopen(file_name, "rb");
		if (file == 0){
			status = new_blank_editor(&editors[0], col, row, 0, 0);
			if (status) return 1;
			for (int i = 0; i < 1024; i++){
				editors[0]->file_path[i] = file_name[i];
				if (file_name[i] == 0) break;
			}
			return 0;
        }
        fseek(file, 0L, SEEK_END);
		sz = ftell(file);
		stream = (char*)malloc(sz + 1);
		if (stream == 0) return 1;
		rewind(file);
		fread(stream, 1, sz, file);
		stream[sz] = 0;
		fclose(file);
		status = new_editor(&editors[0], col, row, 0, 0, stream);
		free(stream);
		if (status) return 1;
		for (int i = 0; i < 1024; i++){
			editors[0]->file_path[i] = file_name[i];
			if (file_name[i] == 0) break;
		}
	}
	for (int i = 1; i < 16; i++){
		editors[i] = 0;
	}
	return 0;
}

int close_manager(void){
	for (int i = 0; i < 16; i++){
		if (editors[i]){
			free_editor(editors[i]);
		}
	}
	endwin();
	return 0;
}

int draw_editor(int index){
	int status;
	editor_t* e = editors[index];
	if (e == 0) return 1;
	int rel_x = e->pos_x;
	int rel_y = e->pos_y + 1;
	move(e->pos_y, e->pos_x);
	int header_len;
	if (e->file_path[0] == 0) header_len = 0;
	else header_len = strlen(e->file_path);
	attron(A_REVERSE);
	for (int i = 0; (i < header_len) && (i < e->screen_x); i++){
		printw("%c", e->file_path[i]);
	}
	for (int i = header_len; i < e->screen_x; i++){
		printw(" ");	
	}
	attroff(A_REVERSE);
	line_t* l;
	status = get_line(e->buffer, e->scroll_y, &l);
	if (status) return 1;
	for (int i = e->scroll_y + 1; i < e->scroll_y + e->screen_y; i++){
		move(rel_y, rel_x);
		if (!l){
			for (int j = 0; j < e->screen_x; j++) printw(" ");
			rel_y++;
			continue;
		}
		for (int j = e->scroll_x; (j < find_new_line(l->chars)) && (j < e->scroll_x + e->screen_x); j++){
			if (i == e->cur_y + 1 && j == e->cur_x){
				attron(A_REVERSE);
				printw("%c", char_translate(l->chars[j]));
				attroff(A_REVERSE);
			} else {
				printw("%c", char_translate(l->chars[j]));
			}
		}	
		for (int j = find_new_line(l->chars); j < e->screen_x + e->scroll_x; j++){
			if (i == e->cur_y + 1 && j == e->cur_x){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			} else {
				printw(" ");
			}
		}
		l = l->next;
		rel_y++;	
	}
	return 0;
}

int draw_dialog(void){
	if (!dialog.active) return 0;
	move(dialog.pos_y, dialog.pos_x); 
	attron(A_REVERSE);
	int j = 0;
	for (int i = 0; i < dialog.scr_x; i++){
		if (dialog.prompt[j] != 0){
			printw("%c", dialog.prompt[j]);
			j++;
		} else {
			printw(" ");
		}
	} 
	move(dialog.pos_y + 1, dialog.pos_x);
	printw(" ");
	attroff(A_REVERSE);
	for (int i = 0; i < dialog.scr_x - 2; i++){
		if (dialog.scroll + i < dialog.len){
			if (dialog.scroll + i == dialog.index) attron(A_REVERSE);
			printw("%c", dialog.resp[dialog.scroll + i]);
			if (dialog.scroll + i == dialog.index) attroff(A_REVERSE);
		} else {
			if (dialog.scroll + i == dialog.index) attron(A_REVERSE);
			printw(" ");
			if (dialog.scroll + i == dialog.index) attroff(A_REVERSE);
		}
	}
	attron(A_REVERSE);
	printw(" ");
	move(dialog.pos_y + 2, dialog.pos_x);
	for (int i = 0; i < dialog.scr_x; i++){
		printw(" ");
	} 
	attroff(A_REVERSE);
}

int draw_editors(void){
	clear();
	for (int i = 0; i < 16; i++){
		draw_editor(i);
	}
	draw_dialog();
	refresh();
}

int insert_into_active_editor(char new_c){
	return editor_insert(editors[active_editor], new_c);
}

int delete_from_active_editor(void){
	return editor_delete(editors[active_editor]);
}

int active_editor_left(void){
	return editor_left(editors[active_editor]);
}

int active_editor_right(void){
	return editor_right(editors[active_editor]);
}

int active_editor_up(void){
	return editor_up(editors[active_editor]);
}

int active_editor_down(void){
	return editor_down(editors[active_editor]);
}

int dialog_keypress(int press){
	int status = 0;
	if (press == KEY_BACKSPACE) status = dialog_delete(&dialog);
	else if (press == KEY_LEFT) status = dialog_left(&dialog);
	else if (press == KEY_RIGHT) status = dialog_right(&dialog);
	else if (press == '\n') {
		dialog_finalize(&dialog);
		status = 64;
	} else if (press >= 32 && press < 256) dialog_insert(&dialog, (char)press);
	return status;
}

int save_to_file(void){
	if (editors[active_editor]->file_path[0] == 0){
		int row, col, status;
		getmaxyx(stdscr, row, col);
		dialog_init(&dialog, 2, row / 2 - 1, col - 2, "Enter file name :");
		do {
			draw_editors();
		} while (!(status = dialog_keypress(getch())));
		if (status != 64) return 1;
		for (int i = 0; i < dialog.len; i++){
			editors[active_editor]->file_path[i] = dialog.resp[i];
		} 
	}
	int len;
	char* stream;
	FILE* file;
	if (buffer_to_stream(editors[active_editor]->buffer, &stream, editors[active_editor]->line_ending, &len)) return 0;
	file = fopen(editors[active_editor]->file_path, "wb");
	if (!file) return 1;
	fwrite(stream, 1, len, file);
	fclose(file);
	free(stream);
	return 0;
}

int handle_keypress(int press){
	int status = 0;
	if (press == KEY_BACKSPACE) status = delete_from_active_editor();
	else if (press == KEY_LEFT) status = active_editor_left();
	else if (press == KEY_RIGHT) status = active_editor_right();
	else if (press == KEY_UP) status = active_editor_up();
	else if (press == KEY_DOWN) status = active_editor_down();
	else if (press == 17) status = 255;
	else if (press == 19) status = save_to_file();
	else if (press < 256) status = insert_into_active_editor((char)press);
	return status;
}


