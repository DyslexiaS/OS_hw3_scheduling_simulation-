#include "scheduling_simulator.h"

void creatq()
{
	front = (Node*)malloc(sizeof(Node));
	rear = (Node*)malloc(sizeof(Node));
	front->next = rear->next = NULL;
	lfront = (Node*)malloc(sizeof(Node));
	lrear = (Node*)malloc(sizeof(Node));
	lfront->lnext = lrear->lnext = NULL;
}
void addq( char* name, int Q_time)
{
	if(strlen(name)!=5) {
		printf("TASK_NAME ERROR.\n");
		return ;
	}
	if(!strncmp(name,"Task",4)||!strncmp(name,"task",4)) {
		if(name[4]>='1'&&name[4]<='6') {
			Node *newnode;
			newnode = (Node*)malloc(sizeof(Node));
			if(front->next == NULL) {
				front->next = newnode;
			}
			newnode->pid = ++PID;
			newnode->Q_time = Q_time;
			newnode->W_time = 0;
			name[0] |= 1<<5;
			strcpy(newnode->name,name);
			strcpy(newnode->state, "TASK_READY");

			char *stack = (char*)malloc(4096);
			getcontext(&newnode->task);
			newnode->task.uc_stack.ss_sp = stack;
			newnode->task.uc_stack.ss_size = 4096;
			newnode->task.uc_stack.ss_flags = 0;
			newnode->task.uc_link = &start;
			switch(name[4]) {
			case '1':
				makecontext(&newnode->task,task1,0);
				break;
			case '2':
				makecontext(&newnode->task,task2,0);
				break;
			case '3':
				makecontext(&newnode->task,task3,0);
				break;
			case '4':
				makecontext(&newnode->task,task4,0);
				break;
			case '5':
				makecontext(&newnode->task,task5,0);
				break;
			case '6':
				makecontext(&newnode->task,task6,0);
				break;
			default:
				printf("TASK_NAME ERROR.\n");
				break;
			}
			newnode->next = NULL;
			rear->next = newnode;
			rear = newnode;
			add_ready_q(newnode);
		} else {
			printf("TASK_NAME ERROR.\n");
			return ;
		}
	} else {
		printf("TASK_NAME ERROR.\n");
		return ;
	}
}
void add_ready_q(Node* newnode)
{
	if(lfront->lnext == NULL)
		lfront->lnext = newnode;
	newnode->S_time = getCurrentTime();
	newnode->lnext = NULL;
	lrear->lnext = newnode;
	lrear = newnode;
}
Node* del_ready_q()
{
	Node* delnode;
	if(lfront->lnext == NULL) {
		printf("Queue is empty!\n ");
		return (NULL);
	}
	delnode = lfront->lnext;
	lfront->lnext = delnode->lnext;
	delnode->lnext = NULL;
	return (delnode);
	//free(delnode);
}
void rm_ready_q(int pid)
{
	Node* rmnode;
	Node* tmpnode;
	if(lfront->lnext == NULL)
		return;
	tmpnode = lfront;
	while(tmpnode->lnext != NULL) {
		rmnode = tmpnode->lnext;
		if(rmnode->pid == pid && lfront->lnext->lnext == NULL) {
			lfront->lnext=NULL;
			rear = (Node*)malloc(sizeof(Node));
			rear->lnext = NULL;
			return;
		} else if(rmnode->pid == pid) {
			tmpnode->lnext = rmnode->lnext;
			rmnode->lnext = NULL;
			return;
		}
		tmpnode = tmpnode->lnext;
	}

}
void removeq(int pid)
{
	Node* rmnode;
	Node* tmpnode;
	rm_ready_q(pid);
	if(front->next == NULL) {
		printf("List is empty, you have nothing to remove!\n ");
		return;
	}
	tmpnode = front;
	while(tmpnode->next != NULL) {
		rmnode = tmpnode->next;
		if(rmnode->pid == pid && front->next->next == NULL) {
			front->next=NULL;
			rear = (Node*)malloc(sizeof(Node));
			rear->next = NULL;
			free(rmnode);
			return;
		} else if(rmnode->pid == pid) {
			tmpnode->next = rmnode->next;
			rmnode->next = NULL;
			free(rmnode);
			return;
		}
		tmpnode = tmpnode->next;
	}
}
void showq()
{
	Node* tmpnode;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		printf("%-3d%s  %-18s%d\n",tmpnode->pid,tmpnode->name,tmpnode->state,
		       tmpnode->Q_time);
		tmpnode = tmpnode->next;
	}
}
void hw_suspend(int msec_10)
{
	return;
}

void hw_wakeup_pid(int pid)
{
	Node* tmpnode;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		if(tmpnode->pid == pid)
			add_ready_q(tmpnode);
		tmpnode = tmpnode ->next;
	}
	return;
}

int hw_wakeup_taskname(char *task_name)
{
	Node* tmpnode;
	int wake_num = 0;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		if(!strcmp(tmpnode->name,task_name)) {
			add_ready_q(tmpnode);
			++ wake_num;
		}
		tmpnode = tmpnode ->next;
	}
	return wake_num;
}

int hw_task_create(char *task_name)
{
	if(strlen(task_name)==5 && !strncmp(task_name,"task",4) && task_name[4]>='1'
	   &&task_name<='6') {
		addq(task_name,10);
		return PID; // the pid of created task name
	} else {
		printf("TASK_NAME ERROR!\n");
		return -1;
	}
}
void shell(void)
{
	char tmp[100];
	char name[100];
	int Q_time=0;
	char c;
	int pid;
	while(printf("$")) {
		scanf("%s",tmp);
		if(!strcmp(tmp,"add")) {
			scanf("%s",name);
			scanf("%c",&c);
			if(c!='\n') {
				scanf("%s",tmp);
				if(!strcmp(tmp,"-t")) {
					scanf("%s",tmp);
					if(!strcmp(tmp,"L"))    Q_time=20;
					else if(!strcmp(tmp,"S"))    Q_time=10;
					else {
						printf("Input error.\n");
						while(getchar()!='\n');
						continue;
					}
				} else {
					printf("Input error.\n");
					continue;
				}
			} else    Q_time=10;
			addq(name, Q_time);
		} else if(!strcmp(tmp,"remove")) {
			scanf("%d",&pid);
			removeq(pid);
		} else if(!strcmp(tmp,"start")) {
			simulating();
		} else if(!strcmp(tmp,"ps")) {
			showq();
		} else {
			printf(" %s: command not found \n",tmp);
			continue;
		}
	}
}
long getCurrentTime()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec *1000 +tv.tv_usec /1000;
}
void handler(int sig_num)
{
	switch(sig_num) {
	case SIGALRM:
		printf("timer\n");
		break;
	case SIGTSTP:
		printf("Catch Ctrl+Z\n");
		shell();
		break;
	default:
		printf("SIGNAL ERROR.\n");
		break;
	}
}
void dormerq(int time)
{


}
void simulating()
{
	getcontext(&start);
	signal(SIGALRM, handler);
	/*timer setting*/
	struct itimerval new_value, old_value;
	new_value.it_value.tv_sec=0;
	new_value.it_value.tv_usec=20000;
	new_value.it_interval.tv_sec=0;
	new_value.it_interval.tv_usec=0;
	setitimer(ITIMER_REAL, &new_value, &old_value);
	printf("simulating...\n");

	//setcontext(&);
	return;
}
int main()
{
	signal(SIGTSTP,handler);
	creatq();
	/*	long t1, t2;
		t1 = getCurrentTime();
		usleep(10000);
		t2 = getCurrentTime();
		printf("time = %ld\n",t2-t1);
	*/
	shell();
	return 0;
}
