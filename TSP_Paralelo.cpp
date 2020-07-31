/*****************************/
/*****Francisco Ortega*******/
/***************************/
// cc -g -o RUN .c -lpthread -lrt

#define INF 999999999
#define M 50
#define N 50
#define NUM_THREAD 15
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <queue>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <pthread.h>

using namespace std;

typedef int Matriz[M][M];
typedef int Vector[N];

typedef struct{
    int origen;
    int destino;
}Indices;

typedef struct{
	int id;
    Indices OD;
    int LB;
    Matriz costos;
    Indices v[N];
    Vector ii;
    Vector jj;
    int n;
    int nivel;
    int kk;
    int t;
}Elemento;

void lectura();
void imprimir(Matriz);
void TSP();
void copia(Matriz, Matriz);
int rowreduction(Matriz);
int rowmin(Matriz, int);
int columnreduction(Matriz);
int columnmin(Matriz, int);
void deleteRow(Matriz, int);
void deleteCol(Matriz, int);
void hijoIzquierdo(Matriz cost, Elemento *e);
bool onlyTour(Indices camino[N]);
bool esInf(Matriz cost);
void mostrarCamino(Indices c[N], Matriz cost);
void camino_inicial();

void  * bajada(void * e);

class CompareElemento {
public:
    bool operator()(Elemento& e1, Elemento& e2)
    {
       	if(e1.LB == e2.LB)
			return e1.nivel < e2.nivel;
		else
			return e1.LB > e2.LB;
    }
};
                       
Matriz costos;
                       
int n, t, hay_nodos = 1, id = 0;
int minCostOb = INF;
Vector pencost;
pthread_mutex_t minCostOb_mutex, cola_mutex, imprimir_mutex, mutex_cv, mutex_terminado;
pthread_cond_t cola_cv = PTHREAD_COND_INITIALIZER;;
bool hayCostMin = false, condiction = false;
bool terminado = false;

priority_queue<Elemento, vector<Elemento>, CompareElemento> colaNodosIz; // Cola de hijos izquierdos

pthread_t threads[NUM_THREAD];

int main()
{
	lectura();
	TSP();
}

void TSP()
{   
	Elemento e1;
	int k, minCostOb = INF;
	Indices auxOD;
	bool bandera;
	pthread_attr_t attr;
    
    /** Reduccion por filas de la matriz de costos **/
    /** Reduccion por columna de la matriz de costos **/
      
	pthread_mutex_init(&minCostOb_mutex, NULL);
	pthread_mutex_init(&cola_mutex, NULL);
	pthread_mutex_init(&imprimir_mutex, NULL);
	pthread_mutex_init(&mutex_cv, NULL);
	pthread_mutex_init(&mutex_terminado, NULL);
    
    
	/* Create threads to perform the dotproduct  */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for(int i = 0; i < NUM_THREAD; i++)
		pthread_create(&threads[i], &attr, bajada, static_cast<void*>(&e1));
	
	for(int i = 0; i < NUM_THREAD; i++)
		pthread_join(threads[i], NULL);
			
	pthread_mutex_destroy(&minCostOb_mutex);
	pthread_mutex_destroy(&cola_mutex);
	pthread_mutex_destroy(&mutex_terminado);
	pthread_mutex_destroy(&mutex_cv);
	pthread_cond_destroy(&cola_cv);
	pthread_exit(NULL);
}

void lectura()
{
	int x;
	char nombre1[30], nombre2[50], nombre3[30], nombre4[30], nombre5[30];
	Vector x1, y1;

    cin.getline(nombre1,30);
	cin.getline(nombre2,50);
	cin.getline(nombre3,30);
	scanf("DIMENSION : %d\n",&n); 
	cin.getline(nombre4,30);
	cin.getline(nombre5,30);
	
	cout << nombre1 << endl;
	cout << nombre2 << endl;
	cout << nombre3 << endl;
	cout << "DIMENSION : " << n << endl;
	cout << nombre4 << endl;
	cout << nombre5 << endl;
	
    for(int i = 0; i < n; i++)
    {
        cin >> x >> x1[i] >> y1[i];
    }
    
    for(int i = 0; i < n; i++)
    {   
    	costos[i][i] = INF;
        for(int j = i+1; j < n; j++)
        {
        	costos[i][j] = pow(x1[i]-x1[j],2) + pow(y1[i]-y1[j],2);
            costos[j][i] = pow(x1[i]-x1[j],2) + pow(y1[i]-y1[j],2);
        }
    }
}

void  * bajada(void * e)
{
	int k, aux;
	Elemento e1 = *(static_cast<Elemento*>(e));
	bool bandera;
	Indices auxOD;
	
	Matriz copy;
	
    copia(copy, costos);		//Copia de la matriz de costos

	e1.t = rowreduction(copy) + columnreduction(copy);;	// Minima cota
    
    copia(e1.costos, copy);	// Copia de la matriz reducida
    
    e1.kk = e1.nivel = 0;
    e1.n = e1.LB = 0;
	
	pthread_mutex_lock (&imprimir_mutex);
	e1.id = id;
	id++;
	//cout << "Thread #" << e1.id << " Creado" << endl;
	pthread_mutex_unlock (&imprimir_mutex);

	pthread_mutex_lock(&mutex_terminado);
	while(!terminado)
	{
			pthread_mutex_unlock(&mutex_terminado);
		 	if(!pthread_mutex_lock(&mutex_cv)) /*cout << "Lock mutex_cv " << e1.id << endl*/;
				while (hay_nodos == 0 and !terminado) {
					/*pthread_mutex_lock (&imprimir_mutex);
					cout << "Thread" << e1.id << "--> " << "Wait" << endl;
					pthread_mutex_unlock (&imprimir_mutex);*/
					pthread_cond_wait(&cola_cv, &mutex_cv);
					/*pthread_mutex_lock (&imprimir_mutex);
					cout << "Thread" << e1.id << "--> " << "Get up" << endl;
					pthread_mutex_unlock (&imprimir_mutex);*/
				}
				
				if(hay_nodos != 0)
				{
					hay_nodos--;
				}					
  			if(!pthread_mutex_unlock(&mutex_cv)) /*cout << "Unlock mutex_cv " << e1.id << endl*/;
			
			pthread_mutex_lock(&cola_mutex);

			aux = e1.id;
			if(!colaNodosIz.empty()) // Si no es vacia la pila de nodos izquierdos
			{
				e1 = colaNodosIz.top(); // Extrae nodo de la cola
				e1.id = aux;
				colaNodosIz.pop();

				pthread_mutex_unlock(&cola_mutex);
				
				e1.costos[e1.OD.origen][e1.OD.destino] = INF;
				rowreduction(e1.costos);	// Reduccion por filas de la matriz de costos
				columnreduction(e1.costos); // Reduccion por columnas de la matriz de costos
				e1.t = e1.LB;
			}
			else !pthread_mutex_unlock(&cola_mutex);
			
			pthread_mutex_lock (&minCostOb_mutex);
			if(e1.LB < minCostOb)
			{
				pthread_mutex_unlock (&minCostOb_mutex);
					
				k = e1.nivel;
				bandera = true;
				e1.LB = 0;
				hijoIzquierdo(e1.costos, &e1); // Buscar nodo Izquierdo
				e1.LB += e1.t;
					
				while( (k < n) and bandera)
				{		
					if(e1.LB > INF)
						e1.LB = INF;
					e1.nivel = k;
						
					pthread_mutex_lock (&minCostOb_mutex);
					if(e1.LB < minCostOb)
					{
						pthread_mutex_unlock (&minCostOb_mutex);
						pthread_mutex_lock (&cola_mutex);
						colaNodosIz.push(e1);	// Se encola el hijo izquierdo
						pthread_mutex_unlock (&cola_mutex);
						
						pthread_mutex_lock(&mutex_cv);
						hay_nodos++;
						pthread_mutex_unlock(&mutex_cv);
						
					}
					else
					pthread_mutex_unlock (&minCostOb_mutex);
				 
					deleteRow(e1.costos, e1.OD.origen);	// Eliminar fila, nodo origen
					deleteCol(e1.costos, e1.OD.destino); // Eliminar columna, nodo destino
						
					/******************************************************************/
					/*************Codiciones para eleminar circuitos*******************/
					/******************************************************************/
					if( (e1.costos[e1.OD.destino][e1.OD.origen] != INF) or (k == 0))
					{
						e1.costos[e1.OD.destino][e1.OD.origen] = INF;
						e1.ii[e1.kk] = e1.OD.origen;
						e1.jj[e1.kk] = e1.OD.destino;
						e1.kk+=1;
					}
					else
					{
						int i = 0, j = 0;
						bool encontrado = false;
						while(i < e1.kk and !encontrado)
						{
							if(e1.jj[i] == e1.OD.origen)
							{
								e1.costos[e1.OD.destino][e1.ii[i]] = INF;
								e1.jj[i] = e1.OD.destino;
								encontrado = true;
							}
							else if(e1.OD.destino == e1.ii[i])
							{
								e1.costos[e1.jj[i]][e1.OD.origen] = INF;
								e1.ii[i] = e1.OD.origen;
								encontrado = true;
							}
							i++;
						}	
						
						i -= 1;
						encontrado = false;
						
						while(j < e1.kk and !encontrado)
						{
							if(e1.jj[j] == e1.ii[i])
							{
								e1.costos[e1.jj[i]][e1.ii[j]] = INF;
								e1.ii[i] = e1.ii[j];
								encontrado = true;
								e1.kk-=1;
							}
							else if(e1.jj[i] == e1.ii[j])
							{
								e1.costos[e1.jj[j]][e1.ii[i]] = INF;
								e1.jj[i] = e1.jj[j];
								encontrado = true;
							}
							j++;	
						}
						
						if(k == n-2)
						{
							if(esInf(e1.costos))
							{
								auxOD.origen = e1.jj[i];
								auxOD.destino = e1.ii[i];
								e1.v[e1.n] = auxOD;
								e1.n += 1;
								k = n;
							}	
						}
					}   	
					/******************************************************************/
					/******************************************************************/
					/******************************************************************/
						
					e1.t += rowreduction(e1.costos) + columnreduction(e1.costos); // Costo hijo derecho
					
					if(e1.LB > INF)
						e1.LB = INF;
					
						
					pthread_mutex_lock (&minCostOb_mutex);
					if(e1.t >= minCostOb) // Si LB >= costo minimo obtimo
					{
						pthread_mutex_unlock (&minCostOb_mutex);
						bandera = false;	// se hace la poda del lado derecho
					}    
					pthread_mutex_unlock (&minCostOb_mutex);
						
					auxOD =  e1.OD;
					hijoIzquierdo(e1.costos, &e1); // Buscar nuevo nodo Izquierdo
					e1.LB += e1.t;
					e1.v[e1.n] = auxOD; // Se guardan los indices del nodo origen y destino
					e1.n += 1;
					k++;
				}

				/*pthread_mutex_lock (&imprimir_mutex);
				cout << endl;
				cout << "Thread --> " << e1.id << endl;
				cout << "LB --> " << e1.t << endl;
				pthread_mutex_unlock (&imprimir_mutex);*/

				if(bandera) // Si se encontro un camino
				{
					pthread_mutex_lock (&minCostOb_mutex);
					if(e1.t < minCostOb and onlyTour(e1.v)) // Se verifica que si su costo es menor al actual y que sea un solo tour
					{
						pthread_mutex_unlock (&minCostOb_mutex);
						
						pthread_mutex_lock (&imprimir_mutex);
						cout << endl;
						cout << "Thread --> " << e1.id << endl;
						cout << "Recorrido: ";
						mostrarCamino(e1.v, costos);
						pthread_mutex_unlock (&imprimir_mutex);
							
						pthread_mutex_lock (&minCostOb_mutex);
						minCostOb =e1.t;
						pthread_mutex_unlock (&minCostOb_mutex);
					}
					else pthread_mutex_unlock (&minCostOb_mutex);	
				}
								
			}
			else pthread_mutex_unlock (&minCostOb_mutex);
			
			pthread_mutex_lock (&mutex_terminado);
			terminado = colaNodosIz.empty();
			pthread_mutex_unlock (&mutex_terminado);
			
			pthread_mutex_lock(&mutex_cv);
			pthread_cond_broadcast (&cola_cv);
			pthread_mutex_unlock(&mutex_cv);
	}
	pthread_mutex_unlock(&mutex_terminado);
	
	pthread_exit(0);
}	

void imprimir(Matriz c)
{
    for(int i=0;i<n;i++)
    {
    	printf("%3.d| ", i+1);
        for(int j=0;j<n;j++)
            printf("%6.d ", c[i][j]);
        cout << endl;
    }
    cout << endl;
}


void copia(Matriz C, Matriz O)
{
	for(int i=0;i<n;i++)
    	for(int j=0;j<n; j++)
            C[i][j] = O[i][j];
}

int rowreduction(Matriz costrow)
{
    int rmin;
    int row=0;
    for(int i=0;i<n;i++)
    {
        rmin=rowmin(costrow,i);
        if(rmin!=INF)
            row+=rmin;

        for(int j=0;j<n;j++)
        {
            if(costrow[i][j]!=INF)
                costrow[i][j]=costrow[i][j]-rmin;
        }
    }
    return(row);
}

int rowmin(Matriz costmin, int i)
{
    int min;
    min=costmin[i][0];
    for(int j=0;j<n;j++)
        if(costmin[i][j]<min)
            min=costmin[i][j];
    return min;
}

int columnreduction(Matriz costred)
{
    int cmin;
    int col=0;
    for(int j=0;j<n;j++)
    {
      cmin=columnmin(costred,j);
      if(cmin!=INF)
      col+=cmin;
      for(int i=0;i<n;i++)
       {
         if(costred[i][j]!=INF)
            costred[i][j]=costred[i][j]-cmin;
       }
     }
     return(col);
}

int columnmin(Matriz costcol, int j)
{
    int min;
    min=costcol[0][j];
    for(int i=0;i<n;i++)
    {
        if(costcol[i][j]<min)
            min=costcol[i][j];
    }
    return min;
}

void deleteRow(Matriz A, int row)
{
	for(int j = 0; j < n; j++)
		A[row][j] = INF;
}

void deleteCol(Matriz A, int col)
{
	for(int i = 0; i < n; i++)
		A[i][col] = INF;
}

void hijoIzquierdo(Matriz cost, Elemento *e)
{
    int max1, max2 = (-1)*INF, aux;

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(cost[i][j] == 0)
            {
                aux = cost[i][j];
                cost[i][j] = INF;
                max1 = rowmin(cost, i) + columnmin(cost, j);
                if(max1 > max2)
                {
                    e->OD.origen = i;
                    e->OD.destino = j;
                    e->LB = max1;
				    max2 = max1;
                }
                cost[i][j] = aux;
            }

        }
    }
}

bool onlyTour(Indices c[N])
{
	int nodoInicial = c[0].origen, encontrado, nodoFinal = c[0].origen, encontrado2 = false;
	Vector cc;

	for(int i = 0; i < n; i++)
	{
		cc[i] = nodoFinal;
		encontrado = false;
		for(int j = 0; j < n and !encontrado; j++)
		{
			if(c[j].origen == nodoFinal)
			{
				nodoFinal = c[j].destino;
				encontrado = true;
			}	
		}
	}

	if(nodoInicial == nodoFinal)
	{
		encontrado2 = false;
		for(int i = 1; i < n and !encontrado2; i++)
		{
			if(nodoInicial == cc[i])
			{
				encontrado2 = true;
			}			
		}
	}
	return(nodoInicial == nodoFinal and !encontrado2);
}

void mostrarCamino(Indices c[N], Matriz cost)
{
	int nodo = c[0].origen, encontrado, costo = 0;


	cout << "< " << nodo+1 << " ";
	for(int i = 0; i < n; i++)
	{
		encontrado = false;
		for(int j = 0; j < n and !encontrado; j++)
		{
			if(c[j].origen == nodo)
			{
				cout << c[j].destino+1 << " ";
				costo += cost[nodo][c[j].destino];
				nodo = c[j].destino;
				encontrado = true;
			}	
		}
	}
	cout << ">" << endl; 
	cout << "Distancia Total: " << costo << endl;
}


bool esInf(Matriz cost)
{
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			if(cost[i][j] != INF)
				return(false);
	return(true);
}
