#include <ncurses.h>
#include <stdio.h>
#include "manager.h"

int main(int argc, char** argv){
	if (argc == 2){
		init_manager(argv[1]);
	} else {
		init_manager(0);
	}
	int pressed = 'x';
	int status = 0;
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
