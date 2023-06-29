#include <stdio.h>
#include<omp.h>
#include <stdlib.h>
#include<limits.h>
#include<stdbool.h>
int numvertex;
typedef struct Node {
    int data;
    int coreness;
    bool in_given_core;
    bool change;
} Node;

typedef struct LinkedList {
    Node** array;  // Array of pointers to the nodes
    int size;
} LinkedList;

typedef struct Graph{
	int numVertices;
	struct LinkedList** adjLists;
}Graph;

Node* createNode(int value);

Node** createListofNodes(){	
int i;
Node** nodearray = malloc(sizeof(Node*)*numvertex);
for(i=0;i<numvertex;i++){
	nodearray[i]=createNode(i);
}
return nodearray;
}

LinkedList* createLinkedList() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->array = NULL;
    list->size = 0;
    return list;
}

Graph* createAGraph(int vertices){
	Graph* graph = malloc(sizeof( Graph));
	graph->numVertices = vertices;
	graph->adjLists = malloc(vertices*sizeof(LinkedList*));
	int i;
	for(i=0;i<vertices;i++)
		graph->adjLists[i]=NULL;
	return graph;
}

Node* createNode(int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = value;
    newNode->change = true;
    newNode->in_given_core = true;
    return newNode;
}


void insertAtBeginning(LinkedList* list, int value, Node** nodearray) {
    // Update the array of pointers
    list->array = realloc(list->array, (list->size + 1) * sizeof(Node*));
    list->array[list->size] = nodearray[value];
    list->size++;
}

Node* getNodeByIndex(LinkedList* list, int index) {
    if (index >= 0 && index < list->size) {
        return list->array[index];
    }
    return NULL;
}


void addEdge(Graph* graph, int s, int d,Node** nodearray){
	if(graph->adjLists[s]==NULL){
		graph->adjLists[s] = createLinkedList();
	}
	if(graph->adjLists[d]==NULL){
		graph->adjLists[d] = createLinkedList();
	}
	insertAtBeginning(graph->adjLists[s],d,nodearray);
	insertAtBeginning(graph->adjLists[d],s,nodearray);
}	

void k_core_decomposition(Graph* graph, Node** nodearray, int core){
	int j,i;
	bool changed=true;
	for(i=0;i<numvertex;i++){
		nodearray[i]->coreness = graph->adjLists[i]->size;
	}
	while(changed){
		changed=false;
		for(i=0;i<numvertex;i++){
			Node* node = nodearray[i];
			if(node->change && node->in_given_core ){
				node->change=false;
				if(node->coreness<core){			
					node->in_given_core=false;
					Node** neighbours = graph->adjLists[i]->array;
					int size = graph->adjLists[i]->size;
					for(j=0;j<size;j++){
						//below should be atomic
						neighbours[j]->coreness--;
						neighbours[j]->change=true;
						changed=true;
					}
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {
	numvertex = atoi(argv[1])+1;
	int source = atoi(argv[2]);
    Node** nodearray;
    nodearray = createListofNodes();
    int i;
    int src,dest;
    int core=32;
    Graph* graph = createAGraph(numvertex);
    FILE* file;
    file = fopen(argv[3],"r");
    if(file==NULL){
	    printf("Nope");
	    exit(0);
    }
    //printf("Adding edges\n");
    while(fscanf(file, "%d %d", &src, &dest) == 2){
	    addEdge(graph,src,dest,nodearray);
  //  printf("\nadding %d and %d",src,dest);
    }
 printf("Added all edges\n");
 printf("Begin the algo has\n");
	k_core_decomposition(graph, nodearray, core);
	printf("algo done\n");
	printf("\n");
	for(i=0;i<numvertex;i++){
		if(nodearray[i]->in_given_core){
			printf("%d, ",i);
		}
	}
printf("\n");	
    return 0;
}

