#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int min_width=1;	/* Nodes/level: WIDTH OF DAG.  */
int max_width=5;
int min_height=50;	/* Levels:HEIGHT OF DAG        */
int max_height=55;
int chance=30; 		/* Chance of having an Edge.  */


int main ()
{
  FILE *f=fopen("data","w");
  if(f==NULL)
{
	printf("ERROR OPENING FILE\n");
	exit(1);
}

  int i, j, k,nodes = 0;
  srand (time (NULL));

  int levels = min_height  //levels is between 1-100(epipeda)
              + (rand () % (max_height - min_height + 1));

  for (i = 0; i < levels; i++)
    {
      /* New nodes are 'higher' in level than all older nodes.  */
      int new_nodes = levels;

      /* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
      for (j = 0; j < nodes; j++)
        for (k = 0; k < new_nodes; k++)
          if ( (rand () % 100) < chance)
            fprintf (f,"%d %d\n",j,k + nodes); /* An Edge.  */
      nodes += new_nodes; /* So edges involve higher value nodes  */
	//so edge is nodes->new_nodes+nodes /no chance of circle
}
  printf ("TELOS \n");
  return 0;
}
