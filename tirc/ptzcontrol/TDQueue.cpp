


#include "inc/TDQueue.h"
#include <string.h>

char debugMsg[128] ;

TDQueue::TDQueue(){

}

TDQueue::~TDQueue(){
	
}

char *TDQueue::nextIdx( DQueue *q, char* ptr ) {
	return ptr == q->last ? q->first : ( ptr+1 );
}

char *TDQueue::previousIdx( DQueue *q, char* ptr ) {
	return ptr == q->first ? q->last : ( ptr-1 );
}

void TDQueue::DQueueBackoffItem(DQueue *q, int n)
{
	if ( (q->nextIn - n) < q->first )
		q->nextIn = q->last - ( n - (q->nextIn - q->first) );
	else
		q->nextIn -= n;
}

void TDQueue::DQueueEmptyQueue(DQueue *q)
{
   q->nextIn = q->nextOut = q->first;
   q->maxDataSize = 0;
}

void TDQueue::DQueueInit(DQueue *q, char *buf, int len)
{
   q->nextIn = q->nextOut = q->first = buf;	
   q->last = buf + len - 1;
   q->maxDataSize = 0;
}

int TDQueue::DQueueAddItem(DQueue *q, char in)
{
	if ( nextIdx( q, q->nextIn ) == q->nextOut ) 
		return 0 ;
  *(q->nextIn) = in;
  q->nextIn = nextIdx( q, q->nextIn );
	return 1;
}

int TDQueue::DQueueAddItems( DQueue *q, char* data, const int size ) {
	int copySize;
	int remainSize = size;

	if ( DQueueFreeSize( q ) < size )
		return 0;

	if( q->nextIn >= q->nextOut ) {
		copySize = q->last - q->nextIn + 1;
		if( remainSize <= copySize ) {
			memcpy(q->nextIn, data, remainSize);
			if( remainSize==copySize )
				q->nextIn = q->first;
			else
				q->nextIn += remainSize;
			return size;
		}
		// remainSize > copySize
		memcpy(q->nextIn, data, copySize);
		remainSize -= copySize;
		q->nextIn = q->first;
		data += copySize;
	}

	memcpy(q->nextIn, data, remainSize);
	q->nextIn += remainSize;
	return size ;
}

int TDQueue::DQueuePeekInQueues( DQueue* q, char* outBuf, int maxBufSize ) {
	int copySize, remainReadSize;
	char* nextOut = q->nextOut ;
	const int readSize = ( maxBufSize > DQueueDataSize(q) ) ? DQueueDataSize(q) : maxBufSize ;
	remainReadSize = readSize;
	if (nextOut > q->nextIn) {
		copySize = q->last - nextOut +1;
		if( remainReadSize<=copySize ) {
			memcpy(outBuf, nextOut, remainReadSize);
			return readSize;
		}
		// remainReadSize>copySize
		memcpy(outBuf, q->nextOut, copySize);
		remainReadSize -= copySize;
		nextOut = q->first;
		outBuf += copySize;
	}

	memcpy(outBuf, nextOut, remainReadSize);
	return readSize;
}

int TDQueue::DQueuePopLine( DQueue* q, char* outBuf, int maxBufSize ) {
	char *ptr ;
	int x ;
	ptr = q->nextOut ;
	for( x=0; x<DQueueDataSize(q); x++ ) {
		if ( *ptr == '\n' ) {
			return DQueueRemoveItems( q, outBuf, ((x+1)<maxBufSize)?x+1:maxBufSize ) ;	
		}
		ptr = nextIdx( q, ptr ) ;
	}
	return 0 ;
}
/*
  ===head===  ==MsgID==  =DataSize=  ===data===  ===tail===
  0xA0, 0xA2,    0xXX,   0xXX, 0xXX, .........., 0xB0, 0xB3
  message size = dataSize + 7
*/
int TDQueue::DQueuePopMsg( DQueue* q, char* outBuf, int maxBufSize ) {
	int readSize, dataSize ;
	while ( (readSize=DQueuePeekInQueues( q, outBuf, maxBufSize )) > 7 ) {
		if ( (unsigned char) outBuf[0] != 0xA0 || (unsigned char) outBuf[1] != 0xA2 ) {
			DQueueRemoveItems(q, NULL, 1 ) ;
			continue ;
		}

		dataSize = ((unsigned char)outBuf[2]) << 8 | ((unsigned char)outBuf[3]) ;
		if ( dataSize+7 > maxBufSize ) {
			DQueueRemoveItems(q, NULL, 1 ) ;
			continue ;
		}

		if ( DQueueDataSize(q) < dataSize+7 )
			return 0 ;

		DQueuePeekInQueues(q, outBuf, dataSize+7 ) ;

		if ( (unsigned char) outBuf[dataSize+5] != 0xB0 || (unsigned char) outBuf[dataSize+6] != 0xB3 ) {
			DQueueRemoveItems(q, NULL, 1 ) ;
			continue ;
		}
		
		DQueueRemoveItems(q, NULL, dataSize+7) ;

		return dataSize+7;
	}

	return 0 ;
}

int TDQueue::DQueueRemoveItems( DQueue* q, char* outBuf, int maxBufSize ) {
	int copySize, remainReadSize;
	int readSize = ( maxBufSize > DQueueDataSize(q) )? DQueueDataSize(q): maxBufSize  ;

	remainReadSize = readSize;
	if (q->nextOut > q->nextIn) {
		copySize = q->last - q->nextOut +1;
		if( remainReadSize<=copySize ) {
			if ( outBuf != NULL )
				memcpy(outBuf, q->nextOut, remainReadSize);
			if( remainReadSize==copySize )
				q->nextOut = q->first;
			else
				q->nextOut += remainReadSize;
			return readSize;
		}
		// remainReadSize>copySize
		if ( outBuf != NULL ) {
			memcpy(outBuf, q->nextOut, copySize);
			outBuf += copySize;
		}
		remainReadSize -= copySize;
		q->nextOut = q->first;
	}

	// here, several conditions are known:
	// 1. q->nextOut==q->nextIn
	// 2. remainReadSize > 0 and remainReadSize <= (q->nextIn - q->nextOut)
	if ( outBuf != NULL )
		memcpy(outBuf, q->nextOut, remainReadSize);
	q->nextOut += remainReadSize;
	return readSize;
}

int TDQueue::DQueueDataSize(DQueue *q)
{
   if (q->nextOut > q->nextIn)
    return (q->nextIn - q->first) + (q->last - q->nextOut + 1);
   else
    return q->nextIn - q->nextOut;
}

int TDQueue::DQueueIsQueueNotEmpty(DQueue *q)
{
   return (q->nextIn != q->nextOut) ? 1 : 0;
}

int TDQueue::DQueueFreeSize(DQueue *q)
{
   if (q->nextOut > q->nextIn)
      return q->nextOut - q->nextIn - 1;
   else
      return (q->nextOut - 1 - q->first) + (q->last - q->nextIn + 1);

}

void TDQueue::DQueueClose(DQueue *q)
{
	/* This will set it back to the init stage */
	DQueueEmptyQueue(q);
}
