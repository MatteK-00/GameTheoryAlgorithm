#ifndef HEADER_FILE
#define HEADER_FILE

	#define MAX_LENGTH_STRING_VICINATO 1000 //massima dimensione della stringa di testo che contiene il vicinato di ogni nodo

	typedef struct edgeList {
	    int id;
	    int weight;
	    struct edgeList *next;
	    struct edgeList *prev;
	} edgeList;

	typedef struct node {
	    int id;
	    int player;
	    int out_degree;
	    int in_degree;
	    int priority;
	    int fixPoint;
	    char *label;
	    edgeList *out_neighbour;
	    edgeList *in_neighbour;
	} node;

	typedef struct graph {
	    node** nodes;
	    int N;
	    int max_priority;
	} graph;





	//Prototipi


	node* addNode(int id, int player, int priority, edgeList *out_neighbour, edgeList *in_neighbour, char *label);
	void addEdge(node **nodes, int out_id,int in_id, int weight);
	void printlist(edgeList *lista);

	void small_Energy_Progress_Measure(node** nodes, int N);
	void small_Progress_Measure(node** nodes, int N, int max_priority);



#endif