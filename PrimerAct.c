#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Definición de la estructura del equipo
typedef struct {
    char *nombre;
    char *ciudad;
    int añoFundacion;
    int puntaje; // Nuevo campo para almacenar el puntaje del equipo
} Equipo;

// Estructura para representar un enfrentamiento
typedef struct {
    char *equipoLocal;
    char *equipoVisitante;
    int semana;
    struct {
        int golesLocal;
        int golesVisitante;
    } marcador;
} Enfrentamiento;

// Función para capturar el número de equipos
int capturarNumeroEquipos() {
    int numeroEquipos;
    printf("¿Cuántos equipos deseas ingresar? ");
    scanf("%d", &numeroEquipos);
    getchar(); // Consumir el carácter de nueva línea
    return numeroEquipos;
}

// Función para capturar los datos del equipo
Equipo* capturarDatosEquipo() {
    Equipo *equipo = (Equipo*)malloc(sizeof(Equipo));
    if (equipo == NULL) {
        printf("Error: No se pudo asignar memoria para el equipo.\n");
        return NULL;
    }

    printf("Por favor ingrese los siguientes datos del equipo:\n");
    
    // Captura del nombre del equipo
    printf("Nombre del equipo: ");
    equipo->nombre = (char*)malloc(100 * sizeof(char)); // Supongamos un máximo de 100 caracteres
    if (equipo->nombre == NULL) {
        printf("Error: No se pudo asignar memoria para el nombre del equipo.\n");
        free(equipo);
        return NULL;
    }
    fgets(equipo->nombre, 100, stdin);
    equipo->nombre[strcspn(equipo->nombre, "\n")] = 0; // Eliminar el salto de línea

    // Captura de la ciudad del equipo
    printf("Ciudad del equipo: ");
    equipo->ciudad = (char*)malloc(100 * sizeof(char)); // Supongamos un máximo de 100 caracteres
    if (equipo->ciudad == NULL) {
        printf("Error: No se pudo asignar memoria para la ciudad del equipo.\n");
        free(equipo->nombre);
        free(equipo);
        return NULL;
    }
    fgets(equipo->ciudad, 100, stdin);
    equipo->ciudad[strcspn(equipo->ciudad, "\n")] = 0; // Eliminar el salto de línea

    // Captura del año de fundación del equipo
    printf("Año de fundacion del equipo: ");
    scanf("%d", &equipo->añoFundacion);
    getchar(); // Consumir el carácter de nueva línea
    
    return equipo;
}

// Función para almacenar el equipo en un archivo
void almacenarEquipoEnArchivo(Equipo *equipo) {
    FILE *archivo = fopen("datos_equipos.txt", "a"); // Abrir en modo append
    
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo para escritura.\n");
        return;
    }

    fprintf(archivo, "Nombre: %s, Ciudad: %s, Año de fundación: %d\n", equipo->nombre, equipo->ciudad, equipo->añoFundacion);

    if (fclose(archivo) != 0) {
        printf("Error: No se pudo cerrar correctamente el archivo.\n");
    }
}

// Función para leer los equipos desde el archivo existente
Equipo** leerEquiposDesdeArchivo(const char* nombreArchivo, int* numEquipos) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo para lectura.\n");
        return NULL;
    }

    // Contar el número de equipos en el archivo
    int numEquiposArchivo = 0;
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        numEquiposArchivo++;
    }

    // Volver al principio del archivo
    rewind(archivo);

    // Crear un array de estructuras de Equipo
    Equipo** equipos = (Equipo**)malloc(numEquiposArchivo * sizeof(Equipo*));
    if (equipos == NULL) {
        printf("Error: No se pudo asignar memoria para los equipos.\n");
        fclose(archivo);
        return NULL;
    }

    // Leer los equipos desde el archivo y almacenarlos en la matriz
    for (int i = 0; i < numEquiposArchivo; i++) {
        equipos[i] = (Equipo*)malloc(sizeof(Equipo));
        if (equipos[i] == NULL) {
            printf("Error: No se pudo asignar memoria para el equipo.\n");
            fclose(archivo);
            return NULL;
        }
        equipos[i]->nombre = (char*)malloc(100 * sizeof(char));
        equipos[i]->ciudad = (char*)malloc(100 * sizeof(char));
        fscanf(archivo, "Nombre: %99[^,], Ciudad: %99[^,], Año de fundación: %d\n",
               equipos[i]->nombre, equipos[i]->ciudad, &equipos[i]->añoFundacion);
        equipos[i]->puntaje = 0; // Inicializar puntaje en 0
    }

    // Cerrar el archivo
    fclose(archivo);

    *numEquipos = numEquiposArchivo;
    return equipos;
}

// Función para generar el calendario de enfrentamientos
Enfrentamiento* generarCalendario(Equipo** equipos, int numEquipos) {
    // Semilla para el generador de números aleatorios
    srand(time(NULL));

    // Crear un array de estructuras de Enfrentamiento
    int numEnfrentamientos = numEquipos * (numEquipos - 1) / 2; // Todos contra todos
    Enfrentamiento* calendario = (Enfrentamiento*)malloc(numEnfrentamientos * sizeof(Enfrentamiento));
    if (calendario == NULL) {
        printf("Error: No se pudo asignar memoria para el calendario de enfrentamientos.\n");
        return NULL;
    }

    // Generar calendario de enfrentamientos aleatorios
    int index = 0;
    for (int semana = 1; semana <= numEquipos - 1; semana++) {
        for (int i = 0; i < numEquipos; i++) {
            for (int j = i + 1; j < numEquipos; j++) {
                if (j - i == semana) {
                    calendario[index].equipoLocal = strdup(equipos[i]->nombre);
                    calendario[index].equipoVisitante = strdup(equipos[j]->nombre);
                    calendario[index].semana = semana;
                    calendario[index].marcador.golesLocal = rand() % 6; // Generar goles aleatorios para el equipo local (máximo 5)
                    calendario[index].marcador.golesVisitante = rand() % 6; // Generar goles aleatorios para el equipo visitante (máximo 5)
                    index++;
                }
            }
        }
    }

    return calendario;
}

// Función para escribir el calendario en un nuevo archivo
void escribirCalendarioEnArchivo(const char* nombreArchivo, Enfrentamiento* calendario, int numEnfrentamientos) {
    FILE *archivo = fopen(nombreArchivo, "w"); // Abrir en modo escritura (se sobrescribe si existe)

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo para escritura.\n");
        return;
    }

    // Escribir el calendario de enfrentamientos en el archivo
    int semanaActual = -1;
    for (int i = 0; i < numEnfrentamientos; i++) {
        if (calendario[i].semana != semanaActual) {
            semanaActual = calendario[i].semana;
            fprintf(archivo, "Semana %d\n", semanaActual);
        }
        fprintf(archivo, "Equipo local: %s, Equipo visitante: %s, Goles local: %d, Goles visitante: %d\n",
                calendario[i].equipoLocal, calendario[i].equipoVisitante,
                calendario[i].marcador.golesLocal, calendario[i].marcador.golesVisitante);
    }

    // Cerrar el archivo
    fclose(archivo);
}

// Función para calcular los puntajes de los equipos
void calcularPuntajes(Enfrentamiento* calendario, int numEnfrentamientos, Equipo** equipos, int numEquipos) {
    for (int i = 0; i < numEquipos; i++) {
        equipos[i]->puntaje = 0; // Inicializar puntaje en 0
    }

    for (int i = 0; i < numEnfrentamientos; i++) {
        for (int j = 0; j < numEquipos; j++) {
            if (strcmp(calendario[i].equipoLocal, equipos[j]->nombre) == 0) {
                equipos[j]->puntaje += calendario[i].marcador.golesLocal; // Sumar goles de local
            }
            if (strcmp(calendario[i].equipoVisitante, equipos[j]->nombre) == 0) {
                equipos[j]->puntaje += calendario[i].marcador.golesVisitante * 2; // Sumar goles de visitante (x2)
            }
        }
    }
}

// Función para escribir los puntajes de los equipos en un archivo
void escribirPuntajesEnArchivo(const char* nombreArchivo, Equipo** equipos, int numEquipos) {
    FILE *archivo = fopen(nombreArchivo, "w"); // Abrir en modo escritura (se sobrescribe si existe)

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo para escribir los puntajes de los equipos.\n");
        return;
    }

    for (int i = 0; i < numEquipos; i++) {
        fprintf(archivo, "%s: %d puntos\n", equipos[i]->nombre, equipos[i]->puntaje);
    }

    fclose(archivo);
}

// Función para determinar los ocho mejores equipos del torneo
void determinarOchoMejoresEquipos(Equipo** equipos, int numEquipos, Equipo** mejorEquipo) {
    // Ordenar los equipos por puntaje (de mayor a menor)
    for (int i = 0; i < numEquipos - 1; i++) {
        for (int j = 0; j < numEquipos - i - 1; j++) {
            if (equipos[j]->puntaje < equipos[j+1]->puntaje) {
                Equipo* temp = equipos[j];
                equipos[j] = equipos[j+1];
                equipos[j+1] = temp;
            }
        }
    }

    // Copiar los ocho mejores equipos en mejorEquipo
    for (int i = 0; i < 8; i++) {
        mejorEquipo[i] = equipos[i];
    }
}

// Función para determinar el ganador del torneo entre los ocho mejores equipos
void determinarGanadorTorneo(Equipo** mejoresEquipos, int numMejoresEquipos) {
    // Encontrar el equipo con el puntaje más alto entre los ocho finalistas
    Equipo* ganador = mejoresEquipos[0];
    for (int i = 1; i < numMejoresEquipos; i++) {
        if (mejoresEquipos[i]->puntaje > ganador->puntaje) {
            ganador = mejoresEquipos[i];
        }
    }

    printf("El ganador del torneo es: %s con %d puntos.\n", ganador->nombre, ganador->puntaje);
}

// Función para escribir los ocho mejores equipos en un archivo
void escribirOchoMejoresEquiposEnArchivo(const char* nombreArchivo, Equipo** mejoresEquipos, int numMejoresEquipos) {
    FILE *archivo = fopen(nombreArchivo, "w"); // Abrir en modo escritura (se sobrescribe si existe)

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo para escribir los ocho mejores equipos.\n");
        return;
    }

    fprintf(archivo, "Los ocho mejores equipos del torneo son:\n");
    for (int i = 0; i < numMejoresEquipos; i++) {
        fprintf(archivo, "%s: %d puntos\n", mejoresEquipos[i]->nombre, mejoresEquipos[i]->puntaje);
    }

    fclose(archivo);
}

int main() {
    // Capturar el número de equipos
    int numeroEquipos = capturarNumeroEquipos();

    for (int i = 0; i < numeroEquipos; i++) {
        // Capturar datos del equipo
        Equipo *equipo = capturarDatosEquipo();

        if (equipo != NULL) {
            // Almacenar datos en el archivo
            almacenarEquipoEnArchivo(equipo);
            printf("Datos del equipo almacenados correctamente.\n");

            // Liberar memoria asignada
            free(equipo->nombre);
            free(equipo->ciudad);
            free(equipo);
        } else {
            printf("Error al capturar los datos del equipo.\n");
        }
    }

    // Leer los equipos desde el archivo
    int numEquipos;
    Equipo** equipos = leerEquiposDesdeArchivo("datos_equipos.txt", &numEquipos);
    if (equipos == NULL) {
        printf("Error: No se pudieron leer los equipos desde el archivo.\n");
        return 1;
    }

    // Generar calendario de enfrentamientos
    Enfrentamiento* calendario = generarCalendario(equipos, numEquipos);
    if (calendario == NULL) {
        printf("Error: No se pudo generar el calendario de enfrentamientos.\n");
        return 1;
    }

    // Escribir el calendario en un nuevo archivo
    escribirCalendarioEnArchivo("calendario_enfrentamientos.txt", calendario, numEquipos * (numEquipos - 1) / 2);

    // Calcular puntajes de los equipos
    calcularPuntajes(calendario, numEquipos * (numEquipos - 1) / 2, equipos, numEquipos);

    // Escribir puntajes de los equipos en un archivo
    escribirPuntajesEnArchivo("puntajes_equipos.txt", equipos, numEquipos);

    // Determinar los ocho mejores equipos del torneo
    Equipo* mejoresEquipos[8];
    determinarOchoMejoresEquipos(equipos, numEquipos, mejoresEquipos);

    // Escribir los ocho mejores equipos en un archivo
    escribirOchoMejoresEquiposEnArchivo("ocho_mejores_equipos.txt", mejoresEquipos, 8);

    // Determinar el ganador del torneo entre los ocho mejores equipos
    determinarGanadorTorneo(mejoresEquipos, 8);

    // Liberar memoria asignada
    for (int i = 0; i < numEquipos; i++) {
        free(equipos[i]->nombre);
        free(equipos[i]->ciudad);
        free(equipos[i]);
    }
    free(equipos);
    for (int i = 0; i < numEquipos * (numEquipos - 1) / 2; i++) {
        free(calendario[i].equipoLocal);
        free(calendario[i].equipoVisitante);
    }
    free(calendario);

    return 0;
}