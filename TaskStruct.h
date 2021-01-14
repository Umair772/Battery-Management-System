struct TaskStruct {
  void (*myTask)(void*);
  void* taskDataPtr;
  struct TaskStruct* next;
  struct TaskStruct* prev;
};

typedef struct TaskStruct TCB;
