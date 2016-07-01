/* 
 * File:   spm.c
 * Author: matteo
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "header.h"

typedef struct list {
    int id;
    int weight;
    struct list *next;
    struct list *prev;
} list;


static int* makeTop(node **nodi, int max_priority, int N) {
    int* M_G_top = (int*) calloc(max_priority + 1, sizeof (int));
    int i;
    for (i = 0; i < N; i++) {
        if ((nodi[i]->priority % 2) != 0)
            M_G_top[(nodi[i]->priority)]++;
    }

    for (i = 0; i < max_priority + 1; i++)
        M_G_top[i] += 2;
    return M_G_top;
}

int* Prog(int v_priority, int *misure_v, int *misure_w, int *M_G_top, int max_priority) { // O(d)	//funzione che restituisce la misura progressiva del nodo v dato il vicino v 	
    int *new_measure = (int*) malloc(max_priority* sizeof (int));
    int i;
    for (i = 0; i < max_priority + 1; i++) {	//copio la misura corrente di v
        new_measure[i] = misure_v[i];
    }

    if (misure_w[0] == -1){	//se la misura di w è il T anche la nuova misura di v sarà T
    	new_measure[0] = -1;
    	return new_measure;
    }

    for (i = 0; i < v_priority + 1; i++) {	//altrimenti calcolo la nuova misura
	    	if (new_measure[i] > misure_w[i]){	//se la misura corrente di v è maggiore di w ho finito
	    			break;
	    	}
	    	if (new_measure[i] < misure_w[i]){	//se la misura corrente di v è minore di w aggiorno v con il valore di w
	    		new_measure[i] = misure_w[i];
	    	}
	    	if (new_measure[i] == misure_w[i]){ //se hanno lo stesso valore 
	    		if (i == v_priority && (v_priority % 2) != 0 ){ //e sono nella posione della array = alla priorità e il nodo v è dispari icremento 
	    			new_measure[i]++; 							//incremento la posizione i della nuova misura per v
	    			if (new_measure[i] > M_G_top[i]){			//se incrementando supero il bound ritorno il T
   	            		new_measure[0] = -1;
            		} 
	    			break;
	    		}
	    	}
    	
        }
  
    return new_measure;
}

typedef struct listaSalvi {		//lista bidirezionale utilizzata da maxMisura e minMisura per mantenere la lista degli indici delle misure candidate
    int id;
    struct listaSalvi *next;
    struct listaSalvi *prev;
} listaSalvi;

listaSalvi** aggiungiNodoMisure(listaSalvi** head, int id) {
    listaSalvi* new_element = (listaSalvi*) malloc(sizeof (listaSalvi));
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

listaSalvi* eliminaNodoMisure(listaSalvi* element, listaSalvi** head) {
    listaSalvi* temp;
    if (element -> prev == NULL) { // sono la testa
    	//printf("Sono la testa \n");
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


int* maxMisura(int **misure, int n_measure, int max_priority,int N) { // O(d*out_v) -> O(d*(v-1)) worst case
    int i, j, k;
    listaSalvi** misure_valide = (listaSalvi**) malloc(sizeof(listaSalvi*)*N); 
    int max_value = -100;

    for (j = 0; j < n_measure; j++) { // O(out_v)	//Se una delle misure è il T mi fermo e restituisco T
        if (misure[j][0] == -1) {
        	for (i = 0; i < n_measure; i++){
        		if (i != j)
        			free(misure[i]);
        	}
            return misure[j];
        } else if (misure[j][0] > max_value) {		//altrimenti trovo il massimo valore nella posizione 0
            max_value = misure[j][0];
        }
    }


    for (j = 0; j < n_measure; j++) { // O(out_v)	//selezioni le misure candidate come massimo selezionando l'indice di tutte le misure che hanno
    												//il massimo nella posione 0
        if (misure[j][0] == max_value) 
        	misure_valide = (listaSalvi**) aggiungiNodoMisure(misure_valide,j);
    }

    int eliminato = 0;
    for (i = 1; i < max_priority; i++) {			//seleziono la misura massima
        listaSalvi *iteratore;
        max_value = -100;

        for (k = 0; k < 2; k++) {
            iteratore = *misure_valide;
            while (iteratore != NULL) {
                if (k == 0) {
                    if (misure[iteratore->id][i] >= max_value) {
                        max_value = misure[iteratore->id][i];
                    }
                } else {
                    if (misure[iteratore->id][i] != max_value) {
                        iteratore = eliminaNodoMisure(iteratore, misure_valide);
                        eliminato = 1;
                    }
                }
                //printf("%i ",iteratore->id);
                if (iteratore == (*misure_valide) && eliminato != 0)
                	eliminato = 0;
                else
                	iteratore = iteratore-> next;
                
            }
        }
    }

    for (i = 0; i < n_measure; i++){
        if (i != (*misure_valide)->id)
        	free(misure[i]);
        }
    return misure[(*misure_valide)->id];
}



int* minMisura(int **misure, int n_measure, int max_priority, int N) { // O(d*out_v) -> O(d*(v-1)) worst case
    int i, j, k;
    listaSalvi** misure_valide = (listaSalvi**) malloc(sizeof(listaSalvi*)*N); 
    int value = 100;
    int controlla_min = -10;


    for (j = 0; j < n_measure; j++) { 	//controllo che le misure candidate non siano tutti T
    	if (misure[j][0] > controlla_min)
            controlla_min = misure[j][0];
    }

    if (controlla_min == -1){			//se lo sono ritorno la prima
    	for (i = 0; i < n_measure; i++){
        	if (i != 0)
        		free(misure[i]);
        }
    	return misure[0];
    }

    
    for (j = 0; j < n_measure; j++) { // O(out_v)  	//altrimenti seleziono le misure che hanno nella posizione 0 il valore minimo
        if (misure[j][0] < value) {
            value = misure[j][0];
        }
    }

    for (j = 0; j < n_measure; j++) { // O(out_v)	//quindi definisco la lista delle misure valide (cioè quelle con i valori a sinistra minimi e uguli tra loro)
        if (misure[j][0] == value) 
        	misure_valide = (listaSalvi**) aggiungiNodoMisure(misure_valide,j);
    }


    int eliminato = 0;
    for (i = 1; i < max_priority +1; i++) {		//estraggo la misura minima
        listaSalvi *iteratore;
        value = 100;
        //fprintf(stderr, "%i \n",i );
        for (k = 0; k < 2; k++) {
            iteratore = *misure_valide;
            while (iteratore != NULL) {
                if (k == 0) {
                    if (misure[iteratore->id][i] <= value) {
                        value = misure[iteratore->id][i];
                    }
                } else {
                    if (misure[iteratore->id][i] != value) {
                        iteratore = eliminaNodoMisure(iteratore, misure_valide);
                        eliminato = 1;
                    }
                }
                //printf("%i ",iteratore->id);
                if (iteratore == (*misure_valide) && eliminato != 0)
                	eliminato = 0;
                else
                	iteratore = iteratore-> next;
            }
            if ((*misure_valide) == NULL){
            	
            }
        }
    }
    for (i = 0; i < n_measure; i++){
        if (i != (*misure_valide)->id)
        	free(misure[i]);
        }

    return misure[(*misure_valide)->id];
}

static void Lift(node **nodo, int **misure, int v, int *M_G_top, int max_priority, int N) {
	edgeList* iteratore = nodo[v]->out_neighbour;
	int** nuove_misure = malloc(N*sizeof(int*));	//definisce il puntatore all'array di array di interi in cui verranno salvate le misure candidate per v
	int* misura_cadidata;
	int i = 0;
	int counterFix;
	nodo[v]->fixPoint = 0;

	printf("Eseguo Lift sul nodo %i del giocatore %i con priorità %i - la misura corrente del nodo è: ",v,nodo[v]->player, nodo[v]->priority);
	for (i=0; i<max_priority+1; i++)
		printf("%i",misure[v][i]);


	for (i=0;i<nodo[v]->out_degree;i++){ //per ogni vicino i di v viene calcolata la nuova misura di v rispetto ad i
		nuove_misure[i] = Prog(nodo[v]->priority, misure[v],misure[iteratore->id],M_G_top,max_priority);
       	iteratore = iteratore->next;
    }

    if (nodo[v]->player == 0) {
		//se il nodo è pari viene selezionata la misura_candidata minore
    	misura_cadidata = minMisura(nuove_misure, nodo[v]->out_degree, max_priority,N);
    } else {
    	//se il nodo è dispari viene selezionata la misura_candidata massima
       	misura_cadidata = maxMisura(nuove_misure, nodo[v]->out_degree, max_priority,N);
    }

    if (misura_cadidata[0] == -1){ 	//controllo se la misura scela è il T
    	misure[v][0] = -1;
    } else { 						//altrimenti controllo se è uguale a quella precedente (punto fisso)
    	counterFix = 0;
		for (i=0;i<max_priority+1;i++){
	    	if (misura_cadidata[i]>misure[v][i] ){
	    		free(misure[v]);
	    		misure[v] = misura_cadidata;
	    		break;
	    	} else if (misura_cadidata[i] == misure[v][i]) {
	    		counterFix++;
	    	}
   		}

   		if (counterFix == (max_priority+1)) {
   			nodo[v]-> fixPoint = 1;
   		}

   	}


	printf(" e verrà aggiornata con: ");
   	for (i=0; i<max_priority+1; i++)
		printf("%i",misure[v][i]);

	printf("\n");

}

void small_Progress_Measure(node** nodes, int N, int max_priority) {


	int** misure = (int**) calloc(N, sizeof(int *));
    int i;
    for (i = 0; i < N; i++) {
        misure[i] = (int*) calloc(max_priority + 1, sizeof (int));
    }

    int* M_G_top = makeTop(nodes, max_priority, N);

	printf("M_G_top = ");
    for (i = 0; i < max_priority + 1; i++) {
        printf("%i", M_G_top[i]);
    }

    printf("\n");
    printf("\n");



    int avanti = 1;
    int iteration_counter = 0;
    int flag = 0;

    while (avanti){
    	int top = 0;
    	int i = 0;
    	int fixed = 0;

    	for (i=0;i<N;i++){ //ciclo per esecuzione della funzione Lift su ogni nodo v
    		nodes[i]->fixPoint = 0;
    		if (misure[i][0] == -1) {
    			top++;
    		} else {
    			//printf("%i ",iteration_counter);
    			Lift(nodes, misure, i, M_G_top, max_priority,N);
    			if (nodes[i]->fixPoint==1){
    				fixed++;
    			}
    		}
    	}

    	if (fixed + top == N){
    		if (flag + 1 == iteration_counter)
    			avanti = 0;
    		else
    			flag = iteration_counter;
    	}
    	iteration_counter++;
	}

}