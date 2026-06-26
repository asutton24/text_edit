#include "dialog.h"

int dialog_init(dialog_t* d, int pos_x, int pos_y, int scr_x, char* prompt){
	d->pos_x = pos_x;
	d->pos_y = pos_y;
	d->scr_x = scr_x;
	d->prompt = prompt;
	d->resp[0] = 0;
	d->index = 0;
	d->scroll = 0;
	d->len = 0;
	d->active = 1;	
}

int dialog_insert(dialog_t* d, char new_c){
	if (d->len == 1023) return 0;
	if (d->index > 1023 || d->index < 0) return 1;
	for (int i = 1023; i > d->index; i--){
		d->resp[i] = d->resp[i-1];
	}
	d->resp[d->index] = new_c;
	d->index++;
	d->len++;
	if (d->index == d->scroll + d->scr_x - 2) d->scroll++;
	return 0;
}

int dialog_delete(dialog_t* d){
	if (d->index == 0) return 0;
	if (d->index > 1024 || d->index < 0) return 1;
	for (int i = d->index - 1; i < 1023; i++){
		d->resp[i] = d->resp[i+1];
	}
	d->len--;
	if (d->index < d->scroll) d->scroll = d->index;
	d->index--;
	return 0;
}

int dialog_left(dialog_t* d){
	if (d->index == 0) return 0;
	d->index--;
	if (d->index < d->scroll) d->scroll = d->index;
	return 0;
}

int dialog_right(dialog_t* d){
	if (d->index == d->len) return 0;
	d->index++;
	if (d->index == d->scroll + d->scr_x - 2) d->scroll++;
	return 0;
}

int dialog_finalize(dialog_t* d){
	d->resp[d->len] = 0;
}
