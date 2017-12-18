#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include "task.h"

enum TASK_STATE {
	TASK_RUNNING,
	TASK_READY,
	TASK_WAITING,
	TASK_TERMINATED
};
typedef struct node {
	int pid;
	char name[100];
	char state[20];
	int Q_time;
	int S_time;
	int W_time;
	ucontext_t task;
	struct node* next;
	struct node* lnext;
} Node;

Node* front, *rear;
Node* lfront, *lrear;
void creatq();
void addq( char* name, int Q_time);
void add_ready_q(Node* newnode);
Node* delq();
void showq();
void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);
int PID;
static ucontext_t start;
#endif

