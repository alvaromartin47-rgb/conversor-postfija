/* *****************************************************************
 *                                                                 *
 *                           INFIX                                 *
 *                                                                 *
 *                                                                 *
 * *****************************************************************/

/*------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mod/pila/pila.h"
#include "mod/cola/cola.h"
#include "mod/strutil/strutil.h"

/*------------------------------------------------------------------*/

/* *****************************************************************
 *                      FUNCIONES DE PROGRAMA
 * *****************************************************************/

/*
Recibe un operador y determina por donde es asociativo. Devuelve 1 si es por
derecha o 0 si es por izquierda.
Pre: los operadores validos son {-,+,/,*,^}.
*/
int asoc(char *tok) {
    return strcmp(tok, "^") == 0 ? 1 : 0; 
}

/*
Recibe un string y devuelve true si es un operador o false si es un digito.
*/
bool es_operador(char *tok) {
    return tok && atoi(tok) == 0 ? true : false;
}

/*
Recibe una cola e imprime cada uno de sus elementos en el orden
que se encuentran y separados por un espacio.
Pre: la cola fue creada y contiene strings.
Post: se mostro por pantalla cada elemento y se hizo free del mismo.
*/
void imprimir_resultado(cola_t *cola) {
    while (!cola_esta_vacia(cola)) {
        char *str_desencolado = cola_desencolar(cola);
        printf("%s", str_desencolado);
        printf(" ");
    }
    printf("\n");
}

/*
Recibe un string y un entero, es decir un token (operador) y su prioridad.
Devulve true en dos casos:
-Si _tok_ es asociativo por izquierda y su prioridad es menor o igual a la de otro tok.
-Si _tok_ es asociativo por derecha y su prioridad es menor a la de otro tok.
De lo contrario, false.
Pre: la funcion tiene sentido si _tok_pri_ es resultado de la comparacion de prioridades. 
*/
bool asociativos(char *tok, int tok_pri) {
    // tok_pri es el resultado de la comparacion de prioridades con el tok perteneciente al
    // tope de la pila.
    if (asoc(tok) == 0 && (tok_pri <= 0)) return true;
    if (asoc(tok) == 1 && tok_pri == -1) return true;
    return false;
}

/*
Recibe un string y su largo, devuelve un arreglo de strings de largo + 1,
En cada posicion del nuevo arreglo habra un operando o un operador, la funcion
se encarga de interpretar cuales son las unidades en caso de tratarse de un operando
es decir, si en _linea_ hay algo del estilo:

                                    "55+5" 

en la primer posicion del nuevo arreglo habra un 55 y no un 5, pero si vienen separados
los tomará como un 5.
Tanto el (char **) como el/los (char *) del nuevo arreglo se alojan en el heap, en caso de
no ser posible devuelve NULL. La ultima posicion del arreglo el NULL siempre.
La liberacion de memoria queda a cargo del usuario de la función.
Post: se devolvió un arreglo dinamico donde hay operandos y operadores por separado. Si
no se pudo reservar memoria se devolvió NULL.
*/
char **parse(char *linea, size_t largo_linea) {
    char **tokens = calloc(sizeof(char*), largo_linea);
    if (!tokens) return NULL;

    size_t i_tokens = 0;
    for (size_t i=0; i < largo_linea - 1; i++) {
        char *str = calloc(sizeof(char), largo_linea);
        if (!str) return NULL;
        
        tokens[i_tokens] = str;
        size_t pos = 0;
        char aux[1] = {linea[i]};
        
        if (atoi(aux) != 0 || linea[i] == '0') {
            while (atoi(aux) != 0 || linea[i] == '0') {
                tokens[i_tokens][pos] = linea[i];
                i++;
                pos++;
                aux[0] = linea[i];
            }
            i_tokens++;
            i--;
            continue;
        }
        tokens[i_tokens][0] = linea[i];
        i_tokens++;
    }

    return tokens;
}

/*
Recibe un operador y devuelve un entero entre [1,5] indicado la precedecia
del mismo.
Pre: _tok_ debe ser un string y uno de los siguientes: {(,-,+,/,*,^,)}. 
*/
int precedencia(char *tok) {
    if (strcmp(tok, "(") == 0) return 1;
    if (strcmp(tok, "+") == 0 || strcmp(tok, "-") == 0) return 2;
    if (strcmp(tok, "*") == 0 || strcmp(tok, "/") == 0) return 3;
    if (strcmp(tok, "^") == 0) return 4;
    return 5;
}

/*
Esta funcion se encarga de cambiar el estado de los booleanos de acuerdo a si
son parentesis de apertura o cierre, el tope de la pila y el string token.
Esto se hace se acuerdo a las siguientes referencias:
a_a = paréntesis de apertura token actual (_tok_).
a_c = paréntesis de cierre token actual (_tok_).
t_a = paréntesis de apertura (tope de la pila).
t_c = paréntesis de cierre (tope de la pila).
El estado puede ser true o false, si es true significará que ese elemento es
un parentesis, dependera de su nombre de variable cual es. Si es falso no
será paréntesis.
Pre: la pila fue creada.
Post: si un parentesis es true su pareja será false, y viceversa. Excepto en
el caso donde no hay elementos en el tope de la pila, alli ambos seran false.
*/
void son_parentesis(pila_t *pila, char *tok, bool *a_c, bool *a_a, bool *t_c, bool *t_a) {
    if (strcmp(tok, "(") == 0) {
        *a_a = true;
        *a_c = false;
    }
    else if (strcmp(tok, ")") == 0) {
        *a_a = false;
        *a_c = true;
    } 

    char *tok_tope = pila_ver_tope(pila);
    
    if (tok_tope) {
        if (strcmp(tok_tope, "(") == 0) {
            *t_a = true;
            *t_c = false;
        }
        else if (strcmp(tok_tope, ")") == 0) {
            *t_a = false;
            *t_c = true;
        }
    }
    else {
        *t_a = false;
        *t_c = false;
    }
}

/*
Recibe una pila, un string y dos punteros a bool. 
Devuelve 1 si la precedencia de _tok_act_ es mayor a la del tope de la pila,
0 si son iguales, o -1 si es menor. Si la pila está vacia devolverá 1. 
Pre: la pila fue creada.
Post: se devolvio el entero correspondinete.
*/
int tok_prioridad(pila_t *pila, char *tok_act) {
    char *tok_tope = pila_ver_tope(pila);
    if (!tok_tope) return 1;

    int p_tok_act = precedencia(tok_act);
    int p_tok_tope = precedencia(tok_tope);

    if (p_tok_act > p_tok_tope) return 1;
    if (p_tok_act == p_tok_tope) return 0;
    return -1;
}

/*
Recibe una pila, una cola, un vector de strings (tokens), un string (linea) y su
largo. Convierte _linea_ que se encuentra en notacion infija a posfija, utilizando
una pila y una cola. Devuelve true si no hubo errores del sistema, de lo contrario
false.
Pre: la pila y la cola fueron creadas, estan vacias y el contenido de _linea_ en 
infija esta escrito correctamente.
Post: la pila debe quedar vacía. Si devolvió true cada elemento de la cola corresponde
a un operador u operando, el orden se encuentra de acuerdo a la notación posfija donde
en el frente de la cola esta el primer elemento de la conversion de izquierda a derecha. 
En caso de que haya devuelto false no se asegura nada. 
*/
bool infix(pila_t *pila, cola_t *cola, char *linea, char **tokens, size_t linea_largo) {
    bool ok = true;

    for (size_t i=0; tokens[i] && ok; i++) {
        // Si es un espacio
        if (strcmp(tokens[i], " ") == 0) continue;
        
        // Si es un digito
        if (!es_operador(tokens[i]) || strcmp(tokens[i], "0") == 0) {
            ok = cola_encolar(cola, tokens[i]);
            continue;
        }

        // Si tok actual es un operador
        bool act_es_cierre = false;
        bool act_es_aper = false;
        bool tope_es_cierre = false;
        bool tope_es_aper = false;
        // Prioridad de tok actual respecto del tope
        int tok_pri = tok_prioridad(pila, tokens[i]);
        son_parentesis(pila, tokens[i], &act_es_cierre, &act_es_aper, &tope_es_cierre, &tope_es_aper);

        // Si no es un operador de apertura o cierre
        if (!act_es_cierre && !act_es_aper) {
            while (es_operador(pila_ver_tope(pila)) && !tope_es_aper && asociativos(tokens[i], tok_pri)) {
                char *desapilado = pila_desapilar(pila);
                ok = cola_encolar(cola, desapilado);
                
                tok_pri = tok_prioridad(pila, tokens[i]);
                son_parentesis(pila, tokens[i], &act_es_cierre, &act_es_aper, &tope_es_cierre, &tope_es_aper);
            }
            ok = pila_apilar(pila, tokens[i]);
        }

        // Si es un operador y es el de apertura
        else if(act_es_aper) ok = pila_apilar(pila, tokens[i]);
        
        // Si es un operador y es el de cierre
        else {
            while (!tope_es_aper) {
                char *desapilado = pila_desapilar(pila);
                ok = cola_encolar(cola, desapilado);
                son_parentesis(pila, tokens[i], &act_es_cierre, &act_es_aper, &tope_es_cierre, &tope_es_aper);
            }
            pila_desapilar(pila);
        }

    }
    
    while (!pila_esta_vacia(pila)) {
        char *desapilado = pila_desapilar(pila);
        ok = cola_encolar(cola, desapilado);
    }
    
    return ok;
}

/*
Recibe una pila y una cola vacia, lee la entrada standard, por cada linea del
archivo stdin realiza una operación que puede devolver true o false,
si devolvió true la operación se realizo correctamete y muestra el resultado
por salida standard. En caso de false, ocurrió un error interno con la memoria
por lo tanto no hace nada y sigue leyendo.
Pre: la pila y la cola fueron creadas, estan vacías y la operacion que se lee por
stdin está correctamente escrita.
Post: se mostró algo por consola, la pila y la cola estan vacias.
*/
void procesar_entrada(pila_t *pila, cola_t *cola) {    
    size_t tam_buffer = 1;
    char *buff_linea = malloc(sizeof(char) * tam_buffer);

    while (getline(&buff_linea, &tam_buffer, stdin) != EOF) {
        size_t largo_linea = strlen(buff_linea);

        if (strcmp(&buff_linea[largo_linea - 1], "\n") == 0) buff_linea[largo_linea - 1] = '\0';
        else largo_linea++;
        
        char **tokens = parse(buff_linea, largo_linea);
        if (!tokens) continue;

        bool es_valido = infix(pila, cola, buff_linea, tokens, largo_linea);

        if (!es_valido) while (!cola_esta_vacia(cola)) cola_desencolar(cola);
        
        else imprimir_resultado(cola);

        free_strv(tokens);
    }
    
    free(buff_linea);
}

/*------------------------------------------------------------------*/

/* *****************************************************************
 *                              MAIN
 * *****************************************************************/

int main() {
    pila_t *pila = pila_crear();
    if (!pila) return 1;
    cola_t *cola = cola_crear();
    if (!cola) {
        pila_destruir(pila); 
        return 1;
    }
    
    procesar_entrada(pila, cola);
    
    pila_destruir(pila);
    cola_destruir(cola, NULL);

    return 0;
}

                        /*FIN DE PROGRAMA*/                         
/*------------------------------------------------------------------*/
