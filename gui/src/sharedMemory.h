#ifndef __SHAREDMEMORY_H__
#define __SHAREDMEMORY_H__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SEG_SIZE 0x6400

class sharedMemory{
	
	public:
	
	int get(int key);
	char* attach(int sh_id);
	void detach(char* shm);
	int getSize(int sh_id);
	void destroy(int sh_id);
	
	private:
	
	struct shmid_ds shmbuffer;
};





#endif // __SHAREDMEMORY_H__
