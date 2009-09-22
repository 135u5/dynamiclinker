#include "sharedMemory.h"

int sharedMemory :: get(int key){
	if(key==0)
		return shmget(IPC_PRIVATE, (const int)SEG_SIZE,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
	else
		return shmget(key, (const int)SEG_SIZE,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
}

char* sharedMemory :: attach(int sh_id){

	return (char*) shmat(sh_id,0,0);

}

void  sharedMemory :: detach(char* shm){

	shmdt (shm);

}

int  sharedMemory :: getSize(int sh_id){

	shmctl(sh_id,IPC_STAT, &shmbuffer);
	return shmbuffer.shm_segsz;
}

void  sharedMemory ::destroy(int sh_id){
	
	shmctl(sh_id,IPC_RMID,0);
	
}