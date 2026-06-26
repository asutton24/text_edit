#ifndef DIALOG_H
#define DIALOG_H

typedef struct {
	int pos_x;
	int pos_y;
	int scr_x;
	char* prompt;
	char resp[1024];
	int index;
	int len;
	int scroll;
	int active;
} dialog_t;

int dialog_init(dialog_t* d, int pos_x, int pos_y, int scr_x, char* prompt);
int dialog_insert(dialog_t* d, char new_c);
int dialog_delete(dialog_t* d);
int dialog_left(dialog_t* d);
int dialog_right(dialog_t* d);
int dialog_finalize(dialog_t* d);
#endif
