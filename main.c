/* DOCUMENTAICION
-Autores: 
(1) Dante, Mecha
(2) David Ezekiel, Veron

Fecha de entrega: ????

-Descripción: Este programa gestiona productos de calzado, permitiendo agregar, listar y modificar productos 
almacenados en un archivo binario, como también darlos de baja a los productos y poder crear un archivo de texto en donde se almacenaran estos productos inactivos.

-Archivos utilizados:
main.c
funciones.h
funciones.c
struct.h

-Funciones utlizadas: 
void limpiarBuffer();
void obtenerFechaActual(int *dia, int *mes, int *anio);
int encontrarMenorFecha(FILE *archivo);
void validarFecha(Calzados *cal, FILE *archivo);
void validarSoloLetras(char *nombre);
void calcularImportes(Calzados *cal);
int leerOrden();
void obtenerNombreArchivo(char *nombre, int tamanio);
int menu();
void crearBinario(FILE *archivo);
void altaProducto(FILE *archivo);
void lista_todos(FILE *archivo, Calzados *cal);
void lista_categoria(FILE *archivo, Calzados *cal);
void listarProdutos(FILE *archivo);
void busca_venta_por_numero(FILE *archivo, Calzados *cal);
void busca_venta_por_nombre(FILE *archivo, Calzados *cal);
void buscarProducto(FILE *archivo);
void modificarProducto(FILE *archivo);
void bajaLogica(FILE *archivo);
void bajaFisica(FILE *archivo);
void listar_texto(FILE *archivo);
void realizarOperacion(int eleccion, FILE *archivo);

-ACLARACIION (IMPORTANTE): 
Tener en cuenta que si se cierra el programa, se perderá tanto del dato de la 'Fecha_base' como de la validacíon de que el encabezado ya ha sido colocado en el archivo de texto
*/
#include "funciones.h"

int main()
{
    FILE *archivo = NULL;
    int eleccion;
    do
    {
        eleccion = menu();
        realizarOperacion(eleccion, archivo);
    } while (eleccion != 0);

    printf("Saliendo del programa.");
    return 0;
}