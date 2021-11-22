#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#define SIZE 15000 //size of graph(how many nodes)
#define CHUNK 20 
char str;
struct timeval start,end;
int i,nei;   //variable for loops
int cnt2; //counts how many tasks i can create
////////////////////	 STRUCTS		//////////


struct list{    //linked list
	int in_deg;
	int id;
	struct list *next;
};
typedef struct list list;


struct queue
{
	int count;
	list *front;
	list *rear;
};
typedef struct queue queue;



struct Graph{						//graph
	int V;							//num of nodes-vertices in graph
	list *adj[SIZE];
}; 
typedef struct Graph Graph;







//////////////////////	FUNCTIONS	///////////////////////////////







void addEdge(Graph g,int u,int v); //function to add edge to graph
void Sort();


list* createNode()	//node constructor
{ 
	list *temp;
        temp=(list*)malloc(sizeof(list)); //allocate memory
	temp->id=0;
        temp->in_deg=0;
	temp->next=NULL;        //next points to NULL
        return temp;
}

Graph createGraph()
{

        Graph *temp;                              //declare node
        temp=malloc(SIZE*sizeof(Graph)); 		  //allocate memory

	temp->V=SIZE;        					   	  //How many nodes are in the graph
        for(i=0; i<SIZE; i++)  
{
	list *n=createNode();
	temp->adj[i]=n;
	temp->adj[i]->id=i;

}


	return *temp;
}




queue* initialize() //initialize queue
{
	queue *temp;
	temp=malloc(sizeof(queue));
	temp->count=0;
	temp->front= NULL;
	temp->rear=NULL;
	return temp;
}




int isempty(queue *q)  //check if queue is empty
{
	if(q->count==0) {return 0;}
	return 1;
}




void enqueue(queue *q,int value)
{
	list *tmp;
	tmp=malloc(sizeof(list));
	tmp->id=value;
	tmp->next=NULL;
	if(isempty(q)!=0 )
{
	q->rear->next =tmp;
	q->rear=tmp;
}

else
{
	q->front =q->rear=tmp;
}

q->count++;

}




int dequeue(queue *q)
{

	int n=q->front->id;

	q->front=q->front->next;

	q->count--;


	return n;
}








 void addToList(list *head,int value)  //add node to list- like push..each node is a "list"
{
	list *temp,*p;
	temp=createNode();
	temp->id=value;

	if(head ==NULL)
{
	head=temp;	//when list is empty

}
	else{

		p=head;
		while(p->next !=NULL)
			{
			p=p->next;
			}
		p->next=temp;   //point the prev last node to new node we just made
		return;
	}
};




void addEdge(struct Graph g,int u,int val)	//add edges to graph
{
  	addToList(g.adj[u],val); 

	return;
}





void  Sort(Graph g)
{

list *itr=NULL;
int cnt=0;
int res[SIZE];
int k;

	//	list *itr=NULL;
                	                          //count and put in_Degrees
		for(i=0; i<SIZE; i++)			//as i progresses change vertival list
         	{
			itr=g.adj[i];

			while(itr->next)
	                {
				itr=itr->next;
									//move down the list
				g.adj[itr->id]->in_deg++; 		//put in_deg in specific ids
	                }
	        }



	//put all vertices in queue\\

	queue *q=initialize();  //make a queue to hold vertices with in_deg=0
	for(i=0; i<SIZE; i++)
	{
		itr=g.adj[i];

		if(itr->in_deg==0) //if in degree is 0 add to queue
			{
				enqueue(q,itr->id);
			}
	}





		   ////PARALLEL REGION \\\\


gettimeofday(&start,NULL);
cnt2=q->count;


	#pragma omp parallel
{
	#pragma omp single nowait
{
  	while(cnt<SIZE)
{

	if(cnt2<CHUNK)  {

			#pragma omp taskwait
			cnt2=q->count;
                        }
		cnt2-=CHUNK;  //a chunk will be given to the task to process


		#pragma omp task default(none) shared(res,g,q) private(itr) firstprivate(cnt) 
                {
	int i,k=0;
	for(i=cnt; i<cnt+CHUNK; i++)
	    {
		//extract from the top of the queue and add to order

		#pragma omp critical (q)
		k=dequeue(q);		//k=num of dequeued node
		res[i]=k;			//add k to results
	
		itr=g.adj[k];  		//itr goes to popped node to reduse in_deg to it's neigborsz


		while(itr->next) 	//reduse in_deg in all neigbors
		{
			itr=itr->next;

							//if the new in_Deg is 0 add to queue

							//2 critical regions with diff names instead of one results in better performance ->

			#pragma omp critical (g)
			if(--g.adj[itr->id]->in_deg==0)
				{
					#pragma omp critical (q)
					enqueue(q,itr->id);

				}//end of if
		}//end of while
	    }//end of for
	}//end of task
cnt+=CHUNK;

}//end of while
#pragma omp taskwait
}//end of single

}//end of parallel
gettimeofday(&end,NULL);



free(itr);


	if (cnt!=SIZE)
	{
		printf("There was a circle in the graph.\n");
		return;
	}


	//print order
	printf("The results are:\n");
	for (i=14950; i<SIZE; i++)
{
		printf(": %d\n",res[i]);
}

}

int main()
{

	int from,to;



	FILE* file=fopen("data","r");


	struct Graph g=createGraph();
	g.V=SIZE;



	while(fscanf(file,"%d %d",&from,&to)==2)

{
	addEdge(g,from,to);
}



	Sort(g);			//SORTING


long seconds=(end.tv_sec -start.tv_sec);
long micros = (end.tv_usec+seconds*1000000) - (start.tv_usec);

printf("The program took %ld mics to execute \n",micros);

return 0;
}





