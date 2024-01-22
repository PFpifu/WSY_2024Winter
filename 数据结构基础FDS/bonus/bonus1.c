/*
Author : Wu Siyu
ID: 3220105846
No.01
 */ 
#include <stdio.h>
#include <stdlib.h>

typedef struct _stack*  stack;
struct _stack
{
    int tp;
    int sa[110];
};
void push (stack s, int element);
int pop (stack s);
int main()
{
    int i, j, temp, count, inst;
    int N;
    char ope;
    stack s1, s2;
    s1 =(stack)malloc(sizeof(struct _stack));
    s2 =(stack)malloc(sizeof(struct _stack));
    s1->tp = s2->tp = -1; //-1 means stack is empty.
    for(i=0; i<110; i++) s1->sa[i] = s2->sa[i] = 0;
    scanf("%d\n", &N);
    for(i=0; i<N; i++)
    {
        count = 0;
        scanf("%c", &ope);
        if(ope == 'I')
        {
            scanf("%d\n", &temp);
            push(s1, temp);
        } else if (ope == 'O')
        {
        	scanf("\n");
            if(s2->tp == -1)
            {
                if(s1->tp == -1)
                {
                    printf("ERROR\n");
                    break;
                } else {
                    while (s1->tp != -1)
                    {
                        inst = pop (s1);
                        count++;
                        push (s2, inst);
                        count++;
                    }
                    inst = pop (s2);
                    count++;
                }
            } else {
                inst = pop (s2);
                count++;
            }
            printf("%d %d\n",inst, count);
        }
    }

    return 0;
}

void push (stack s, int element)
{
    s->tp++;
    s->sa[s->tp] = element;
}

int pop (stack s)
{
    if(s->tp == -1) return -1;
    int element = s->sa[s->tp];
    s->tp -- ;
    return element;
}
