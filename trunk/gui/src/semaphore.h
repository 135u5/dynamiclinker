#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>


union semun{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};


class semaphore{
	
public:

int get(int key);
void initialize(int sem, int value);
void wait(int sem, int val);
void post(int sem, int val);
void destroy(int sem);

private:
union semun args, no_args;
unsigned short val[1];
struct sembuf wait_op[1],post_op[1];

};
#endif // __SEMAPHORE_H__
