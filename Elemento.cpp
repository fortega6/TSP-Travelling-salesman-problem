#include <iostream>
#include "Elemento.h"

using namespace std;

Elemento::Elemento()
{
	ori = 0;
    des = 0;
    LB = 0;
    /*this->costos = new int *[6];
    for(int i = 0; i < 6; i++){
      this->costos[i] = new int[6];
    }*/
    
    //cout << "Se creo el Elemento " << 6 << endl;
}

/*Elemento::Elemento(int n)
{
	ori = 0;
    des = 0;
    LB = 0;
    //cout << "Se creo el Elemento " << 6 << endl;
    this->costos = new int *[n];
    for(int i = 0; i < n; i++){
      this->costos[i] = new int[n];
      //cout << "Se creo el Elemento " << n << endl;
    }
}*/

void Elemento::setOrigen(int ori)
{
	this->ori = ori;
}

void Elemento::setDestino(int des)
{
	this->des = des;
}


void Elemento::setCosto(int cost)
{
	this->LB = cost;
}

void Elemento::setElemento(int ori, int des, int cost)
{
	setOrigen(ori);
	setDestino(des);
	setCosto(cost);
}

void Elemento::setMatrizCostos(int **m, int n)
{
    reservar(n);
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            this->costos[i][j] = m[i][j];
}

void Elemento::addIndice(Indices x)
{
    v.push_back(x);
}


int Elemento::getOrigen()
{
	return(ori);
}

int Elemento::getDestino()
{
	return(des);
}

int Elemento::getCosto()
{
	return(LB);
}

vector<Indices> Elemento::getIndices()
{
	return(v);
}

void Elemento::imprimir(int n)
{
    for(int i=0;i<n;i++)
    {
    	printf("%3.d| ", i+1);
        for(int j=0;j<n;j++)
            printf("%6.d ", costos[i][j]);
        cout << endl;
    }
    cout << endl;
}

void Elemento::reservar(int n)
{
    this->costos = new int *[n];
    for(int i = 0; i < n; i++){
      this->costos[i] = new int[n];
      cout << "Se creo el Elemento " << n << endl;
    }
    
}

Elemento::~Elemento()
{
    delete[] costos;
	// Vacia
}
