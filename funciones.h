// Acá solo se llaman a la funciones EJ: int sumar(int a,int b);
// "funciones.h"
#include "funciones.c"

//---------------------FUNCIONES EXTRA--------------------------------
void limpiarBuffer();

void guardarOrden(int primer_orden);

int obtenerOrdenTemp();

void obtenerFechaActual(int *dia, int *mes, int *anio);

int encontrarMenorFecha(FILE *archivo);

void validarFecha(Calzados *cal, FILE *archivo);

void validarSoloLetras(char *nombre);

void calcularImportes(Calzados *cal);

int leerOrden();

void obtenerNombreArchivo(char *nombre, int tamanio);

void guardarImpresion();

int obtenerImpresionTemp();
// ---------------------------FUNCIONES PEDIDAS--------------------------
// 1 (MENU)
int menu();

// 2 (CREAR ARCHIVO BINARIO)
void crearBinario(FILE *archivo);

// 3 (ALTA)
void altaProducto(FILE *archivo);

// 4 (LISTAR BINARIO)
void lista_todos(FILE *archivo, Calzados *cal);

void lista_categoria(FILE *archivo, Calzados *cal);

void listarProdutos(FILE *archivo);

// 5 (BUSCAR)
void busca_venta_por_numero(FILE *archivo, Calzados *cal);

void busca_venta_por_nombre(FILE *archivo, Calzados *cal);

void buscarProducto(FILE *archivo);

// 6 (MODIFICAR)
void modificarProducto(FILE *archivo);

// 7 (BAJA LOGICA)
void bajaLogica(FILE *archivo);

// 8 (BAJA FISICA)
void bajaFisica(FILE *archivo);

// 9 (LISTAR TEXTO)
void listar_texto(FILE *archivo);

// Parte del menu
void realizarOperacion(int eleccion, FILE *archivo);