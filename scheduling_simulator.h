#ifndef SCHEDULING_SIMULATOR_H
#define SCHEDULING_SIMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
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
	int Q_time; //Quantum T
	long long int S_time; //start T
	long long int W_time; //Queueing T
	int Sleep_time; //suspend T
	ucontext_t task;
	struct node* next;
	struct node* lnext;
} Node;
Node* now;
Node* front, *rear;
Node* lfront, *lrear;
int PID;
struct itimerval new_value,old_value;
/****Queue****/
void creatq();
void addq( char* name, int Q_time);
void add_ready_q(Node* newnode);
Node* delq();
void re_ready_q(int pid);
void removeq(int pid);
void showq();
/****hw_function****/
void hw_suspend(int msec_10);
void hw_wakeup_pid(int pid);
int hw_wakeup_taskname(char *task_name);
int hw_task_create(char *task_name);
/*****time*****/
void set_timer(int Q_time);
void start_time();
void renew_time();
void simulating();
long getCurrentTime();
/*****signal*****/
void signal_init();
void handler(int sig_num);
void dormerq();
bool task_exist();
void termination();
void shell();

#endif

