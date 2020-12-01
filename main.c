#include <pthread.h>
#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "list.h"
#include "service.h"

typedef struct Context{
	List *list;
	int ret_code;
} Context;

int readNewLine(const char *buff, List *list, char *new_line_flag){
	if (buff == NULL || list == NULL || new_line_flag == NULL)
		return EINVAL;

	int err;
	if (buff[0] == '\n'){
		//show list
		err = showList(list);
		assertSuccess("readLoop:showList", err);
		return SUCCESS_CODE;
	}
	
	//adding element
	char *new_line_pos = strchr(buff, '\n');
	if (new_line_pos == NULL)
		*new_line_flag = 0;
	else 
		*new_line_pos = '\0';

	err = addElement(list, buff);
	assertSuccess("readLoop:addElement", err);

	return SUCCESS_CODE;
}

int readRestLine(const char *buff, List *list, char *new_line_flag){
	if (buff == NULL || list == NULL || new_line_flag == NULL)
		return EINVAL;

	char *new_line_pos = strchr(buff, '\n');
	if (new_line_pos != NULL){
		*new_line_flag = 1;
		*new_line_pos = '\0';
	}

	if (new_line_pos != buff){
		int err = addElement(list, buff);
		assertSuccess("readLoop:addElement", err);
	}

	return SUCCESS_CODE;
}

void readLoop(List *list){
	//1 for \0
    char buff[MAX_LEN + 1];
	char new_line = 1;
	do{
		errno = SUCCESS_CODE;
		ssize_t err_read = read(STDIN_FILENO, buff, MAX_LEN);
		if (err_read == ERROR_CODE){
			assertSuccess("readLoop:read", err_read);
		}
		
		size_t read_cnt = err_read;
		//eof
		if (read_cnt == 0)
			break;

		buff[read_cnt] = '\0';
		if (new_line)
			readNewLine(buff, list, &new_line);
		else
			readRestLine(buff, list, &new_line);

	} while (1);
}

void *routine(void *data){
	if (data == NULL)
		pthread_exit(SUCCESS_CODE);

	Context *cntx = (Context*)data;
	while (1){
		int err = sortList(cntx->list);
		assertSuccess("routine: sortList", err);
		sleep(5);
	}
}

int main(int argc, char **argv){

	List list;
    int err = createList(&list);
    assertSuccess("createList", err);

	Context cntx;
	cntx.list = &list;
	cntx.ret_code = SUCCESS_CODE;
	pthread_t pid;
	err = pthread_create(&pid, NULL, routine, (void*)(&cntx));

	readLoop(&list);

	err = pthread_cancel(pid);
	assertSuccess("main:pthread_cancel", err);

	err = pthread_join(pid, NULL);
	if (err != ESRCH)
		assertSuccess("main:pthread_join", err);

	err = destroyList(&list);
	assertSuccess("main:destroyList", err);

	return 0;
}
