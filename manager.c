#include <ncurses.h>
#include <string.h>
#include "manager.h"
#include "editor.h"

editor_t* editors[16];
int screen_map[16];
int active_editor;
int total_editors;

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
	active_editor = 0;
	total_editors = 1;
	initscr();
	raw();
	cbreak();
	scrollok(stdscr, FALSE);
	curs_set(0);
	noecho();
    keypad(stdscr, TRUE);
	getmaxyx(stdscr, row, col);
	if (file_name == 0){
		status = new_blank_editor(&editors[0], col, row, 0, 0);
		if (status) return 1;
	} else {
		return 1;
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
	clear();
	int status;
	editor_t* e = editors[index];
	if (e == 0) return 1;
	int rel_x = e->pos_x;
	int rel_y = e->pos_y + 1;
	move(e->pos_y, e->pos_x);
	int header_len;
	if (e->file_path == 0) header_len = 0;
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
		for (int j = e->scroll_x; (j < find_new_line(l->chars)) && (j < e->screen_x); j++){
			if (i == e->cur_y + 1 && j == e->cur_x){
				attron(A_REVERSE);
				printw("%c", l->chars[j]);
				attroff(A_REVERSE);
			} else {
				printw("%c", l->chars[j]);
			}
		}	
		for (int j = find_new_line(l->chars); j < e->screen_x; j++){
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
	refresh();
	return 0;
}

int insert_into_active_editor(char new_c){
	return editor_insert(editors[active_editor], new_c);
}
