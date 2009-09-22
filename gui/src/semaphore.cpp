#include "semaphore.h"



int semaphore:: get(int key){
	if (key==0)
		return semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
	else 
		return semget(key,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
}

void semaphore::initialize(int sem, int value){
	val[0]=value;
	args.array=val;
	semctl(sem,0,SETALL,args);
}
void semaphore::wait(int sem, int val){
	wait_op[0].sem_num=0;
	wait_op[0].sem_op=-val;
	wait_op[0].sem_flg=SEM_UNDO;
	semop(sem,wait_op,1);
}
void semaphore::post(int sem, int val){
	post_op[0].sem_num=0;
	post_op[0].sem_op=val;
	post_op[0].sem_flg=SEM_UNDO;
	semop(sem,post_op,1);
}

void semaphore::destroy(int sem){
	semctl(sem,1,IPC_RMID,no_args);
}