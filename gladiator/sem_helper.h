/****************
 * sem_helper.h
 *
 * Eric McGregor
 ****************/

int getSemaphoreID(int size, int tokid);
void freeSemaphores(int semid);

void setAllSemaphores(int semid, int size, int value);
void getAllSemaphores(int semid, int size, unsigned short* arr);

void setSemaphore(int semid, int index, int value);
int getSemaphore(int semid, int index);

void incrementSemaphore(int semid, int index);
void decrementSemaphore(int semid, int index);

void print_error(const char*, int);
