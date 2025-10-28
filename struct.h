// struct.h
// Acá defino los structs usados (En este caso, solo uno)
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

typedef struct
{
    int orden;
    char vendedor[20];
    int dia, mes, anio;
    char categoria[15];
    int cantidad;
    float precio;
    float descuento;
    float sub_total;
    float iva;
    float total;
    int activo;
} Calzados;