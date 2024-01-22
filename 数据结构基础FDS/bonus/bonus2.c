/*
Author : Wu Siyu
ID: 3220105846
No.02
 */ 
 
#include <stdio.h>
#include <stdlib.h>

typedef struct _heap* Heap;
struct _heap{
	int H[100010];
	int size;
};

void Insert (Heap heap, int x);
int DeleteMin (Heap heap);

int main()
{
	int N, M;
	scanf("%d %d", &N, &M);
	int seq[N+10];
	int i, j, p, temp;
	for(i=1; i<=N; i++) scanf("%d", &seq[i]);
	Heap heap1, heap2;
	heap1 = (Heap)malloc(sizeof(struct _heap));
	heap2 = (Heap)malloc(sizeof(struct _heap));
	heap1->H[0] = heap2->H[0] = -100000;
	heap1->size = heap2->size = 0;
	
	for(i=1; i<=M; i++) Insert(heap2, seq[i]);
	p = M+1;  //p is the pointer of seq[];
	while(heap2->size == M)
	{
		heap1->size = heap2->size;
		for(i=1; i<=heap1->size; i++) heap1->H[i] = heap2->H[i];
		while(heap2->size != 0) temp = DeleteMin(heap2);
		int res[N+10];
		int rescount = 0;
		temp = heap1->H[1];  //heap top;
		while(heap2->size != M && p <= N)
		{
			int t = DeleteMin(heap1);
			res[++rescount] = t;
			temp = t;
			int t2 = seq[p++];
			if(t2>=temp) Insert(heap1, t2);
			else Insert(heap2, t2);
		}
		while(heap1->size != 0)
		{
			res[++rescount] = DeleteMin(heap1);
		}
		for(i=1; i<=rescount; i++) printf("%d%c", res[i], (i!=rescount) ? ' ' : '\n');
	}
	if(heap2->size != 0)
	{
		int res[N+10];
		int rescount = 0;
		while(heap2->size != 0) res[++rescount] = DeleteMin(heap2);
		for(i=1; i<=rescount; i++) printf("%d%c", res[i], (i!=rescount) ? ' ' : '\n');
	}
	return 0;
	
	
}

void Insert (Heap heap, int x)
{
	int i;
	for(i=++heap->size; heap->H[i/2]>x; i/=2) heap->H[i] = heap->H[i/2];
	heap->H[i] = x;
	return ;
}

int DeleteMin (Heap heap)
{
	int i, child, min, last;
	if(heap->size == 0) return 0;
	min = heap->H[1];
	last = heap->H[heap->size--];
	
	
	for(i=1; i*2<=heap->size; i=child)
	{
		child = i*2;
		if(child!=heap->size && heap->H[child+1]<heap->H[child])
			child++;
		if(last>heap->H[child]) heap->H[i] = heap->H[child];
		else break;
	}
	heap->H[i] = last;
	return min;
	
}
