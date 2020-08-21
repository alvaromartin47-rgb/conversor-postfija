#define _POSIX_C_SOURCE 200809L
#include "strutil.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* DECLARACION DE FUNCIONES */

void free_strv(char *strv[]);
char *join(char **strv, char sep);
char **split(const char *str, char sep);
char *substr(const char *str, size_t n);

/* *****************************************************************
 *                     FUNCIONES AUXILIARES
 * *****************************************************************/

void *liberar(char **strv) {
    free_strv(strv);
    return NULL;
}

size_t ag_letra(char **strv, const char *str, size_t i_pal, size_t i_let, size_t i) {
    strv[i_pal - 1][i_let] = str[i];
    return i_let + 1;
}

/* *****************************************************************
 *                FUNCIONES PEDIDAS POR EL CURSO
 * *****************************************************************/

void free_strv(char *strv[]) {
    for (size_t i=0; strv[i] != NULL; i++) free(strv[i]);
    free(strv);
}

char *substr(const char *str, size_t n) {
    return strndup(str, n);
}

char **split(const char *str, char sep) {
    size_t largo = 0;
    size_t c_pal = 0;
    for (size_t i=0; str[i] != '\0'; i++) {
        if (str[i] == sep) c_pal ++;
        largo ++;
    }
    
    char **sepa = malloc(sizeof(char*) * (c_pal + 2));
    if (!sepa) return NULL;
    sepa[c_pal + 1] = NULL;

    size_t i_sepa = 0;
    size_t ult_pos = 0;
    
    for (size_t act=0; act <= largo; act++) {        
        if (str[act] == '\0' || str[act] == sep) {
            char *palabra = substr(&str[ult_pos], act - ult_pos);
            if (!palabra) {
                free_strv(sepa);
                return NULL;
            }

            sepa[i_sepa] = palabra;
            ult_pos = act + 1;
            i_sepa++;
        }
    }
    
    return sepa;
}

char *join(char **strv, char sep) {
    size_t largo_pal = 0;
    size_t c_sep = 0;
    for (size_t i=0; strv[i]; i++) {
        largo_pal += strlen(strv[i]);
        c_sep ++;
    }
    
    char *unidas = calloc(sizeof(char), (largo_pal + c_sep + 1));
    if(!unidas) return NULL;
    size_t cont = 0;
    
    for (size_t i=0; strv[i]; i++) {
        for (size_t n=0; strv[i][n] != '\0'; n++) {
            unidas[cont] = strv[i][n];
            cont ++;
        }
        if (sep != '\0' && strv[i + 1]) {
            unidas[cont] = sep;
            cont++;
        }
    }
    
    return unidas;
}

