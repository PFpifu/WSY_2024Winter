#include <stdio.h>
#include <stdlib.h>

typedef struct Lgraph* Graph;


struct Lgraph
{
	int adjm[1010][1010];  //store the edges in adjacent matrix.
	int Nv, Ne;  //number of vertices and edges.
};

typedef struct 
{
	int known;  //the vertex is visited or not.
	int dist;  //the distance from start vertex.
	int path;  
    int adj[1010];  //the adjacent vertices.
} LTable;

LTable* InitTable(Graph G);
int IsDijk (LTable* T, Graph G, int seq[]);

int main()
{
	int i, j, k, m, n;
	scanf("%d %d",&m, &n);
	Graph G = (Graph)malloc(sizeof(struct Lgraph));
	G->Nv = m;
	G->Ne = n;
	
	for(i=0; i<1010; i++)
	{
		for(j=0; j<1010; j++)
		{
			G->adjm[i][j] = 0;  //initialize the adjacent matrix.
		}
	}
	
	int temp1, temp2, temp3;
	for(i=0; i<G->Ne; i++)
	{
		scanf("%d %d %d", &temp1, &temp2, &temp3);
		G->adjm[temp1][temp2] = G->adjm[temp2][temp1] = temp3;  //read the edges and store it into the matrix.
	}
	
	scanf("%d", &k);  //k test cases.
	int seq[G->Nv];  //the specific sequence of a test case, containing V elements.
	for(i=0; i<k; i++)
	{
		for(j=0; j<G->Nv; j++) scanf("%d", &seq[j]);
		LTable* T = InitTable (G);  //change the graph to the table.
		if(IsDijk(T, G, seq) == 1) printf("Yes\n");  //judgement.
		else printf("No\n");
	}
	
	return 0;
}

LTable* InitTable(Graph G)
{
	int i, j;
	int* p;
	LTable T1[G->Nv+1];  //the output structure array.
	for(i=0; i<=G->Nv; i++)
	{
		T1[i].known = 0;  //set every vertex to unknown.
		T1[i].dist = 1010000;  //set every distance to infinity.
		T1[i].path = 0;  //set every vertex to distinct.
		p = T1[i].adj;
		for(j=0; j<=G->Nv; j++)
		{
			T1[i].adj[j] = 0;
			if(G->adjm[i][j] != 0)  //if i and j are adjacent.
			{
				*p = j;
				p++;
			}  //if vertex has adjacent vertices, store them in adjacent array.
		}
	}
	LTable* tempT = T1; 
	return tempT;
}

int IsDijk (LTable* T, Graph G, int seq[])
{
	int i, j, min, temp;
	T[seq[0]].dist = 0;  //set the start vertex's distance to 0.

    for(i=0; i<G->Nv; i++)
    {
        temp = 1;  //temp is the minimum-distance and unknown vertex.
        min = 1010000;  //set min to infinity initially.
        for(j=1; j<=G->Nv; j++)
        {
            if(T[j].known == 0 && T[j].dist < min)
            {
                temp = j;
                min = T[j].dist;
            }  //choose the the minimum-distance and unknown "temp".
        }
        T[temp].known = 1;  //temp is visited.
        if(T[seq[i]].dist != min) return 0;  //if the distance of given sequence is not equal to shortest path, this is not a Dijkstra sequence.

        for(j=0; T[temp].adj[j]!=0; j++)
        {
            if(T[T[temp].adj[j]].known == 0 && T[T[temp].adj[j]].dist > T[temp].dist + G->adjm[temp][T[temp].adj[j]])
                T[T[temp].adj[j]].dist = T[temp].dist + G->adjm[temp][T[temp].adj[j]];  //update the diatance of adjacent vertices.
        }
    }
	return 1;
}
