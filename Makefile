# CREADO POR ALVARO MARTIN EN PYTHON :)

CFLAGS = -g -std=c99 -Wall -Wconversion -Wtype-limits -pedantic
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

# OBJETOS A BORRAR
REMOVE = infix.o mod/pila/pila.o mod/cola/cola.o mod/strutil/strutil.o


# COMPILACION DE OBJETOS

infix.o: infix.c
	@- $(CC) -c $(CFLAGS) $^ -o $@

mod/pila/pila.o: mod/pila/pila.c
	@- $(CC) -c $(CFLAGS) $^ -o $@

mod/cola/cola.o: mod/cola/cola.c
	@- $(CC) -c $(CFLAGS) $^ -o $@

mod/strutil/strutil.o: mod/strutil/strutil.c
	@- $(CC) -c $(CFLAGS) $^ -o $@


# COMPILACION DE EJECUTABLES

infix: infix.o mod/pila/pila.o mod/cola/cola.o mod/strutil/strutil.o
	@- $(CC) $(CFLAGS) $^ -o $@


# LIMPIAR

clean:
	@- rm -r $(REMOVE)
