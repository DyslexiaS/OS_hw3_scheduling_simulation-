# OS_hw3_scheduling_simulation-

## add 
$ add TASK_NAME -t TIME_QUANTUM
- TASK_NAME : What task to add  (task1~task6)
- Optional argument 
    - L for the larger time quantum: 20 ms
    - S for the small time quantum: 10 m
    - Default value is 
    
## remove
$ remove PID
- Remove a task with PID

## $ start
simulating

## ps
when simulating… ,you can do Ctrl + z to pause the simulation and switch to shell mode.
$ ps 
- 1    task1    TASK_READY         50 
- 2    task2    TASK_TERMINATED    10 
- 3    task2    TASK_READY         50
- 4    task3    TASK_WAITING       50
PID Task name                      Task state Queueing time
