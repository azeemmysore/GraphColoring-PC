#include<stdio.h>
#include<stdlib.h>
#include<omp.h>


typedef struct adj 				//Structure of adjacency list
{
	int x;
	struct adj *next;
}Node;


Node* newNode(int val,Node *nextNode)
{
	Node *temp;
	temp= (Node *)malloc(sizeof(Node));
	temp->x=val;
	temp->next = nextNode;
	return temp;
}

void addAdjacentVertex (Node **list, int v, int w)
{
	Node *temp;
	v--;
	w--;

	temp=*(list+v);
	*(list+v)=newNode(w,temp);

	temp=*(list+w);
	*(list+w)=newNode(v,temp);


}

void printGraph (Node **list,int n)				// for debugging
{
	Node *temp;
	int i;

	for (i=0;i<n;i++)
	{
		printf("Node%d is adjacent to: ",i+1);
		temp=*(list+i);
		while (temp!=NULL)
		{
			printf("%d ",temp->x+1);
			temp=temp->next;
		}
		printf("\n");
	}
}

int min(int a, int b)
{
	return ((a<b)?a:b);
}



int main()
{
	Node **list;								//adjacency list
	int n,i,m,e1,e2,p,*color,j,*conflict;		//color[n] to store the color given to the vertices and conflict[n] to detect if there is any conflict of color between vertices
	printf("\nInput the no. of vertices: ");
	scanf("%d",&n);

	color=(int *)malloc (n*sizeof(int));
	conflict=(int *)malloc (n*sizeof(int));
	list=(Node **)malloc (n*sizeof(Node *));
	for (i=0;i<n;i++)
		list[i]=NULL;

	printf("\nInput the no. of edges: ");
	scanf("%d",&m);
	printf("\nInput the vertices of each edges: \n");
	for (i=0;i<m;i++)
	{
		scanf("%d%d",&e1,&e2);
		addAdjacentVertex(list,e1,e2);
	}

	//printGraph(list,n);


	#pragma omp parallel
	{
		Node *temp;
		int availablecol[n],j;
		

		#pragma omp for
		for (i = 0; i < n; ++i)
		{
			color[i]=-1;
			conflict[i]=0;
		}


		#pragma omp for
		for (i=0;i<n;i++)						//first phase of the algorithm: assigning colors to all nodes parallely
		{
				for (j=0;j<n;j++)
					availablecol[j]=1;
				temp=*(list+i);
				while (temp!=NULL)
				{
					if (color[temp->x]>=0)
						availablecol[color[temp->x]]=0;
					temp=temp->next;
				}
				for (j=0;j<n;j++)
				{
					if (availablecol[j])
					{
						break;
					}
					if (j==4)
						printf("Something's wrong.\n");
				}
				color[i]=j;
		}


		#pragma omp for
		for (i=0;i<n;i++)					//second phase of the algorithm: detecting if there's any conflict of colors between nodes which are assinged colors parallely
		{
			temp=*(list+i);
			while(temp!=NULL)
			{
				if (color[temp->x]==color[i])
				{
					conflict[min(temp->x,i)]=1;
					// printf("%d %d\n",temp->x,i);		
				}
				temp=temp->next;
			}
		}
	}

	/*for (i=0;i<n;i++)
		printf("%d %d\n",color[i],conflict[i]);
	printf("\n");*/

	Node *temp;
	int availablecol[n];
									//third phase of the algorithm: solving the conflicts sequentially
	for (i=0;i<n;i++)
	{
		if (conflict[i])
		{
			for (j=0;j<n;j++)
					availablecol[j]=1;
			temp=*(list+i);
			while (temp!=NULL)
			{
				if (color[temp->x]>=0)
					availablecol[color[temp->x]]=0;
				temp=temp->next;
			}
			for (j=0;j<n;j++)
			{
				if (availablecol[j])
				{
					break;
				}
			}
			color[i]=j;
		}
	}

	for (i=0;i<n;i++)
		printf("%d %d\n",i,color[i]);
	printf("\n");
	return 0;
}
