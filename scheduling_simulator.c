#include "scheduling_simulator.h"

void call_SIGALRM(__time_t tv_sec, __time_t tv_usec)
{
	struct itimerval timer;
	timer.it_value.tv_sec = tv_sec;
	timer.it_value.tv_usec = tv_usec;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 0;

	if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
		fprintf(stderr, "\nsetitimer(ITIMER_REAL, &timer, NULL) < 0\n");
		exit(EXIT_FAILURE);
	}
}

void stop_SIGALRM(void)
{
	call_SIGALRM(0, 0);
}

int is_empty(struct node *list_head)
{
	return list_head == NULL;
}

struct node *last_of(struct node *list)
{
	struct node *ret_val = list;
	while (ret_val != NULL && ret_val->next != NULL) {
		ret_val = ret_val->next;
	}
	return ret_val;
}

int in_queue(struct node *list_head, struct node *item)
{
	struct node *ptr = list_head;
	while (ptr != NULL) {
		if (ptr == item) {
			return 1;
		}
		ptr = ptr->next;
	}
	return 0;
}

void insert(struct node *target, struct node *item)
{
	assert(target != NULL && item != NULL && item->next == NULL);
	item->next = target->next;
	target->next = item;
}

void push(struct node **list_head, struct node *item)
{
	assert(item != NULL && item->next == NULL);
	struct node *last = last_of(*list_head);
	if (last != NULL) {
		insert(last, item);
	} else {
		*list_head = item;
	}
}

struct node *delete(struct node **list_head, struct node *item)
{
	assert(in_queue(*list_head, item));

	if (*list_head == item) {
		*list_head = (*list_head)->next;
	} else {
		struct node *front = *list_head;
		while (front->next != item) {
		}

		front->next = item->next;
	}

	item->next = NULL;
	return item;
}


void creatq()
{
	front = rear = (Node *)malloc(sizeof(Node));
	front->next = rear->next = NULL;
	lfront = lrear = (Node *)malloc(sizeof(Node));
	lfront->lnext = lrear->lnext = NULL;
}
void addq(char *name, int Q_time)
{
	if (strlen(name) != 5) {
		printf("TASK_NAME ERROR.\n");
		return ;
	}
	if (!strncmp(name, "Task", 4) || !strncmp(name, "task", 4)) {
		if (name[4] >= '1' && name[4] <= '6') {
			Node *newnode;
			newnode = (Node *)malloc(sizeof(Node));
			if (front->next == NULL) {
				front->next = newnode;
			}
			newnode->pid = ++PID;
			newnode->Q_time = Q_time;
			newnode->W_time = 0;
			strcpy(newnode->name, name);
			strcpy(newnode->state, "TASK_READY");

			char *stack = (char *)malloc(8192);
			getcontext(&newnode->task);
			newnode->task.uc_stack.ss_sp = stack;
			newnode->task.uc_stack.ss_size = 8192;
			newnode->task.uc_stack.ss_flags = 0;
			newnode->task.uc_link = &end;
			switch (name[4]) {
			case '1':
				makecontext(&newnode->task, task1, 0);
				break;
			case '2':
				makecontext(&newnode->task, task2, 0);
				break;
			case '3':
				makecontext(&newnode->task, task3, 0);
				break;
			case '4':
				makecontext(&newnode->task, task4, 0);
				break;
			case '5':
				makecontext(&newnode->task, task5, 0);
				break;
			case '6':
				makecontext(&newnode->task, task6, 0);
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
void add_ready_q(Node *newnode)
{
	assert(newnode->lnext == NULL);
	if (lfront == lrear) {
		lfront->lnext = newnode;
		lrear = newnode;
	} else {
		lrear->lnext = newnode;
		lrear = newnode;
	}
	strcpy(newnode->state, "TASK_READY");
	newnode->S_time = getCurrentTime();

	// printf("in add_ready_Q:pid= %d, W_time= %d\n", newnode->pid, newnode->W_time);
	// if (lfront->lnext == NULL)
	// 	lfront->lnext = newnode;
	// newnode->lnext = NULL;

	// if (lrear != NULL) {
	// 	lrear->lnext = newnode;
	// }

	// lrear = newnode;
}
Node *del_ready_q()
{
	struct node *ret_val = lfront->lnext;
	if (lfront->lnext == lrear) {
		lrear = lfront;
		lfront->lnext = NULL;
	} else {
		lfront->lnext = ret_val->lnext;
	}

	ret_val->lnext = NULL;
	ret_val->W_time += (getCurrentTime() - ret_val->S_time);
	return ret_val;

	// Node *delnode;
	// if (lfront->lnext == NULL) {
	// 	//		printf("Queue is empty!\n ");
	// 	return (NULL);
	// }
	// delnode = lfront->lnext;
	// lfront->lnext = delnode->lnext;
	// delnode->lnext = NULL;

	// if (delnode->lnext == NULL) {
	// 	lrear = NULL;
	// }
	// //renew Queueing time
	// delnode->W_time += (getCurrentTime() - delnode->S_time);
	// printf("del = %d\n", delnode->pid);
	// return (delnode);
}
void rm_ready_q(int pid)
{
	Node *rmnode;
	Node *tmpnode;
	if (lfront->lnext == NULL)
		return;
	tmpnode = lfront;
	while (tmpnode->lnext != NULL) {
		rmnode = tmpnode->lnext;
		if (rmnode->pid == pid && lfront->lnext->lnext == NULL) {
			lfront->lnext = NULL;
			rear = (Node *)malloc(sizeof(Node));
			rear->lnext = NULL;
			return;
		} else if (rmnode->pid == pid) {
			tmpnode->lnext = rmnode->lnext;
			rmnode->lnext = NULL;
			return;
		}
		tmpnode = tmpnode->lnext;
	}

}
void removeq(int pid)
{
	Node *rmnode;
	Node *tmpnode;
	rm_ready_q(pid);
	if (front->next == NULL) {
		printf("List is empty, you have nothing to remove!\n ");
		return;
	}
	tmpnode = front;
	while (tmpnode->next != NULL) {
		rmnode = tmpnode->next;
		if (rmnode->pid == pid && front->next->next == NULL) {
			front->next = NULL;
			rear = (Node *)malloc(sizeof(Node));
			rear->next = NULL;
			free(rmnode->task.uc_stack.ss_sp);
			free(rmnode);
			return;
		} else if (rmnode->pid == pid) {
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
	Node *tmpnode;
	tmpnode = front->next;
	printf("********All Queue **********\n");
	while (tmpnode != NULL) {
		printf("%-3d%s  %-18s%d\n", tmpnode->pid, tmpnode->name, tmpnode->state,
		       tmpnode->W_time);
		tmpnode = tmpnode->next;
	}
	printf("********Ready Queue**********\n");
	tmpnode = lfront->lnext;
	while (tmpnode != NULL) {
		//puts("RQ");
		printf("%-3d%s  %-18s%d\n", tmpnode->pid, tmpnode->name, tmpnode->state,
		       tmpnode->W_time);
		tmpnode = tmpnode->lnext;
	}
}
void hw_suspend(int msec_10)
{
	set_timer(0);
	strcpy(now->state, "TASK_WAITING");
	now->Sleep_time = msec_10 * 10;
	Node *tmp = now;
	now = NULL;
	swapcontext(&tmp->task, &start);
	return;
}

void hw_wakeup_pid(int pid)
{
	Node *tmpnode;
	tmpnode = front->next;
	while (tmpnode != NULL) {
		if (tmpnode->pid == pid)
			add_ready_q(tmpnode);
		tmpnode = tmpnode ->next;
	}
	return;
}

int hw_wakeup_taskname(char *task_name)
{
	Node *tmpnode;
	int wake_num = 0;
	tmpnode = front->next;
	while (tmpnode != NULL) {
		if (!strcmp(tmpnode->name, task_name)) {
			add_ready_q(tmpnode);
			++ wake_num;
		}
		tmpnode = tmpnode ->next;
	}
	return wake_num;
}

int hw_task_create(char *task_name)
{
	if (strlen(task_name) == 5 && !strncmp(task_name, "task", 4)
	    && task_name[4] >= '1'
	    && task_name[4] <= '6') {
		addq(task_name, 10);
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
	int Q_time = 0;
	char c;
	int pid;
	while (printf("$")) {
		scanf("%s", tmp);
		if (!strcmp(tmp, "add")) {
			scanf("%s", name);
			scanf("%c", &c);
			if (c != '\n') {
				scanf("%s", tmp);
				if (!strcmp(tmp, "-t")) {
					scanf("%s", tmp);
					if (!strcmp(tmp, "L"))    Q_time = 20;
					else if (!strcmp(tmp, "S"))    Q_time = 10;
					else {
						printf("Input error.\n");
						while (getchar() != '\n');
						continue;
					}
				} else {
					printf("Input error.\n");
					continue;
				}
			} else    Q_time = 10;
			addq(name, Q_time);
		} else if (!strcmp(tmp, "remove")) {
			scanf("%d", &pid);
			removeq(pid);
		} else if (!strcmp(tmp, "start")) {
			simulating();
		} else if (!strcmp(tmp, "ps")) {
			showq();
		} else {
			printf(" %s: command not found \n", tmp);
			continue;
		}
	}
}
long getCurrentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
void handler(int sig_num)
{
	switch (sig_num) {
	case SIGALRM:
		printf("Time is up!\n");
		dormerq();
		break;
	case SIGTSTP:
		printf("Catch Ctrl+Z\n");
		//renew_time();
		if (now != NULL) {
			add_ready_q(now);
			/*       printf("********RQ**********\n");
			       Node* tmpnode;
			       tmpnode = lfront->lnext;
			       while(tmpnode != NULL) {
			           printf("%-3d%s  %-18s%d\n",tmpnode->pid,tmpnode->name,tmpnode->state,
			                   tmpnode->W_time);
			           tmpnode = tmpnode->lnext;
			       }
			  */
			now = NULL;
			set_timer(0);
		}
		shell();
		break;
	default:
		printf("SIGNAL ERROR.\n");
		break;
	}
}
//Timer
void dormerq()
{
	if (now == NULL)
		setcontext(&start);
	printf("dormer pid = %d\n", now->pid);
	Node *tmpnode;
	tmpnode = front->next;
	while (tmpnode != NULL) {
		if (!strcmp(tmpnode->state, "TASK_WAITING")) {
			tmpnode->Sleep_time -= now->Q_time;
			/*wake up task*/
			if (tmpnode->Sleep_time <= 0)
				add_ready_q(tmpnode);
		}
		tmpnode = tmpnode->next;
	}
	add_ready_q(now);
	puts("in dormer,after add , before del.\n");
	Node *run_node = del_ready_q();
	strcpy(run_node->state, "TASK_RUNNING");
	now = run_node;
	set_timer(run_node->Q_time);
	// printf("swp %d / %d\n", lrear->pid, now->pid);
	swapcontext(&lrear->task, &now->task);
	printf("dormer done. pid =%d\n", now->pid);
}
void set_timer(int Q_time)
{
	/*timer setting*/
	//struct itimerval new_value, old_value;
	new_value.it_value.tv_sec = 0;
	new_value.it_value.tv_usec = Q_time * 1000;
	new_value.it_interval.tv_sec = 0;
	new_value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &new_value, &old_value);
}
void renew_time()
{
	Node *tmpnode;
	tmpnode = front->next;
	while (tmpnode != NULL) {
		if (!strcmp(tmpnode->state, "TASK_READY"))
			tmpnode->W_time += (getCurrentTime() - now->S_time);
		if (!strcmp(tmpnode->state, "TASK_WAITING")) {
			tmpnode->Sleep_time -= now->Q_time;
			/*wake up task*/
			if (tmpnode->Sleep_time <= 0)
				add_ready_q(tmpnode);
		}
		tmpnode = tmpnode->next;
	}
	puts("yeah");
}
bool task_exist()
{
	/*check no task*/
	Node *tmpnode;
	tmpnode = front->next;
	while (tmpnode != NULL) {
		if (strcmp(tmpnode->state, "TASK_TERMINATED"))
			return 1;
		tmpnode = tmpnode ->next;
	}
	return 0;
}
void simulating()
{
	/*everyone get start time*/
	Node *tmpnode;
	tmpnode = lfront->lnext;
	while (tmpnode != NULL) {
		tmpnode->S_time = getCurrentTime();
		printf("pid= %d,S_time= %d", tmpnode->pid, tmpnode->S_time);
		tmpnode = tmpnode ->next;
	}
	printf("simulating...\n");
	getcontext(&start);
	if (!task_exist())   shell();
	/*check ready Queue*/
	Node *run_node = del_ready_q();
	if (run_node == NULL) { //no running, some waiting
		usleep(10000);
		Node *tmpnode;
		tmpnode = front->next;
		while (tmpnode != NULL) {
			if (!strcmp(tmpnode->state, "TASK_WAITING"))
				tmpnode->Sleep_time -= 10;
			if (tmpnode->Sleep_time <= 0)
				add_ready_q(tmpnode);
			tmpnode = tmpnode ->next;
		}
		setcontext(&start);
	} else {
		now = run_node;
		strcpy(run_node->state, "TASK_RUNNING");
		set_timer(run_node->Q_time);
		assert(setcontext(&run_node->task) != -1);
	}
	return;
}
void termination()
{
	getcontext(&end);
	set_timer(0);
	if (now == NULL) return;
	strcpy(now->state, "TASK_TERMINATED");
	now = NULL;
	setcontext(&start);
}
void signal_init()
{
	struct sigaction stp;
	stp.sa_handler = &handler;
	sigemptyset(&stp.sa_mask);
	sigaddset(&stp.sa_mask, SIGALRM);
	sigaddset(&stp.sa_mask, SIGTSTP);
	stp.sa_flags = 0;
	assert(sigaction(SIGALRM, &stp, NULL) == 0);
	assert(sigaction(SIGTSTP, &stp, NULL) == 0);
}
int main()
{
	signal_init();
	creatq();
	termination();
	shell();
	return 0;
}
