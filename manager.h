#ifndef MANAGER_H
#define MANAGER_H

#include "editor.h"

int init_manager(char* file_name);
int close_manager(void);
int draw_editor(int index);
int insert_into_active_editor(char new_c);

#endif 
