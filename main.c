#include <ncurses.h>
#include <stdio.h>
#include "manager.h"

int main(int argc, char** argv){
	int pressed = 'x';
	int status = 0;
	if (argc == 2){
		status = init_manager(argv[1]);
	} else {
		status = init_manager(0);
	}
	if (status) return status;
	while (pressed != 'q'){
		draw_editor(0);
		pressed = getch();
		status = handle_keypress(pressed);
		if (status) break;
	}
	close_manager();
	printf("%d\n", status);
	return status;		
}
