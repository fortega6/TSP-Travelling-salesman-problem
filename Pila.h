#include "Elemento.h"

#ifndef PILA_H
#define PILA_H

#define N 1000

class Pila {
	private:
		int n;
		Elemento p[N];
    public:
    	Pila();
    	void apilar(Elemento);
    	void desapilar();
    	Elemento tope();
    	bool esVacia();
};
#endif
