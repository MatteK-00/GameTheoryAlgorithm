#include <stdlib.h>
#include <stdio.h>
#include "header.h"


typedef struct lista_bidirezionale {     //lista bidirezionale utilizzata da maxMisura e minMisura per mantenere la lista degli indici delle misure candidate
    int id;
    struct lista_bidirezionale *next;
    struct lista_bidirezionale *prev;
} lista_bidirezionale;

lista_bidirezionale** aggiungiNodo(lista_bidirezionale** head, int id) {
    lista_bidirezionale* new_element = (lista_bidirezionale*) malloc(sizeof (lista_bidirezionale));
    if (*head == NULL) {
        *head = new_element;
        new_element -> id = id;
        new_element -> next = NULL;
        new_element -> prev = NULL;
    } else {
        new_element -> id = id;
        new_element -> prev = NULL;
        new_element -> next = *head;
        new_element -> next -> prev = new_element;
        *head = new_element;
    }
    return (head);
}

lista_bidirezionale* eliminaNodo(lista_bidirezionale* element, lista_bidirezionale** head) {
    lista_bidirezionale* temp;
    if (element -> prev == NULL) { // sono la testa
        if (element -> next != NULL)
            element -> next -> prev = NULL;
        *head = element -> next;
        temp = *head;
    } else if (element -> next == NULL) { // sono la coda
        //printf("Sono la coda \n");
        temp = element -> prev;
        element -> prev -> next = NULL;
    } else { // sono nel corpo
        //printf("Sono nel corpo \n");
        temp = element -> prev;
        element -> next -> prev = element -> prev;
        element -> prev -> next = element -> next;
    }
    free(element);
    return temp;
}


void printL(lista_bidirezionale* lista_L, node** nodi){
    fprintf(stderr, "La lista L contiene i nodi:");
    while (lista_L != NULL) {
        fprintf(stderr, " %s",nodi[lista_L->id]->label);
        lista_L = lista_L->next;
    }
    fprintf(stderr, "\n");
}

int inLista(int id, lista_bidirezionale* lista_L){
    while (lista_L != NULL) {
        fprintf(stderr, "puntatore %p\n",lista_L );
        fprintf(stderr, "puntatore %i\n", lista_L -> id );
        if (id == lista_L -> id)
            return 1;
        else
            lista_L = lista_L->next;
    }
    return 0;
}

static int makeTop(node** nodes, int N){
    int T = 0;
    int i;
    for (i=0; i<N; i++){
        edgeList* iteratore = nodes[i] -> out_neighbour;
        int max_w = 0;
        while (iteratore != NULL){
            int temp = iteratore->weight * (-1);
            if (temp > max_w)
                max_w = temp;
            iteratore = iteratore-> next;
        }
        T += max_w;
    }
    return T+1;
}

int confronta_misure(int a, int b, int T){
    int ris = a - b;
    //fprintf(stderr, "Sono ris %i \n",ris);
    if (a != T && ris < T){
        if (ris >= 0)
            return ris;
        else
            return 0;
    }
    return T;
}

int setCount_v(node** nodes, int v, int T, int* f){
    edgeList* iteratore = nodes[v]-> out_neighbour;
    int count = 0;
    while (iteratore != NULL){
        if (f[v] >= confronta_misure(f[iteratore->id],iteratore->weight,T)){
            count++;
        }
        iteratore = iteratore->next;
    }

    return count;
}

static int Lift(int* f, int v, node** nodes, int T){
    int temp;
    edgeList* iteratore = nodes[v]-> out_neighbour;
    int ris  = confronta_misure(f[iteratore->id],iteratore->weight,T);
    while (iteratore != NULL){
        temp = confronta_misure(f[iteratore->id],iteratore->weight,T);
        //fprintf(stderr, "player %i uso confronta misura su %i e %i e il risultato e' %i \n",nodes[v]->player, f[iteratore->id],iteratore->weight,temp);
        if(nodes[v]->player == 0){
            //fprintf(stderr, "Sono temp %i \n",v);
            if (temp < ris)
                ris = temp;
        } else{
            if (temp > ris)
                ris = temp;
        }
        iteratore = iteratore -> next;   
    }
    return ris;
}


void small_Energy_Progress_Measure(node** nodes, int N) {
   
    int i;
    lista_bidirezionale** lista_L = (lista_bidirezionale**) malloc(sizeof(lista_bidirezionale*)); 
    int* inlistaL = calloc(N,sizeof(int));


    for (i=0; i<N; i++){
        //printlist(nodes[i]->out_neighbour);
        edgeList* iteratore; // = malloc(sizeof(edgeList));
        iteratore = nodes[i]->out_neighbour;

        if (nodes[i]->player == 0){
            int counter = 0;

            while(iteratore != NULL){
                if (iteratore->weight < 0){
                    counter++;
                    iteratore = iteratore->next;
                } else {
                    break;
                }
            }
            if (counter == nodes[i]->out_degree){
                lista_L = aggiungiNodo(lista_L, nodes[i]->id);
                inlistaL[nodes[i]->id]=1;
            }
        }else {
            while(iteratore != NULL){
                if (iteratore->weight < 0){
                    lista_L = aggiungiNodo(lista_L, nodes[i]->id);
                    inlistaL[nodes[i]->id]=1;
                    break;
                }else{
                    iteratore = iteratore->next;
                }       
            } 

        }
        //printlist(nodes[i]->out_neighbour);
    }

    int* f = calloc(N,sizeof(int));
    int* count = malloc(N*sizeof(int));
    int T = makeTop(nodes,N);

    printf("Il top è %i \n", T);

    printL(*lista_L,nodes);

    for (i=0; i<N; i++){
        if (nodes[i]-> player == 0){
            if (inLista(nodes[i]->id, *lista_L)){
                count[i] = 0;
            } else {
                count[i] = setCount_v(nodes,nodes[i]->id,T,f);
            }
        }
    }

    printL(*lista_L,nodes);
    /*
    lista_bidirezionale* it = *lista_L; 
    while (it != NULL){
        fprintf(stderr, "  %i \n",it->id);
        it = it-> next;
    }
    */

    int old_f, v;
    lista_bidirezionale* it = *lista_L; 
    while (it != NULL){
        v = (*lista_L) -> id;
        old_f = f[v];
        it = eliminaNodo(*lista_L, lista_L);
        inlistaL[v]=0;
        f[v] = Lift(f,v,nodes,T);

        printf("f[%s] vale %i \n",nodes[v]->label,f[v]);
        if (nodes[v]->player == 0)
            count[v] = setCount_v(nodes,v,T,f);
        edgeList* v_1 = nodes[v]->in_neighbour;
        while (v_1 != NULL){
            //fprintf(stderr,"v_1 e' %s \n",nodes[v_1->id]->label);
            if (f[v_1->id] < confronta_misure(f[v],v_1->weight,T)){
                if (nodes[v_1->id]->player == 0) {
                    if (f[v_1->id] >= confronta_misure(old_f,v_1->weight,T))
                        count[v_1->id]-= 1;
                    if (count[v_1->id] <= 0){
                        //fprintf(stderr,"count[%s] <= 0 Aggiungo il nodo %s a L\n",nodes[v_1->id]->label,nodes[v_1->id]->label);
                        if (inlistaL[nodes[v_1->id]->id]==0){
                            lista_L = aggiungiNodo(lista_L, nodes[v_1->id]->id);
                            inlistaL[nodes[v_1->id]->id]=1;
                            it = *lista_L;
                        }
                    }
                } else{
                    if (inlistaL[nodes[v_1->id]->id]==0){
                        lista_L = aggiungiNodo(lista_L, nodes[v_1->id]->id);
                        inlistaL[nodes[v_1->id]->id]=1;
                        it = *lista_L;
                    }
                }
            }
            v_1 = v_1 -> next;
        }
        printL(*lista_L,nodes);
    }

    for (i=0; i<N; i++){
        if (f[i] == T)
            fprintf(stdout, "Il nodo %s ha misura = T  \n",nodes[i]->label);
        else
            fprintf(stdout, "Il nodo %s ha misura = %i  \n",nodes[i]->label , f[i]);
    }

}