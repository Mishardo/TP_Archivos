#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#include "funciones.h"

#define IVA 23

// STRUCT
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

//---------------------FUNCIONES EXTRA--------------------------------

// Limpiar el buffer para evitar saltos de linea que causen errores
// Colocar cada vez que se haga un scanf
void limpiarBuffer()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void obtenerFechaActual(int *dia, int *mes, int *anio)
{
    time_t t = time(NULL);
    struct tm *fechaLocal = localtime(&t);

    *dia = fechaLocal->tm_mday;
    *mes = fechaLocal->tm_mon + 1;      // Los meses van de 0 a 11, por eso +1
    *anio = fechaLocal->tm_year + 1900; // Años desde 1900
}

void validarFecha(Calzados *cal)
{
    int dia_actual, mes_actual, anio_actual;
    obtenerFechaActual(&dia_actual, &mes_actual, &anio_actual);

    while (cal->anio > anio_actual)
    {
        printf("El anio es superior al anio acutal.\n");
        printf("Vuelva a intentarlo: ");
        scanf("%d", cal->anio);
        limpiarBuffer();
    }

    while (cal->mes > mes_actual)
    {
        printf("El mes ingresado es superior al mes actual.\n");
        printf("Intente de nuevo: ");
        scanf("%d", cal->mes);
        limpiarBuffer();
    }

    while (cal->dia > dia_actual)
    {
        printf("El dia ingresado es superior al dia acutal.\n");
        printf("Ingrese de nuevo: ");
        scanf("%d", cal->dia);
        limpiarBuffer();
    }
}

void validarSoloLetras(char *nombre)
{
    char c;
    for (int i = 0; nombre[i] != '\n' && nombre[i] != '\0'; i++)
    {
        c = nombre[i];

        // Comparo si está entre a y z
        if (c >= 'a' && c <= 'z')
        {
            if (i == 0)
            {
                nombre[i] = toupper(c); // Mayusculizo solo la primera letra
            }
            continue; // vuelvo al for
        }

        // Comparo si está entre A y Z
        else if (c >= 'A' && c <= 'Z')
        {
            if (i != 0)
            {
                nombre[i] = tolower(c); // Minusculizo solo si no es la primera letra
            }
            continue;
        }

        // Si el caracter no está entre ninguno de ellos, entonces no es una letra
        else
        {
            printf("El nombre ingresado no es válido, solo debe contener letras.\n");
            printf("Intente de nuevo: ");
            scanf("%s", nombre);
            limpiarBuffer();
            i = -1; // Como se ingresa nuevo texto, el ciclo debe volver a empezar
            continue;
        }
    }
}

void calcularImportes(Calzados *cal)
{
    cal->sub_total = cal->cantidad * cal->precio - cal->cantidad * cal->descuento;
    cal->iva = cal->sub_total * IVA;
    cal->total = cal->sub_total + cal->iva;
}

int leerOrden()
{
    int orden;

    printf("Ingrese el nro de orden");
    scanf("%d", &orden);
    limpiarBuffer();
    return orden;
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
    Calzados cal;
    archivo = fopen("registros.dat", "r+b");

    // °°°°Ingreso de datos y validaciones°°°°
    // Ingreso del ORDEN
    printf("Ingrese el nro de orden (mayor a 0): ");
    int lectura = scanf("%d", &cal.orden); // scanf si leyó correctamente el tipo de dato, devolverá 1
    limpiarBuffer();

    int bandera = 0;
    while (!bandera)
    {
        // Validar que no es un float o caracter
        if (lectura != 1)
        {
            printf("El numero ingresado no es un entero.\n");
            printf("Vuelva a intententarlo: ");
            lectura = scanf("%d", &cal.orden);
            limpiarBuffer();
            continue;
        }

        // Validar que no sea menor o igual a 0
        if (cal.orden <= 0)
        {
            printf("El numero ingresado no puede ser igual o menor que 0.\n");
            printf("Vuelva a intentarlo: ");
            scanf("%d", &cal.orden);
            limpiarBuffer();
            continue;
        }

        // Validar que no exista previamente
        // Seguramente funcione mal
        fseek(archivo, (cal.orden - 1) * sizeof(Calzados), SEEK_SET);
        if ((fread(&cal, sizeof(Calzados), 1, archivo)) == 1)
        {
            printf("El nro de orden ingresado se encuentra ocupado.\n");
            printf("Ingrese otro nro: ");
            scanf("%d", &cal.orden);
            limpiarBuffer();
            continue;
        }

        // Si pasó todas las validaciones, entonces ya puedo salir del while
        bandera = 1;
    }

    // Ingreso del NOMBRE VENDEDOR
    printf("Nombre del vendedor/a: ");
    scanf("%s", cal.vendedor);
    limpiarBuffer();
    validarSoloLetras(cal.vendedor);

    // Ingreso de la FECHA (Falta validar el caso base)
    printf("Fecha de ingreso del producto (No puede superar la fecha actual): ");
    scanf("%d %d %d", &cal.dia, &cal.mes, &cal.anio);
    limpiarBuffer();
    validarFecha(&cal);

    // Ingreso de la CATEGORIA
    printf("Ahora, ingrese la categoria del calzado: ");
    scanf("%s", cal.categoria);
    limpiarBuffer();
    validarSoloLetras(&cal.categoria);

    // Ingreso de la CANTIDAD
    printf("Ingrese la cantidad: ");
    scanf("%d", &cal.cantidad);
    limpiarBuffer();

    while (cal.cantidad < 0)
    {
        printf("Cantidad ingresada inferior a cero, ingreso invalido.\n");
        printf("Intente de nuevo: ");
        scanf("%d", &cal.cantidad);
        limpiarBuffer();
    }

    // Ingreso del PRECIO
    printf("Ingrese el precio unitario: ");
    scanf("%f", &cal.precio);
    limpiarBuffer();

    while (cal.precio <= 0)
    {
        printf("El precio ingresado no puede ser igual o inferior a 0.\n");
        printf("Ingrese de nuevo: ");
        scanf("%f", &cal.precio);
        limpiarBuffer();
    }

    // Ingreso del DESCUENTO
    printf("Ingrese el descuento por unidad (en porcentaje del 0 al 100 sin incluirlo): ");
    scanf("%f", &cal.descuento);
    limpiarBuffer();

    while (cal.descuento < 0 || cal.descuento >= 100)
    {
        printf("El descuento ingresado es incorrecto.\n");
        printf("Intente de nuevo: ");
        scanf("%f", &cal.descuento);
        limpiarBuffer();
    }

    cal.activo = 1; // Valido que el producto está activo

    // °Cuentas
    calcularImportes(&cal);

    // °Agregar datos al archivo
    fseek(archivo, (cal.orden - 1) * sizeof(Calzados), SEEK_SET);
    fwrite(&cal, sizeof(Calzados), 1, archivo);
    fclose(archivo);

    // *************FALTA TERMINAR*****************
}
// 4 (LISTAR BINARIO)

// 5 (BUSCAR)

// 6 (MODIFICAR)
void modificarProducto(FILE *archivo)
{
    Calzados cal;
    int cantidad_nueva;
    archivo = fopen("registros.dat", "r+b");
    if (archivo == NULL)
    {
        printf("Aún no se han ingresado productos\n");
        return;
    }
    int orden = leerOrden();

    int encontrado = 0;
    int validacion = 0;
    fseek(archivo, 0, SEEK_SET);
    while ((fread(&cal, sizeof(Calzados), 1, archivo)) == 1)
    {
        if (orden == cal.orden)
        {
            encontrado = 1;
            printf("ORDEN encontrado.\n");
            printf("Está seguro que desea modificar los datos de la orden %d? (1 para continuar): ", orden);
            scanf("%d", &validacion);
            limpiarBuffer();
            if (validacion)
            {
                printf("Ingrese la cantidad entera a agregar o quitar distinta de 0 (ej: 3 o -5): \n");
                scanf("%d", &cantidad_nueva);
                limpiarBuffer();

                if (cantidad_nueva == 0)
                {
                    printf("Se ha ingresado una cantidad incorrecta.\n");
                    printf("Volviendo al menu\n");
                    fclose(archivo);
                    return;
                }
                else if (cantidad_nueva + cal.cantidad < 0)
                {
                    printf("El resultado de la operacion volverá negativa la cantidad existente.\n");
                    printf("Volviendo al menu\n");
                    fclose(archivo);
                    return;
                }

                cal.cantidad += cantidad_nueva;

                calcularImportes(&cal);

                fseek(archivo, -sizeof(Calzados), SEEK_CUR);
                fwrite(&cal, sizeof(Calzados), 1, archivo);
                break;
            }
            else
            {
                printf("Se ha seleccionado el no querer realizar una modificacion\n");
                fclose(archivo);
                return;
            }
        }
    }
    if (!encontrado)
    {
        printf("La orden %d no se ha encontado.\n", orden);
        fclose(archivo);
        return;
    }

    fclose(archivo);
}
// 7 (BAJA LOGICA)

// 8 (BAJA FISICA)

// 9 (LISTAR TEXTO)

//-------------------------------------------------------------------------------------

int main()
{
    // MENU

    return 0;
}
