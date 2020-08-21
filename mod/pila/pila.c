#include "pila.h"
#include <stdlib.h>

// Declaro las funciones auxiliares
bool pila_redimensionar(pila_t *pila, size_t tam_nuevo);

// Defino la cantidad inicial de elementos con la que se creará la pila.
#define CANT_INICIAL_ELEMENTOS 10
// Defino coeficiente de multiplicacion o division para agrandar o reducir la pila.
#define EXPANSION_PILA 2
// Defino coeficiente de division maximo para que la pila se reduzca.
#define MAX_DIVISION 4


/* Definición del struct pila proporcionado por la cátedra.*/

struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados
    size_t capacidad; // Capacidad del arreglo 'datos'
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t* pila_crear(void) {
    pila_t* pila = malloc(sizeof(pila_t));
    if (pila == NULL) return NULL;
    
    pila->datos = malloc(sizeof(void*) * CANT_INICIAL_ELEMENTOS);
    if (pila->datos == NULL) {
        free(pila);
        return NULL;
    }
    pila->cantidad = 0;
    pila->capacidad = CANT_INICIAL_ELEMENTOS;
    return pila;
}

void pila_destruir(pila_t *pila) {
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila) { 
    return pila->cantidad == 0 ? true : false;
}

bool pila_apilar(pila_t *pila, void* valor) {
    if (pila->cantidad == pila->capacidad) {
        size_t tam_nuevo = pila->capacidad * EXPANSION_PILA;
        if (!pila_redimensionar(pila, tam_nuevo)) {
            return false;
        }
    }
    pila->datos[pila->cantidad] = valor;
    pila->cantidad ++;
    return true;
}

void* pila_ver_tope(const pila_t *pila) {
    return pila_esta_vacia(pila) ? NULL : pila->datos[pila->cantidad-1];
}

void* pila_desapilar(pila_t *pila) {
    if (pila_esta_vacia(pila)) return NULL;
    
    if (pila->cantidad == pila->capacidad / MAX_DIVISION && pila->capacidad > CANT_INICIAL_ELEMENTOS) {
        size_t tam_nuevo = pila->capacidad / EXPANSION_PILA;
        pila_redimensionar(pila, tam_nuevo);
    }
    void* ultimo = pila->datos[pila->cantidad-1];
    pila->cantidad --;
    return ultimo;
}

/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/


/*
Recibe un puntero al struct pila_t y un tamaño de tipo size_t.

Redimensiona la pila, es decir la capacidad en elementos que ésta tiene, por ende el tamaño
en bytes que ocupa el arreglo de punteros sobre el que está creada la pila.

Si la redimension es exitosa actualiza los miembros datos y capacidad del struct pila_t
y devuelve true, si por algun motivo el sistema operativo no pudo reservar la nueva porcion de
memoria, devuelve false.
*/ 

bool pila_redimensionar(pila_t *pila, size_t tam_nuevo) {
    if (tam_nuevo > 0) {
    	void** datos_nuevo = realloc(pila->datos, tam_nuevo * sizeof(void*));
    	
    	if (datos_nuevo == NULL) return false;
    	
    	pila->datos = datos_nuevo;
    	pila->capacidad = tam_nuevo;
    	
        return true;
    }
    return false;
}
