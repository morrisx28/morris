#ifndef __D_QUEUE_H__
#define __D_QUEUE_H__


typedef struct 
{
   char *nextIn;		
   char *nextOut;
   char *first;
   char *last;
   int maxDataSize;
} DQueue;

class TDQueue{

private:
   char* nextIdx( DQueue *q, char* ptr);
   char* previousIdx( DQueue *q, char* ptr);
public:
   TDQueue();
   ~TDQueue();
   void DQueueBackoffItem(DQueue *q, int n) ;
   void DQueueEmptyQueue(DQueue *q);
   void DQueueInit(DQueue *q, char *buf, int len);
   int DQueueAddItem(DQueue *q, char in);
   int DQueueAddItems( DQueue *q, char* data, const int size ) ;
   int DQueuePeekInQueues( DQueue* q, char* outBuf, int maxBufSize ) ;
   int DQueueRemoveItems( DQueue* q, char* outBuf, int maxBufSize ) ;
   int DQueueDataSize(DQueue *q) ;
   int DQueueIsQueueNotEmpty(DQueue *q);
   int DQueueFreeSize(DQueue *q);
   void DQueueClose(DQueue *q);
   void DQueueDebugDump( DQueue *q ) ;
   int DQueuePopLine( DQueue* q, char* outBuf, int maxBufSize );
   int DQueuePopMsg( DQueue* q, char* outBuf, int maxBufSize );


};


#endif