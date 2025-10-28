// funciones.c
#include "struct.h" // Se incluye la librefía 

#define IVA 23 // EL IVA usado en el ejemplo fue 23

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
    time_t t = time(NULL);                 // Obtiene el tiempo actual en segundos desde la época (1970-01-01)
    struct tm *fechaLocal = localtime(&t); // Convierte ese tiempo en una estructura tm con la fecha y hora local

    *dia = fechaLocal->tm_mday;                 // tm_mday: día del mes
    *mes = fechaLocal->tm_mon + 1;              // tm_mon: mes del año (0-11), por eso se suma 1
    *anio = (fechaLocal->tm_year + 1900) % 100; // tm_year: años desde 1900, se suma 1900 para obtener el año real y se lo reduce a dos digitos con % 100
}

int encontrarMenorFecha(FILE *archivo)
{
    Calzados cal;
    int anio_minimo = 9999;
    int mes_minimo = 9999;
    int dia_minimo = 9999;

    fseek(archivo, 0, SEEK_END);
    int cantidad_datos = ftell(archivo) / sizeof(Calzados);

    int anio_encontrado = 0;
    fseek(archivo, 0, SEEK_SET);
    for (int i = 0; i < cantidad_datos; i++)
    {
        fread(&cal, sizeof(Calzados), 1, archivo); // Almaceno los datos en 'cal'

        if (cal.anio < anio_minimo && cal.anio != 0)
        {
            anio_minimo = cal.anio;
            anio_encontrado = 1; // Si lo encontró, lo valido. Sino, significa que no existe otro producto así que retornará 0 el nro_orden
        }
    }

    fseek(archivo, 0, SEEK_SET);
    for (int i = 0; i < cantidad_datos && anio_encontrado == 1; i++)
    {
        fread(&cal, sizeof(Calzados), 1, archivo);

        if (cal.mes < mes_minimo && cal.anio == anio_minimo) // Verifico que ese mes minimo contenga su anio correspondiente y no cualquier anio.
        {
            mes_minimo = cal.mes;
        }
    }

    int orden = 0;
    fseek(archivo, 0, SEEK_SET);
    for (int i = 0; i < cantidad_datos && anio_encontrado == 1; i++)
    {
        fread(&cal, sizeof(Calzados), 1, archivo);

        if (cal.dia < dia_minimo && cal.mes == mes_minimo && cal.anio == anio_minimo) // Misma idea pero añadiendo el mes
        {
            dia_minimo = cal.dia;
            orden = cal.orden; // Una vez obtenida la fecha, asigno la orden correspondiente
        }
    }

    return orden; // Retorno el valor encontrado
}

void validarFecha(Calzados *cal, FILE *archivo)
{
    int dia_actual, mes_actual, anio_actual;
    obtenerFechaActual(&dia_actual, &mes_actual, &anio_actual);

    int validacion = 0;
    do
    {
        // Validar que tenga un formato correcto
        struct tm fecha = {0};
        fecha.tm_mday = cal->dia;
        fecha.tm_mon = cal->mes - 1;     // meses van de 0–11
        fecha.tm_year = cal->anio + 2000 - 1900; // años desde 1900
        // Se normaliza la fecha con mktime()
        time_t t = mktime(&fecha);
        struct tm *f = localtime(&t);
        // Se compara si la fecha normalizada es distinta de la fecha ingresada.
        // Si resulta serlo, entonces la fecha tenía un formato inválido
        if (f->tm_mday != cal->dia || f->tm_mon != cal->mes - 1 || f->tm_year != cal->anio + 2000 - 1900)
        {
            printf("La fecha ingresada tiene formato incorrecto.\n");
            printf("Vuelva a intentarlo: ");
            scanf("%d %d %d", &cal->dia, &cal->mes, &cal->anio);
            limpiarBuffer();
            continue; // Repetir el ciclo
        }

        // Validar que la fecha no supere a la fecha actual
        while (cal->anio > anio_actual)
        {
            printf("El anio es superior al anio actual (FECHA ACTUAL: %d/%d/%d).\n", dia_actual, mes_actual, anio_actual);
            printf("Vuelva a intentarlo ingresando un anio igual o inferior al actual: ");
            scanf("%d", &cal->anio);
            limpiarBuffer();
        }

        while (cal->mes > mes_actual && cal->anio == anio_actual)
        {
            printf("El mes ingresado es superior al mes actual (FECHA ACTUAL: %d/%d/%d).\n", dia_actual, mes_actual, anio_actual);
            printf("Intente de nuevo ingresando un mes igual o inferior al actual: ");
            scanf("%d", &cal->mes);
            limpiarBuffer();
        }

        while (cal->dia > dia_actual && cal->mes == mes_actual && cal->anio == anio_actual)
        {
            printf("El dia ingresado es superior al dia acutal (FECHA ACTUAL: %d/%d/%d).\n", dia_actual, mes_actual, anio_actual);
            printf("Ingrese de nuevo un dia igual o inferior al actual: ");
            scanf("%d", &cal->dia);
            limpiarBuffer();
        }

        // Validar que la fecha no sea anterior a la fecha base (la primera fecha ingresada en el programa)
        static int primer_orden = 0;
        Calzados primerIngreso;

        Calzados temporal;
        int orden_encontrado = 0;
        if (primer_orden == 0)
        {
            primer_orden = cal->orden;
        }
        else
        {
            // Valido que la 'orden' siga existiendo debido a que en bajaLogica pueden dar de baja justo la orden que está siendo usada como fecha base
            fseek(archivo, 0, SEEK_SET);
            while ((fread(&temporal, sizeof(Calzados), 1, archivo)) == 1)
            {
                if (primer_orden == temporal.orden)
                {
                    orden_encontrado = 1;
                    break;
                }
            }
            if (!orden_encontrado)
            {
                primer_orden = encontrarMenorFecha(archivo); // Si no encontró la orden, debe asignar una nueva orden con la menor fecha registrada
                // Si la función retornó 0, debo asegurar que se le asigne el nro de orden que ingresó el usuario
                if (primer_orden == 0)
                {
                    primer_orden = cal->orden;
                    break; // Rompo el do-while para que de por válido el ingreso de la fecha base
                }
            }

            fseek(archivo, (primer_orden - 1) * sizeof(Calzados), SEEK_SET);
            fread(&primerIngreso, sizeof(Calzados), 1, archivo);

            if (cal->anio < primerIngreso.anio)
            {
                printf("El anio ingresado es inferior al 1er anio registrado (FECHA BASE: %d/%d/%d).\n", primerIngreso.dia, primerIngreso.mes, primerIngreso.anio);
                printf("Intente de nuevo ingresando un anio supeior o igual: ");
                scanf("%d", &cal->anio);
                limpiarBuffer();
                continue; // saltará al while en donde evaluará la validacion, como sigue en 0 el codigo volverá a empezar desde la 1ra validacion
            }

            if (cal->mes < primerIngreso.mes && cal->anio == primerIngreso.anio)
            {
                printf("El mes ingresado es inferior al 1er mes registrado (FECHA BASE: %d/%d/%d).\n", primerIngreso.dia, primerIngreso.mes, primerIngreso.anio);
                printf("Intente de nuevo ingresando un mes supeior o igual: ");
                scanf("%d", &cal->mes);
                limpiarBuffer();
                continue; // saltará al while en donde evaluará la validacion, como sigue en 0 el codigo volverá a empezar desde la 1ra validacion
            }

            if (cal->dia < primerIngreso.dia && cal->mes == primerIngreso.mes && cal->anio == primerIngreso.anio)
            {
                printf("El dia ingresado es inferior al 1er dia registrado (FECHA BASE: %d/%d/%d).\n", primerIngreso.dia, primerIngreso.mes, primerIngreso.anio);
                printf("Intente de nuevo ingresando un dia superior o igual: ");
                scanf("%d", &cal->dia);
                limpiarBuffer();
                continue; // saltará al while en donde evaluará la validacion, como sigue en 0 el codigo volverá a empezar desde la 1ra validacion
            }
        }
        // Si pasó todas las validaciones o es el 1er producto ingresado, dará el OK dando por válida la fecha.
        validacion = 1;
    } while (!validacion);
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
            i = -1; // Como se ingresa un nuevo texto, el ciclo debe volver a empezar
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
    int orden = 0;

    printf("Ingrese el nro de orden: ");
    scanf("%d", &orden);
    limpiarBuffer();
    while (orden <= 0)
    {
        printf("El nro de orden ingresado no es correcto.\n");
        printf("Intente de nuevo: ");
        scanf("%d", &orden);
        limpiarBuffer();
    }

    return orden;
}

void obtenerNombreArchivo(char *nombre, int tamanio)
{
    int dia_actual, mes_actual, anio_actual;
    obtenerFechaActual(&dia_actual, &mes_actual, &anio_actual);
    // snprintf reemplazará los %d colocados por los valores de dichas variables
    // 'nombre' es en donde esa cadena será almacenada y 'tamanio' es para validar que no se desborde. En caso de que se desbore, la operación no se realiza.
    snprintf(nombre, tamanio, "bajas_%d-%d-%d.xyz", dia_actual, mes_actual, anio_actual); 
}

// ---------------------------FUNCIONES PEDIDAS--------------------------
// 1 (MENU)
int menu() {
    int operacion = 10;

    do {
        printf("Ingrese el numero de la accion que desea ejecutar:\n");
        printf(" 1. Crear archivo.\n");
        printf(" 2. Registrar nuevo producto.\n");
        printf(" 3. Listar los productos.\n");
        printf(" 4. Buscar un producto.\n");
        printf(" 5. Modificar producto (cantidad).\n");
        printf(" 6. Dar de baja un producto.\n");
        printf(" 7. Crear archivo de productos dados de baja.\n");
        printf(" 8. Listar las bajas fisicas realizadas.\n");
        printf(" 0. Para finalizar el programa.\n");
        scanf("%d", &operacion);
        limpiarBuffer();

        if (operacion < 0 || operacion > 8)
            printf("Opcion invalida. Ingrese un número entre 1 y 8.\n");

    } while (operacion < 0 || operacion > 8); 

    return operacion;
}

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
        // Validar que no es un caracter
        if (lectura != 1)
        {
            printf("El numero ingresado no es un entero.\n");
            printf("Vuelva a intententarlo: ");
            lectura = scanf("%d", &orden_temporal);
            limpiarBuffer();
            continue;
        }

        // Validar que no es un float (No está funcionando)
        /*char c = getchar();
        if ((c = getchar()) != '\n' && c != EOF) // Si despues de leer la parte entera no es un enter o un fin del archivo, entonces es un nro flotante
        {
            printf("Se ingresó un numero flotante.\n");
            printf("Intente de nuevo: ");
            lectura = scanf("%d", &orden_temporal);
            limpiarBuffer();
            continue;
        }*/

        // Validar que no sea menor o igual a 0
        if (orden_temporal <= 0)
        {
            printf("El numero ingresado no puede ser igual o menor que 0.\n");
            printf("Vuelva a intentarlo: ");
            lectura = scanf("%d", &orden_temporal);
            limpiarBuffer();
            continue;
        }

        // Validar que no exista previamente
        Calzados temporal;
        fseek(archivo, (orden_temporal - 1) * sizeof(Calzados), SEEK_SET); // Me paro en la linea en donde estaría la orden
        if ((fread(&temporal, sizeof(Calzados), 1, archivo)) == 1)         // fread va a almacenar los datos en temp y dará 1 si existen los datos.
        {
            if (orden_temporal == temporal.orden && temporal.activo == 1)
            {
                printf("El nro de orden ingresado se encuentra ocupado.\n");
                printf("Ingrese otro nro: ");
                lectura = scanf("%d", &orden_temporal);
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
    scanf("%s", cal.vendedor); // Suponiendo que solo ingresa un nombre y no varios
    limpiarBuffer();
    validarSoloLetras(cal.vendedor);

    // Ingreso de la FECHA (Error al momento de validar)
    printf("Fecha de ingreso del producto\n");
    printf("Fomato dia/mes/anio (separado por espacios)\n");
    printf("FECHA: ");
    scanf("%d %d %d", &cal.dia, &cal.mes, &cal.anio);
    limpiarBuffer();
    validarFecha(&cal, archivo);

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

// 4 (LISTAR BINARIO)
void lista_todos(FILE *archivo, Calzados *cal){
    int primera_impresion = 0;
    fseek(archivo, 0, SEEK_SET);
    printf("Listado de todos los archivos:\n");
    while ((fread(cal, sizeof(Calzados), 1, archivo)) == 1){
        if (!primera_impresion)
        {
            printf("Orden  Vendedor      Fecha     Categoria     Cantidad  Precio Unitario  Descuento   Subtotal     I.V.A       Total      Activo\n");
            printf("-------------------------------------------------------------------------------------------------------------------------------\n");
            primera_impresion = 1;
        }
        // Si la fecha está vacía, poner solo el 0
        if (cal->dia == 0 && cal->mes == 0 && cal->anio == 0)
        {
            printf("%-6d %-12s %-9s %-12s %9d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n",cal->orden, cal->vendedor, "0", cal->categoria, cal->cantidad, cal->precio, cal->descuento, cal->sub_total, cal->iva, cal->total, cal->activo);
        }
        else
        {
            printf("%-6d %-12s %02d/%02d/%02d \t%s %13d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal->orden, cal->vendedor, cal->dia, cal->mes, cal->anio, cal->categoria, cal->cantidad, cal->precio, cal->descuento, cal->sub_total, cal->iva, cal->total, cal->activo);
        }
    }
    printf("\nFin del listado\n");

    fclose(archivo);
}

void lista_categoria(FILE *archivo, Calzados *cal){
    char busca_categoria[15];
    printf("Ingrese el nombre de la categoria que desea buscar\n");
    scanf("%s", busca_categoria);
    limpiarBuffer();
    validarSoloLetras(busca_categoria); // Acá, además de validar que se esté ingresando solo letras, tambíen vuelvo la 1ra letra mayúscula y el resto minuscula
    // Logrando que la comparacion en strcmp funcione independiente de como escrita la palabra. EJ: oJoTa --> Ojota

    int categoria_encontrada = 0;
    int primera_impresion = 0;
    fseek(archivo, 0, SEEK_SET);
    while ((fread(cal, sizeof(Calzados), 1, archivo)) == 1){
        if (strcmp(busca_categoria, cal->categoria) == 0){
            if (!primera_impresion)
            {
                printf("Orden  Vendedor      Fecha     Categoria     Cantidad  Precio Unitario  Descuento   Subtotal     I.V.A       Total      Activo\n");
                printf("-------------------------------------------------------------------------------------------------------------------------------\n");
                primera_impresion = 1;
                categoria_encontrada = 1;
            }
            // Si la fecha está vacía, poner solo el 0
            if (cal->dia == 0 && cal->mes == 0 && cal->anio == 0)
            {
                printf("%-6d %-12s %-9s %-12s %9d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal->orden, cal->vendedor, "0", cal->categoria, cal->cantidad, cal->precio, cal->descuento, cal->sub_total, cal->iva, cal->total, cal->activo);
            }
            else
            {
                printf("%-6d %-12s %02d/%02d/%02d \t%s %13d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal->orden, cal->vendedor, cal->dia, cal->mes, cal->anio, cal->categoria, cal->cantidad, cal->precio, cal->descuento, cal->sub_total, cal->iva, cal->total, cal->activo);
            }
        }
    }
    if (!categoria_encontrada){
        printf("La categoria %s no se ha encontrado.\n", busca_categoria);
        fclose(archivo);
    }
    else{
        printf("\nFin del listado.\n");
        fclose(archivo);
    }
}

void listarProdutos(FILE *archivo)
{
    Calzados cal;
    archivo = fopen("registro.dat", "rb");

    if (archivo == NULL) {
    printf("No se pudo abrir el archivo.\n");
    return;
    }

    // Valido que haya productos ingresados
    fseek(archivo, 0, SEEK_END);
    int cantidad_datos = ftell(archivo) / sizeof(Calzados);
    if (cantidad_datos == 0){
        printf("No hay productos ingresados.\n");
        fclose(archivo);
        return;
    }

    char listado;
    do {
        printf("\nIndique como desea listar:\n");
        printf("a) Listar todos (activos e inactivos)\n");
        printf("b) Listar por categoría ingresada\n");
        printf("Ingrese una opcion (a/b): ");
        scanf(" %c", &listado);

        if ((listado != 'a' && listado != 'A') &&
            (listado != 'b' && listado != 'B'))
            printf("Opcion invalida. Ingrese a o b.\n");

    } while ((listado != 'a' && listado != 'A') && (listado != 'b' && listado != 'B'));

    if ((listado == 'a' || listado == 'A')) lista_todos(archivo, &cal);
    if ((listado == 'b' || listado == 'B')) lista_categoria(archivo, &cal);
}

// 5 (BUSCAR)
void busca_venta_por_numero(FILE *archivo, Calzados *cal){
    int busca_numero;
    printf("Ingrese el número de producto que desea buscar\n");
    scanf("%d", &busca_numero);
    limpiarBuffer();

    // valido que el nro a buscar sea mayor que 0
    while (busca_numero <= 0){
        printf("El numero ingresado es menor o igual que cero (0).\n");
        printf("Vuelva a intentarlo: ");
        scanf("%d", &busca_numero);
        limpiarBuffer();
    }

    int primera_impresion = 0;
    int numero_encontrado = 0;
    fseek(archivo, 0, SEEK_SET);
    while ((fread(cal, sizeof(Calzados), 1, archivo)) == 1){
        if (busca_numero == cal->orden){
            numero_encontrado = 1;
            if (!primera_impresion)
            {
                printf("Orden  Vendedor      Fecha     Categoria     Cantidad  Precio Unitario  Descuento   Subtotal     I.V.A       Total      Activo\n");
                printf("-------------------------------------------------------------------------------------------------------------------------------\n");
                primera_impresion = 1;
            }
            printf("%-6d %-12s %02d/%02d/%02d \t%s %13d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal->orden, cal->vendedor, cal->dia, cal->mes, cal->anio, cal->categoria, cal->cantidad, cal->precio, cal->descuento, cal->sub_total, cal->iva, cal->total, cal->activo);
            
            fclose(archivo);
            break;
        }
    }
    if (!numero_encontrado){
        printf("El numero %d no se ha encontrado.\n", busca_numero);
        fclose(archivo);
    }
}

void busca_venta_por_nombre(FILE *archivo, Calzados *cal){
    char busca_nombre[20];
    printf("Ingrese el nombre del vendedor que desea buscar\n");
    scanf("%s", busca_nombre);
    limpiarBuffer();
    validarSoloLetras(busca_nombre); // Acá, además de validar que se esté ingresando solo letras, tambíen vuelvo la 1ra letra mayúscula y el resto minuscula
    // Logrando que la comparacion en strcmp funcione independiente de como escrita la palabra. EJ: daNtE --> Dante

    int nombre_encontrado = 0;
    int primera_impresion = 0;
    fseek(archivo, 0, SEEK_SET);
    while ((fread(cal, sizeof(Calzados), 1, archivo)) == 1){
        if (strcmp(busca_nombre, cal->vendedor) == 0){
            nombre_encontrado = 1;
            if (!primera_impresion)
            {
                printf("Orden  Vendedor      Fecha     Categoria     Cantidad  Precio Unitario  Descuento   Subtotal     I.V.A       Total      Activo\n");
                printf("-------------------------------------------------------------------------------------------------------------------------------\n");
                primera_impresion = 1;
            }
            printf("%-6d %-12s %02d/%02d/%02d \t%s %13d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal->orden, cal->vendedor, cal->dia, cal->mes, cal->anio, cal->categoria, cal->cantidad, cal->precio, cal->descuento, cal->sub_total, cal->iva, cal->total, cal->activo);
            
            fclose(archivo);
            return;
        }
    }
    if (!nombre_encontrado){
        printf("El nombre %s no se ha encontrado.\n", busca_nombre);
        fclose(archivo);
    }
}

void buscarProducto(FILE *archivo)
{
    Calzados cal;
    archivo = fopen("registro.dat", "rb");

    if (archivo == NULL) {
    printf("No se pudo abrir el archivo.\n");
    return;
    }

    // Valido que haya productos ingresados
    fseek(archivo, 0, SEEK_END);
    int cantidad_datos = ftell(archivo) / sizeof(Calzados);
    if (cantidad_datos == 0){
        printf("No hay productos ingresados.\n");
        fclose(archivo);
        return;
    }

    char busca_venta;
    do {
        printf("\nIndique como desea buscar:\n");
        printf("c) Buscar por número de orden.\n");
        printf("d) Buscar por nombre del vendedor.\n");
        printf("Ingrese una opcion (c/d): ");
        scanf(" %c", &busca_venta);

        if ((busca_venta != 'c' && busca_venta != 'C') &&
            (busca_venta != 'd' && busca_venta != 'D'))
            printf("Opcion invalida. Ingrese c o d.\n");

    } while ((busca_venta != 'c' && busca_venta != 'C') && (busca_venta != 'd' && busca_venta != 'D'));

    if ((busca_venta == 'c' || busca_venta == 'C')) busca_venta_por_numero(archivo, &cal);
    if ((busca_venta == 'd' || busca_venta == 'D')) busca_venta_por_nombre(archivo, &cal);
}

// 6 (MODIFICAR)
void modificarProducto(FILE *archivo)
{
    Calzados cal;
    int cantidad_nueva;
    archivo = fopen("registro.dat", "r+b");

    if (archivo == NULL)
    {
        printf("No se ha podido abrir el archivo.\n");
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
        printf("La orden %d no se ha encontrado.\n", orden);
        fclose(archivo);
        return;
    }

    printf("Modificacion realizada.\n");
    fclose(archivo);
}

// 7 (BAJA LOGICA)
void bajaLogica(FILE *archivo)
{
    Calzados cal;
    archivo = fopen("registro.dat", "r+b");
    if (archivo == NULL)
    {
        printf("No se ha podidio abir el archivo.\n");
        return;
    }

    // Validar que haya productos
    fseek(archivo, 0, SEEK_END);
    int cantidad_datos = ftell(archivo) / sizeof(Calzados);
    if (cantidad_datos == 0)
    {
        printf("No se han ingresado productos.\n");
        fclose(archivo);
        return;
    }

    // Validar que haya productos para dar de baja
    int activo_encontado = 0;
    fseek(archivo, 0, SEEK_SET);
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
            printf("Orden  Vendedor      Fecha     Categoria     Cantidad  Precio Unitario  Descuento   Subtotal     I.V.A       Total      Activo\n");
            printf("-------------------------------------------------------------------------------------------------------------------------------\n");
            printf("%-6d %-12s %02d/%02d/%02d \t%s %13d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal.orden, cal.vendedor, cal.dia, cal.mes, cal.anio, cal.categoria, cal.cantidad, cal.precio, cal.descuento, cal.sub_total, cal.iva, cal.total, cal.activo);
            // Validacion de querer darlo de baja
            printf("Estas seguro de querer darlo de baja? (1 para continuar): ");
            scanf("%d", &validacion);

            if (validacion == 1)
            {
                // Colocacion y reescritura del producto dado de baja
                cal.activo = 0;
                fseek(archivo, -sizeof(Calzados), SEEK_CUR);
                fwrite(&cal, sizeof(Calzados), 1, archivo);
                printf("El producto ha sido colocado como inactivo\n");

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
                            printf("Orden  Vendedor      Fecha     Categoria     Cantidad  Precio Unitario  Descuento   Subtotal     I.V.A       Total      Activo\n");
                            printf("-------------------------------------------------------------------------------------------------------------------------------\n");
                            primera_impresion = 1;
                        }
                        printf("%-6d %-12s %02d/%02d/%02d \t%s %13d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal.orden, cal.vendedor, cal.dia, cal.mes, cal.anio, cal.categoria, cal.cantidad, cal.precio, cal.descuento, cal.sub_total, cal.iva, cal.total, cal.activo);
                    }
                }

                // En caso de que no haya quedado al menos un producto activo
                if (!activo_encontado)
                {
                    printf("No quedaron productos activos.\n");
                    fclose(archivo);
                    return;
                }
                else
                {
                    printf("Fin del listado.\n");
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

    // Validar que haya productos
    fseek(archivo, 0, SEEK_END);
    int cantidad_datos = ftell(archivo) / sizeof(Calzados);
    if (cantidad_datos == 0)
    {
        printf("No se han ingresado productos.\n");
        fclose(archivo);
        return;
    }

    FILE *temp;
    temp = fopen("temporal.dat", "w+b");
    if (temp == NULL)
    {
        printf("El archivo temporal no se pudo abrir.\n");
        fclose(archivo);
        return;
    }

    // Creacion del archivo con la fecha actual (Ver si lo puedo optimizar)
    char nombre_archivo[20];
    obtenerNombreArchivo(nombre_archivo, sizeof(nombre_archivo));
    FILE *archivo_txt;
    archivo_txt = fopen(nombre_archivo, "at");

    fseek(archivo, 0, SEEK_SET);
    fseek(archivo_txt, 0, SEEK_SET);

    int encontrado = 0;
    static int primera_impresion = 0; // Colocado como static para eivtar la impresión de nuevo del encabezado (si se cierra el programa, este valor se va)
    while ((fread(&cal, sizeof(Calzados), 1, archivo)) == 1)
    {
        if (cal.activo == 0 && cal.orden != 0)
        {
            encontrado = 1;
            if (!primera_impresion)
            {
                fprintf(archivo_txt, "Orden  Vendedor      Fecha     Categoria     Cantidad  Precio Unitario  Descuento   Subtotal     I.V.A       Total      Activo\n");
                fprintf(archivo_txt, "-------------------------------------------------------------------------------------------------------------------------------\n");
                primera_impresion = 1;
            }
            fprintf(archivo_txt, "%-6d %-12s %02d/%02d/%02d \t%s %13d %16.2f %11.2f %12.2f %11.2f %10.2f %8d\n", cal.orden, cal.vendedor, cal.dia, cal.mes, cal.anio, cal.categoria, cal.cantidad, cal.precio, cal.descuento, cal.sub_total, cal.iva, cal.total, cal.activo);

            Calzados temporal = {0}; // Creo 'temporal' para almacenar los datos en el archivo 'temp'
            fwrite(&temporal, sizeof(Calzados), 1, temp);
        }
        else
        {
            fwrite(&cal, sizeof(Calzados), 1, temp);
        }
    }

    if (!encontrado)
    {
        printf("No se han encontrado productos inactivos\n");
        printf("El archivo de texto no ha sido creado.\n");
        fclose(archivo);
        fclose(archivo_txt);
        fclose(temp);
        remove("temporal.dat");
        return;
    }

    fclose(archivo);
    fclose(temp);
    fclose(archivo_txt);
    remove("registro.dat");
    rename("temporal.dat", "registro.dat");
    printf("El archivo de texto con los productos inactivos ha sido creado.\n");
}

// 9 (LISTAR TEXTO)
void listar_texto(FILE *archivo){
    char nombre_archivo[30];
    obtenerNombreArchivo(nombre_archivo, sizeof(nombre_archivo));
    archivo = fopen(nombre_archivo, "rt");

    if (archivo == NULL) {
    printf("No se pudo abrir el archivo.\n");
    return;
    }

    if (fgetc(archivo) == EOF){
        printf("No hay productos que se hayan dado de baja.\n");
        fclose(archivo);
        return;
    }
    rewind(archivo);

    printf("Listado de todas las bajas:\n");
    char linea[200];
    while (fgets(linea, sizeof(linea), archivo)) {
        printf("%s", linea);
    }

    fclose(archivo);
    return;
}

void realizarOperacion(int eleccion, FILE *archivo)
{
    switch (eleccion)
        {
        case 1:
            crearBinario(archivo);
            break;
        case 2:
            altaProducto(archivo);
            break;
        case 3:
            listarProdutos(archivo);
            break;
        case 4:
            buscarProducto(archivo);
            break;
        case 5:
            modificarProducto(archivo);
            break;
        case 6:
            bajaLogica(archivo);
            break;
        case 7:
            bajaFisica(archivo);
            break;
        case 8:
            listar_texto(archivo);
            break;
        default:
            break;
        }
}
