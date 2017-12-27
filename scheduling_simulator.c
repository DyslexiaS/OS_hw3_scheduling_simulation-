#include "scheduling_simulator.h"
static ucontext_t start;
static ucontext_t end;
static ucontext_t shell_mode;
static ucontext_t current;
void creatq()
{
	front = rear = (Node*)malloc(sizeof(Node));
	front->next = rear->next = NULL;
	lfront = lrear = (Node*)malloc(sizeof(Node));
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
			strcpy(newnode->name,name);
			strcpy(newnode->state, "TASK_READY");

			char *stack = (char*)malloc(8192);
			getcontext(&newnode->task);
			newnode->task.uc_stack.ss_sp = stack;
			newnode->task.uc_stack.ss_size = 8192;
			newnode->task.uc_stack.ss_flags = 0;
			newnode->task.uc_link = &end;
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
	//printf("in add_ready_Q:pid= %d, W_time= %lld\n",newnode->pid,newnode->W_time);
	newnode->lnext = NULL;
	if(lfront == lrear) {
		lfront->lnext = newnode;
		lrear = newnode;
	} else {
		lrear->lnext = newnode;
		lrear=newnode;
	}
	strcpy(newnode->state,"TASK_READY");
	newnode->S_time = getCurrentTime();
}
Node* del_ready_q()
{
	Node* delnode = lfront->lnext;
	if(delnode==NULL)
		return NULL;
	if(lfront->lnext == lrear) {
		lrear = lfront;
		lfront->lnext = NULL;
	} else {
		lfront->lnext = delnode->lnext;
	}
	delnode->lnext = NULL;
	//renew Queueing time
	delnode->W_time +=(getCurrentTime()-delnode->S_time);
	//  printf("del = %d\n",delnode->pid);
	return delnode;
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
//			lrear = (Node*)malloc(sizeof(Node));
//          lrear->lnext = NULL;
			lrear = lfront;
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
			free(rmnode->task.uc_stack.ss_sp);
			free(rmnode);
			return;
		} else if(rmnode->pid == pid) {
			tmpnode->next = rmnode->next;
			rmnode->next = NULL;
			free(rmnode->task.uc_stack.ss_sp);
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
	printf("*************All Queue ***************\n");
	printf("PID TASK_NAME TASK_STATE QUEUEING_TIME SLEEPING_TIME\n");
	while(tmpnode != NULL) {
		printf("%-3d  %s    %-18s%lld         %-5d\n",tmpnode->pid,tmpnode->name,
		       tmpnode->state,
		       tmpnode->W_time,tmpnode->Sleep_time);
		tmpnode = tmpnode->next;
	}
	tmpnode = lfront->lnext;
	printf("*************Ready Queue***************\n");
	printf("PID TASK_NAME TASK_STATE QUEUEING_TIME SLEEPING_TIME\n");
	while(tmpnode != NULL) {
		printf("%-3d  %s    %-18s%lld         %-5d\n",tmpnode->pid,tmpnode->name,
		       tmpnode->state,
		       tmpnode->W_time,tmpnode->Sleep_time);
		tmpnode = tmpnode->lnext;
	}
}
void hw_suspend(int msec_10)
{
	set_timer(0);
	strcpy(now->state,"TASK_WAITING");
	now->Sleep_time = msec_10*10;
	Node* tmp = now;
	now = NULL;
	swapcontext(&tmp->task, &start);
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
		if(!strcmp(tmpnode->name,task_name)&&!strcmp(tmpnode->state,"TASK_WAITING")) {
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
	   &&task_name[4]<='6') {
		addq(task_name,10);
		return PID; // the pid of created task name
	} else {
		printf("TASK_NAME ERROR!\n");
		return -1;
	}
}
void shell(void)
{
	getcontext(&shell_mode);
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
			start_time();
			printf("simulating...\n");
			swapcontext(&shell_mode,&start);
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
		dormerq();
		break;
	case SIGTSTP:
		printf("Catch Ctrl+Z\n");
		if(now!=NULL) {
			add_ready_q(now);
			now = NULL;
			set_timer(0);
		}
		getcontext(&current);
		swapcontext(&current,&shell_mode);
		break;
	default:
		printf("SIGNAL ERROR.\n");
		break;
	}
}
//Timer
void dormerq()
{
	if(now == NULL)
		setcontext(&start); //HERE
	//printf("dormer pid = %d\n",now->pid);
	Node* tmpnode;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		if(!strcmp(tmpnode->state,"TASK_WAITING")) {
			tmpnode->Sleep_time -= now->Q_time;
			/*wake up task*/
			if(tmpnode->Sleep_time <= 0)
				add_ready_q(tmpnode);
		}
		tmpnode = tmpnode->next;
	}
	add_ready_q(now);
	Node* run_node = del_ready_q();
	strcpy(run_node->state,"TASK_RUNNING");
	now = run_node;
	set_timer(run_node->Q_time);
	//printf("swp %d / %d\n",lrear->pid,now->pid);
	swapcontext(&lrear->task,&now->task);
}
void set_timer(int Q_time)
{
	/*timer setting*/
	//struct itimerval new_value, old_value;
	new_value.it_value.tv_sec=0;
	new_value.it_value.tv_usec=Q_time*1000;
	new_value.it_interval.tv_sec=0;
	new_value.it_interval.tv_usec=0;
	if(setitimer(ITIMER_REAL, &new_value, &old_value)<0) {
		fprintf(stderr, "\nsetitimer < 0");
		exit(EXIT_FAILURE);
	}
}
void renew_time()
{
	Node* tmpnode;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		if(!strcmp(tmpnode->state,"TASK_READY"))
			tmpnode->W_time += (getCurrentTime()-now->S_time);
		if(!strcmp(tmpnode->state,"TASK_WAITING")) {
			tmpnode->Sleep_time -= now->Q_time;
			/*wake up task*/
			if(tmpnode->Sleep_time <= 0)
				add_ready_q(tmpnode);
		}
		tmpnode = tmpnode->next;
	}
}
bool task_exist()
{
	/*check no task*/
	Node* tmpnode;
	tmpnode = front->next;
	while(tmpnode != NULL) {
		if(strcmp(tmpnode->state,"TASK_TERMINATED"))
			return 1;
		tmpnode = tmpnode ->next;
	}
	return 0;
}
void start_time()
{
	/*everyone get start time*/
	Node* tmpnode;
	tmpnode = lfront->lnext;
	while(tmpnode != NULL) {
		tmpnode->S_time = getCurrentTime();
		//printf("pid= %d,S_time= %lld",tmpnode->pid,tmpnode->S_time);
		tmpnode = tmpnode ->lnext;
	}
}
void simulating()
{
	while(1) {
		if(!task_exist())
			swapcontext(&start,&shell_mode);
		/*check ready Queue*/
		Node* run_node = del_ready_q();
		if(run_node == NULL) {  //no running, some waiting
			usleep(10000);
			Node* tmpnode;
			tmpnode = front->next;
			while(tmpnode != NULL) {
				if(!strcmp(tmpnode->state,"TASK_WAITING")) {
					tmpnode->Sleep_time-=10;
					if(tmpnode->Sleep_time <= 0)
						add_ready_q(tmpnode);
				}
				tmpnode = tmpnode ->next;
			}
		} else {
			now = run_node;
			strcpy(run_node->state,"TASK_RUNNING");
			set_timer(run_node->Q_time);
			assert(setcontext(&run_node->task)!=-1);//HERE
		}
	}
}
void termination()
{
	while(1) {
		set_timer(0);
		if(now == NULL) return;
		strcpy(now->state,"TASK_TERMINATED");
		now = NULL;
		swapcontext(&end,&start);
	}
}
void signal_init()
{
	struct sigaction stp;
	stp.sa_handler = &handler;
	sigemptyset(&stp.sa_mask);
	sigaddset(&stp.sa_mask, SIGALRM);
	sigaddset(&stp.sa_mask, SIGTSTP);
	stp.sa_flags = 0;
	assert(sigaction(SIGALRM, &stp, NULL)==0);
	assert(sigaction(SIGTSTP, &stp, NULL)==0);
}
int main()
{
	signal_init();
	creatq();
	char *stack = (char*)malloc(8192);
	getcontext(&end);
	end.uc_stack.ss_sp = stack;
	end.uc_stack.ss_size = 8192;
	end.uc_stack.ss_flags = 0;
	end.uc_link = NULL;
	makecontext(&end,termination,0);

	char *stack2 = (char*)malloc(8192);
	getcontext(&start);
	start.uc_stack.ss_sp = stack2;
	start.uc_stack.ss_size = 8192;
	start.uc_stack.ss_flags = 0;
	start.uc_link = NULL;
	makecontext(&start,simulating,0);

	shell();
	return 0;
}
