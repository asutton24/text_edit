#include <ncurses.h>
#include "manager.h"

int main(int argc, char** argv){
	init_manager(0);
	char pressed = 'x';
	int status = 0;
	while (pressed != 'q'){
		draw_editor(0);
		pressed = getch();
		status = insert_into_active_editor(pressed);
		if (status) break;
	}
	close_manager();
	return status;		
}
