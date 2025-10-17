#include <stdio.h>
#include <ctype.h>
#include "funciones.h"

// STRUCT
typedef struct
{
    int orden;
    char vendedor[20];
    char fecha[10];
    char categoria[15];
    int cantidad;
    float precio;
    float descuento;
    float sub_total;
    float iva;
    float total;
    int activo;
} calzados;

//---------------------FUNCIONES EXTRA--------------------------------

// Limpiar el buffer para evitar saltos de linea que causen errores
// Colocar cada vez que se haga un scanf
void limpiarBuffer()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ---------------------------FUNCIONES PEDIDAS--------------------------
//(Despues las pasamos a un archivo a parte donde estáran todas las funciones)

// 1 (MENU)

// 2 (CREAR ARCHIVO BINARIO)
void crearBinario(FILE *archivo)
{
    if ((archivo = fopen("registro.dat", "w+b")) == NULL)
    {
        printf("El archivo no pudo ser creado");
    }
    else
    {
        printf("Archivo binario creado con exito");
        fclose(archivo);
    }
}

// 3 (ALTA)
void altaProducto(FILE *archivo)
{
    calzados cal;
    archivo = fopen("registros.dat", "r+b");

    printf("Ingrese el nro de orden (mayor a 0): ");
    scanf("%d", &cal.orden);

    limpiarBuffer();

    while (cal.orden <= 0)
    {
        printf("El numero ingresado es erroneo.\n");
        printf("Vuelva a intentarlo: ");
        scanf("%d", &cal.orden);
        limpiarBuffer();
    }

    // *************FALTA TERMINAR*****************
}
// 4 (LISTAR BINARIO)

// 5 (BUSCAR)

// 6 (MODIFICAR)

// 7 (BAJA LOGICA)

// 8 (BAJA FISICA)

// 9 (LISTAR TEXTO)

//-------------------------------------------------------------------------------------

int main()
{
    // MENU

    return 0;
}
