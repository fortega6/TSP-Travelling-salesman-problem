#include <iostream>
#include "Pila.h"

using namespace std;

Pila::Pila()
{
	this->n = 0;	
}

void Pila::apilar(Elemento e)
{
    this->p[n].setElemento(e.getOrigen(), e.getDestino(), e.getCosto());
    this->n += 1;
}

void Pila::desapilar()
{
    this->n -= 1;
}

bool Pila::esVacia()
{
    return(this->n == 0);
}

Elemento Pila::tope()
{
	return(this->p[n-1]);
}
