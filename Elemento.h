#include <vector>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#define M  500

using namespace std;

#ifndef ELEMENTO_H
#define ELEMENTO_H

class Indices
{
public:
    int ori;
    int des;
};

typedef int Matriz[M][M];

class Elemento 
{   
	int ori;
   	int des;
   	int LB;
   	Indices indi;
   	vector<Indices> v;
   	int **costos;
public:
    Elemento();
   	void setOrigen(int ori);
  	void setDestino(int des);
   	void setCosto(int cost);
   	void setElemento(int, int, int);
    void setMatrizCostos(int **m, int);
   	void addIndice(Indices);
   	
   	int getOrigen();
   	int getDestino();
   	int getCosto();
   	void imprimir(int);
   	vector<Indices> getIndices();
	~Elemento();
    void reservar(int);
}; // PERSONA_H

#endif
