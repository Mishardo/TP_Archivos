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

    // Validaciones
    while (cal->anio > anio_actual)
    {
        printf("El anio es superior al anio acutal.\n");
        printf("Vuelva a intentarlo: ");
        scanf("%d", &cal->anio);
        limpiarBuffer();
    }

    while (cal->mes > mes_actual)
    {
        printf("El mes ingresado es superior al mes actual.\n");
        printf("Intente de nuevo: ");
        scanf("%d", &cal->mes);
        limpiarBuffer();
    }

    while (cal->dia > dia_actual)
    {
        printf("El dia ingresado es superior al dia acutal.\n");
        printf("Ingrese de nuevo: ");
        scanf("%d", &cal->dia);
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
    cal->iva = cal->sub_total * IVA / 100;
    cal->total = cal->sub_total + cal->iva;
}

int leerOrden()
{
    int orden;

    printf("Ingrese el nro de orden: ");
    scanf("%d", &orden);
    limpiarBuffer();
    return orden;
}

char *obtenerNombreArchivo()
{
    char nombre[20];
    int dia_actual, mes_actual, anio_actual;
    obtenerFechaActual(&dia_actual, &mes_actual, &anio_actual);
    snprintf(nombre, sizeof(nombre), "bajas_%d-%d-%d.xyz", dia_actual, mes_actual, anio_actual);

    return nombre;
}
// ---------------------------FUNCIONES PEDIDAS--------------------------
//(Despues las pasamos a un archivo a parte donde estáran todas las funciones)

// 1 (MENU)

// 2 (CREAR ARCHIVO BINARIO)
void crearBinario(FILE *archivo)
{
    archivo = fopen("registro.dat", "w+b");

    if (archivo == NULL)
    {
        printf("El archivo no pudo ser creado\n");
    }
    else
    {
        printf("Archivo binario creado con exito\n");
        fclose(archivo);
    }
}

// 3 (ALTA)
void altaProducto(FILE *archivo)
{
    Calzados cal;
    archivo = fopen("registro.dat", "r+b");
    if (archivo == NULL)
    {
        printf("No se ha podido abir el archivo.\n");
        return;
    }

    // °°°°Ingreso de datos y validaciones°°°°
    // Ingreso del ORDEN
    int lectura, orden_temporal;
    printf("Ingrese el nro de orden (mayor a 0): ");
    lectura = scanf("%d", &orden_temporal); // scanf si leyó correctamente el tipo de dato, devolverá 1
    limpiarBuffer();

    int bandera = 0;
    while (!bandera)
    {
        // Validar que no es un float o caracter
        if (lectura != 1)
        {
            printf("El numero ingresado no es un entero.\n");
            printf("Vuelva a intententarlo: ");
            lectura = scanf("%d", &orden_temporal);
            limpiarBuffer();
            continue;
        }

        // Validar que no sea menor o igual a 0
        if (orden_temporal <= 0)
        {
            printf("El numero ingresado no puede ser igual o menor que 0.\n");
            printf("Vuelva a intentarlo: ");
            scanf("%d", &orden_temporal);
            limpiarBuffer();
            continue;
        }

        // Validar que no exista previamente
        Calzados temporal;
        fseek(archivo, (orden_temporal - 1) * sizeof(Calzados), SEEK_SET); // Me paro en la linea en donde estaría al
        if ((fread(&temporal, sizeof(Calzados), 1, archivo)) == 1)         // fread va a almacenar los datos en temp y dará 1 si existen los datos.
        {
            if (orden_temporal == temporal.orden && temporal.activo == 1)
            {
                printf("El nro de orden ingresado se encuentra ocupado.\n");
                printf("Ingrese otro nro: ");
                scanf("%d", &orden_temporal);
                limpiarBuffer();
                continue;
            }
        }

        // Si pasó todas las validaciones, entonces ya puedo salir del while
        bandera = 1;
        cal.orden = orden_temporal;
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
    validarSoloLetras(cal.categoria);

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
    fwrite(&cal, sizeof(Calzados), 1, archivo); // escribo los datos que están en 'cal' en el archivo

    printf("Se ha aniadido el producto.\n");
    fclose(archivo);
}
// *************FALTA TERMINAR*****************

// 4 (LISTAR BINARIO)

// 5 (BUSCAR)

// 6 (MODIFICAR)
void modificarProducto(FILE *archivo)
{
    Calzados cal;
    int cantidad_nueva;
    archivo = fopen("registro.dat", "r+b");

    if (archivo == NULL)
    {
        printf("Aún no se han ingresado productos\n");
        return;
    }

    // Validar que haya productos
    fseek(archivo, 0, SEEK_END);
    if ((ftell(archivo) / sizeof(Calzados)) == 0)
    {
        printf("No se han ingresado productos.\n");
        fclose(archivo);
        return;
    }

    // Leer la orden a buscar
    int orden = leerOrden();

    int orden_encontrada = 0;
    int validacion = 0;
    fseek(archivo, 0, SEEK_SET);
    while ((fread(&cal, sizeof(Calzados), 1, archivo)) == 1)
    {
        if (orden == cal.orden)
        {
            orden_encontrada = 1;
            printf("ORDEN encontrado.\n");
            // Validar que se quiera realizar la modificacion
            printf("Esta seguro que desea modificar los datos de la orden %d? (1 para continuar): ", orden);
            scanf("%d", &validacion);
            limpiarBuffer();

            if (validacion == 1)
            {
                printf("Ingrese la cantidad entera a agregar o quitar distinta de 0 (ej: 3 o -5): ");
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
    if (!orden_encontrada)
    {
        printf("La orden %d no se ha encontado.\n", orden);
        fclose(archivo);
        return;
    }

    printf("Modificacion realizada.\n");
    fclose(archivo);
}
// ********************** VERIFICAR QUE FUNCIONA *********************

// 7 (BAJA LOGICA)
void bajaLogica(FILE *archivo)
{
    Calzados cal;
    archivo = fopen("registro.dat", "r+b");
    if (archivo == NULL)
    {
        printf("No hay productos para dar de baja.\n");
        return;
    }

    // Validar que haya productos
    int cantidad_datos = ftell(archivo) / sizeof(Calzados);
    if (cantidad_datos == 0)
    {
        printf("No se han ingresado productos.\n");
        fclose(archivo);
        return;
    }

    // Validar que haya productos para dar de baja
    int activo_encontado = 0;
    for (int i = 0; i < cantidad_datos; i++)
    {
        fread(&cal, sizeof(Calzados), 1, archivo);
        if (cal.activo == 1)
        {
            activo_encontado = 1;
        }
    }
    if (!activo_encontado)
    {
        printf("No se han encontardo productos activos.\n");
        fclose(archivo);
        return;
    }

    // Lectura de la orden a buscar
    int orden = leerOrden();

    int orden_encontrada = 0;
    int validacion = 0;
    fseek(archivo, 0, SEEK_SET);
    while ((fread(&cal, sizeof(Calzados), 1, archivo)) == 1)
    {
        if (orden == cal.orden)
        {
            orden_encontrada = 1;
            // Datos de la orden encontada
            printf("Vendedor\tFecha\t\tCategoria\tCantidad\tPrecio Unitario\t\tDescuento\tSubtotal\tI.V.A\tTotal\tActivo\n");
            printf("%2s %10d/%d/%d %15s %12d %22.2f %19.2f %16.2f %10.2f %10.2f %10d\n", cal.vendedor, cal.dia, cal.mes, cal.anio, cal.categoria, cal.cantidad, cal.precio, cal.descuento, cal.sub_total, cal.iva, cal.total, cal.activo);
            // Validacion de querer darlo de baja
            printf("Estas seguro de querer darlo de baja? (1 para continuar): ");
            scanf("%d", &validacion);

            if (validacion == 1)
            {
                // Colocacion y reescritura del producto dado de baja
                cal.activo = 0;
                fseek(archivo, -sizeof(Calzados), SEEK_CUR);
                fwrite(&cal, sizeof(Calzados), 1, archivo);
                printf("El producto ha sido colocado como inactivo");

                // Mostrar los productos que quedaron activos
                fseek(archivo, 0, SEEK_SET);
                activo_encontado = 0;
                int primera_impresion = 0;
                for (int i = 0; i < cantidad_datos; i++)
                {
                    fread(&cal, sizeof(Calzados), 1, archivo);
                    if (cal.activo == 1)
                    {
                        activo_encontado = 1;
                        // Mostrar el encabezado solo una vez
                        if (!primera_impresion)
                        {
                            printf("Orden\tVendedor\tFecha\tCategoria\tCantidad\tPrecio Unitario\tDescuento\tSubtotal\tI.V.A\tTotal\tActivo\n");
                            primera_impresion = 1;
                        }
                        printf("%2d %10s %10d/%d/%d %10s %10d %10.2f %10.2f %10.2f %10.2f %10.2f %10d\n", cal.orden, cal.vendedor, cal.dia, cal.mes, cal.anio, cal.categoria, cal.cantidad, cal.precio, cal.descuento, cal.sub_total, cal.iva, cal.total, cal.activo);
                    }
                }

                // En caso de que no haya quedado al menos un producto activo
                if (!activo_encontado)
                {
                    printf("No quedaron productos activos.\n");
                    fclose(archivo);
                    return;
                }
            }
            else
            {
                printf("Se ha seleccionado no querer darlo de baja.\n");
                printf("Volviendo al menu.\n");
                fclose(archivo);
                return;
            }
        }
    }

    // Si orden_encontada se mantuvo en 0, !0 = 1
    if (!orden_encontrada)
    {
        printf("No se ha encontado el orden %d\n", orden);
        fclose(archivo);
        return;
    }

    fclose(archivo);
}
// ********************** VERIFICAR QUE FUNCIONA *********************

// 8 (BAJA FISICA)
void bajaFisica(FILE *archivo)
{
    Calzados cal;

    archivo = fopen("registro.dat", "r+b");
    if (archivo == NULL)
    {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    // Validar que no hay productos
    fseek(archivo, 0, SEEK_END);
    int cantidad_datos = ftell(archivo) / sizeof(Calzados);
    if (cantidad_datos == 0)
    {
        printf("No se han ingresado productos.\n");
        fclose(archivo);
        return;
    }

    // Creacion del archivo con la fecha actual (Ver si lo puedo optimizar)
    char *nombre_archivo;
    nombre_archivo = obtenerNombreArchivo();
    FILE *archivo_txt;
    archivo_txt = fopen(nombre_archivo, "wt");

    fseek(archivo, 0, SEEK_SET);
    fseek(archivo_txt, 0, SEEK_SET);

    int encontrado = 0;
    for (int i = 0; i < cantidad_datos; i++)
    {
        fread(&cal, sizeof(Calzados), 1, archivo);
        if (cal.activo == 0)
        {
            encontrado = 1;
            // grabado de datos en archivo de texto (ARREGLAR IMPRESION DE DATOS)
            fprintf(archivo_txt, "Orden\tVendedor\tFecha\t\tCategoria\tCantidad\tPrecio Unitario\t\tDescuento\tSubtotal\tI.V.A\tTotal\tActivo\n");
            fprintf(archivo_txt, "%2d %2s %10d/%d/%d %15s %12d %22.2f %19.2f %16.2f %10.2f %10.2f %10d\n", cal.orden, cal.vendedor, cal.dia, cal.mes, cal.anio, cal.categoria, cal.cantidad, cal.precio, cal.descuento, cal.sub_total, cal.iva, cal.total, cal.activo);

            // incializado todos los campos en cero
            fseek(archivo, -sizeof(Calzados), SEEK_CUR);
            Calzados cal = {0, {0}, 0, 0, 0, {0}, 0, 0, 0, 0, 0, 0, 0};
            fwrite(&cal, sizeof(Calzados), 1, archivo);
        }
    }

    if (!encontrado)
    {
        printf("No se han encontrado productos inactivos\n");
        printf("Archivo de texto no ha sido creado.\n");
        fclose(archivo);
        remove(nombre_archivo);
        return;
    }

    printf("El archivo de texto con los productos inactivos ha sido creado.\n");

    fclose(archivo);
    remove(nombre_archivo);
}
// ******************* FALTA ARREGLAR ******************

// 9 (LISTAR TEXTO)

//-------------------------------------------------------------------------------------

int main()
{
    // MENU

    return 0;
}
