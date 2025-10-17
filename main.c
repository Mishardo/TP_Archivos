#include <stdio.h>
#include <ctype.h>

//FUNCIONES (Despues las pasamos a un archivo a parte donde estáran todas las funciones)--------------------------
//1 (MENU)

//2 (CREAR ARCHIVO BINARIO)
void crearBinario (FILE * archivo)
{
    if ( (archivo = fopen("registro.dat", "w+b")) == NULL )
    {
        printf("El archivo no pudo ser creado");
    }
    else
    {
        printf("Archivo binario creado con exito");
        fclose(archivo);
    }
}

//3 (ALTA)

//4 (LISTAR BINARIO)

//5 (BUSCAR)

//6 (MODIFICAR)

//7 (BAJA LOGICA)

//8 (BAJA FISICA)

//9 (LISTAR TEXTO)

//-------------------------------------------------------------------------------------


int main()
{
    //MENU


    return 0;
}
