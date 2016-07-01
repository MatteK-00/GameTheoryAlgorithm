#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "header.h"


void printlist(edgeList *lista) {
    while (lista != NULL) {
        printf(" %i %i ", lista->id,lista->weight);
        lista = lista->next;
    }
}

node* addNode(int id, int player, int priority, edgeList *out_neighbour, edgeList *in_neighbour, char *label) {
    node* nodo = (node*) malloc(sizeof (node));
    char* LABEL = malloc(sizeof(char)*strlen(label));
    strcpy(LABEL,label);
    nodo->id = id;
    nodo->player = player;
    nodo->out_degree = 0;
    nodo->priority = priority;
    nodo->out_neighbour = out_neighbour;
    nodo->in_neighbour = in_neighbour;
    nodo->label = LABEL;

    return nodo;
}

void addEdge(node **nodes, int out_id,int in_id, int weight) {
    edgeList* nextElement_out = (edgeList*) malloc(sizeof (edgeList));
    edgeList* nextElement_in = (edgeList*) malloc(sizeof (edgeList));

    nextElement_out->id = in_id; //nextElement->id = ID;
    nextElement_out->weight = weight;
    nextElement_out->next = nodes[out_id]->out_neighbour;
    nextElement_out->prev = NULL;
    nodes[out_id]->out_degree++;

    nextElement_in->id = out_id;
    nextElement_in->weight = weight;
    nextElement_in->next = nodes[in_id]->in_neighbour;
    nextElement_in->prev = NULL;
    nodes[in_id]->in_degree++;

    if (nodes[out_id]->out_neighbour != NULL) {
        nodes[out_id]->out_neighbour->prev = nextElement_out;
    }
    if (nodes[in_id]->in_neighbour != NULL) {
        nodes[in_id]->in_neighbour->prev = nextElement_in;
    }

    nodes[out_id]->out_neighbour = nextElement_out;
    nodes[in_id]->in_neighbour = nextElement_in;
}

const char* getfield(char* line, int num){
    const char* tok;
    for (tok = strtok(line, "\t");
            tok && *tok;
            tok = strtok(NULL, "\t\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

/*
char **strsplit(const char* str, const char* delim, size_t* numtokens) {
    char *s = strdup(str);
    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));
    char *token, *rest = s;
    while ((token = strsep(&rest, delim)) != NULL) {
        if (tokens_used == tokens_alloc) {
            tokens_alloc *= 2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    *numtokens = tokens_used;
    free(s);
    return tokens;
    }
*/

static int cmpNeighbours_r(const void *a, const void *b, void *array2){
  int index_a = *(int *) a;
  int index_b = *(int *) b;
  int *array = array2;
  return array[index_a] < array[index_b] ? -1 : array[index_a] > array[index_b];  
}

void findRepetition(char* vicinato, char* pesi, node** nodes,int N,int id){
    char* token1;
    char* token2;
    int lenght = 0;
    int i,j;
    for(i=0;vicinato[i]!='\0';++i){
       if(','==vicinato[i])
           lenght++;
        }
    
    lenght++;

    //fprintf(stderr, "cose a caso %s - %s - len = %i\n",vicinato,pesi,lenght);


    int *array1 = malloc(lenght*sizeof(int));
    int *array2 = malloc(lenght*sizeof(int));
    int *array_indici = malloc(lenght*sizeof(int));
    i = 0;
    while (((token1 = strsep(&vicinato, ",")) != NULL) && ((token2 = strsep(&pesi, ",")) != NULL)){
        //fprintf(stderr, "%s %s \n",token1,token2);
        array1[i] = atoi(token1);
        array2[i] = atoi(token2);
        array_indici[i] = i;
        //fprintf(stderr, "%i %i \n",array1[i],array2[i]);
        i++;
    }

    qsort_r(&array_indici[0],lenght,sizeof(int),cmpNeighbours_r,array1);
    
    /*
    for (i=0; i<lenght;i++)
        fprintf(stderr, " %i", array1[i]);
    fprintf(stderr, "\n");
    for (i=0; i<lenght;i++)
        fprintf(stderr, " %i", array2[i]);
    fprintf(stderr, "\n");
    for (i=0; i<lenght;i++)
        fprintf(stderr, " %i", array_indici[i]);
    fprintf(stderr, "\n");
    */
    
    
    int def, value;
    if (nodes[id]->player == 0)
        def = INT_MIN;
     else 
        def = INT_MAX;

    value = array2[array_indici[0]];
    for (i=0; i<lenght; i++){
        if (i != lenght-1 && array1[array_indici[i]] == array1[array_indici[i+1]]){
            if (nodes[id]->player == 0){
                if (value < array2[array_indici[i+1]])
                    value = array2[array_indici[i+1]];
            } else {
                if (value > array2[array_indici[i+1]])
                    value = array2[array_indici[i+1]];
            }
        } else {
            //fprintf(stderr, "prima %i %i \n",array1[array_indici[i]], value);
            if (nodes[id]->player == 0){
                if (value < array2[array_indici[i]])
                    value = array2[array_indici[i]];
            } else {
                if (value > array2[array_indici[i]])
                    value = array2[array_indici[i]];
            }
            //fprintf(stderr, "dopo %i %i \n",array1[array_indici[i]], value);
            addEdge(nodes,nodes[id]->id,array1[array_indici[i]],value);
            //addEdge(nodes,nodes[id]->id,0,value);
            value = def;
        }
    }

    free(array1);
    free(array2);
    free(array_indici);
}
    



graph* getGraphNew(char* path){
    FILE* stream = fopen(path, "r");

    char line[1024];
    int N,lineCounter,max_priority,boolean;
    max_priority=-1;
    N=0;
    boolean = 1;
    node** nodes;
    char** out_neighbours;
    char** weights;
    while (fgets(line, 1024, stream)){
        char* tmp = strdup(line);
        char* tok;
        if (boolean){
            boolean = 0;
            while ((tok = strsep(&tmp, " ")) != NULL) {
                N=atoi(strsep(&tok, ";\n"));
                //fprintf(stderr, "%i \n",N);
                if (N!=0){
                    N++;
                    nodes = (node**) malloc(N*sizeof(node*));
                    out_neighbours = (char**) malloc(N*sizeof(char*));
                    weights =(char**) malloc(N*sizeof(char*));
                    int j;
                    for (j=0; j<N;j++){
                        //fprintf(stderr, "%i \n",j);
                        nodes[j] = malloc(sizeof(node));
                        nodes[j]->id = j;
                    }
                    break;
                }
            }
        } else {
            int counter = 1;
            int id, priority,player;
            //char vicinato[MAX_LENGTH_STRING_VICINATO];
            char* label;
            char* vicinato_out;
            char* weight;


            while ((tok = strsep(&tmp, "\t")) != NULL)  {
                switch (counter){
                    case 1:
                        id = atoi(tok);
                        break;
                    case 2:
                        priority = atoi(tok);
                        break;
                    case 3:
                        player = atoi(tok);
                        break;
                    case 4:
                        //fprintf(stderr, "%s\n", tok);
                        vicinato_out = malloc(sizeof(char)*strlen(tok));
                        //vicinato_out = malloc(1000*sizeof(char));
                        strcpy(vicinato_out,tok);
                        out_neighbours[id] = vicinato_out;
                        break;
                    case 5:
                        label = malloc(sizeof(char)*strlen(tok));
                        strcpy(label,strsep(&tok, ";\n"));
                        break;
                    case 6:
                        weight = malloc(sizeof(char)*strlen(tok));
                        strcpy(weight,strsep(&tok, ";\n"));
                        weights[id] = weight;
                        break;
                    }
                counter++;
            }
            //fprintf(stderr, "%i %s %s -- %s \n",id,label,vicinato_out,weight);
            nodes[id]->player=player;
            nodes[id]->priority=priority;
            nodes[id]->label=label;

            findRepetition(vicinato_out,weight,nodes,N,id);

            if (nodes[id]->priority > max_priority)
                max_priority = nodes[id]->priority;

            lineCounter++;
            free(tmp);
        }
    }

    //fprintf(stderr, "%i \n",max_priority);

    graph* g1 = malloc(sizeof(graph));
    g1->nodes = nodes;
    g1->N = N;
    g1->max_priority = max_priority;
    
    return g1;
}


void printGraph(graph* g1){
    edgeList* it;
    int i = 0;
    int archi_in = 0;
    int archi_out = 0;
    for (i=0;i<g1->N;i++){
        it = g1->nodes[i]->out_neighbour;
        fprintf(stderr,"id = %i player = %i out = [",g1->nodes[i]->id,g1->nodes[i]->player);
        while(it!=NULL){
            archi_out++;
            fprintf(stderr,"(%i,",it->id);
            fprintf(stderr,"%i)",it->weight);
            it = it-> next;
        }
        it = g1->nodes[i]->in_neighbour;
        fprintf(stderr,"] in = [");
        while(it!=NULL){
            archi_in++;
            fprintf(stderr,"(%i,",it->id);
            fprintf(stderr,"%i)",it->weight);
            it = it-> next;
        }
        fprintf(stderr,"] label = %s out_degree = %i \n",g1->nodes[i]->label,g1->nodes[i]->out_degree);
    }

    fprintf(stderr, "Il grafo ha %i archi out e %i archi in\n",archi_out,archi_in);
}



int main(int argc, char **argv){

    graph* g1 = getGraphNew(argv[1]);
    
    printGraph(g1);
    
    small_Energy_Progress_Measure(g1->nodes,g1->N);
    //small_Progress_Measure(g1->nodes,g1->N,g1->max_priority);

    return 0;
}

