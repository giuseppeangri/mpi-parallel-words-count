extern int    CounterContainer_calculateSendPackSize(CounterContainer * entriesContainer);
extern int    CounterContainer_calculateRecvPacksBufferSize(int num_processes, int master_rank, int * packsSizes);
extern void   CounterContainer_calculateDisplacements(int num_processes, int master_rank, int * packsSizes, int * displacements);
extern void * CounterContainer_makeSendPackBuffer(CounterContainer * entriesContainer, int sendPackSize);
extern void   CounterContainer_merge(CounterContainer * entriesContainer, void * recvBuffer, int recvBufferSize, int numContainers);