/*
Equipo 9:
Gerardo Josue Rubio Calderon
Argenis Adan Gutierrez Hurtado

Actividad: Ordenamiento Iterativo y Recursivos con OpenMP

Algoritmos utilizados:
Para el Iterativo: Bubble Sort
Para el recursivo: QuickSort

*/


#include <stdio.h>
#include <omp.h>
#include <random>
#include <time.h>
#include <utility>

using namespace std;



void copiarArreglo(int* origen, int* destino, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        destino[i] = origen[i];
    }
}

void imprimirArreglo(int* arr, int n) {
    if (n <= 100) {
        #pragma omp single
        {
            for (int i = 0; i < n; i++) {
                printf("%d", arr[i]);
                if (i < n - 1) {
                    printf(", ");
                }
            }
            printf("\n");
        }
    } else {
        printf("[Arreglo demasiado gigante para mostrar en la consola]\n");
    }
}

//Primera parte osi osi (Bubble Sort)

void bubbleSortSecuencial(int* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }

}

void bubbleSortParalelo(int* arr, int n) {
    bool isSorted = false;
    while (!isSorted) {
        isSorted = true;

        #pragma omp parallel default(none) shared (arr, n, isSorted)
        {
            bool local_sorted = true;

            #pragma omp for
            for (int i = 0; i <= n - 2; i += 2) {
                if (arr[i] > arr[i + 1]) {
                    swap(arr[i], arr[i + 1]);
                    local_sorted = false;
                }
            }

            if (!local_sorted) {
                #pragma omp atomic write
                isSorted = false;
            }
            #pragma omp barrier

            //Fase Impar
            #pragma omp for
            for (int i = 1; i <= n - 2; i += 2) {
                if (arr[i] > arr[i + 1]) {
                    swap(arr[i], arr[i + 1]);
                    local_sorted = false;
                }
            }

            if (!local_sorted) {
                #pragma omp atomic write
                isSorted = false;
            }
        }
    }

}

// Algoritmo Recursivo (Quick Sort)

int particion (int* arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSortSecuencial (int* arr, int low, int high) {
    if (low < high) {
        int pi = particion(arr, low, high);
        quickSortSecuencial(arr, low, pi - 1);
        quickSortSecuencial(arr, pi + 1, high);
    }
}

void quickSortParaleloHelper(int* arr, int low, int high) {
    if (low < high) {
        int pi = particion(arr, low, high);

        if (high - low < 1000) {
            quickSortSecuencial(arr, low, pi - 1);
            quickSortSecuencial(arr, pi + 1, high);
        } else {
            #pragma omp task shared(arr)
            quickSortParaleloHelper(arr, low, pi - 1);

            #pragma omp task shared(arr)
            quickSortParaleloHelper(arr, pi + 1, high);

            #pragma omp taskwait
        }
    }

}


void quickSortParalelo(int* arr, int n) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            quickSortParaleloHelper(arr, 0, n - 1);
        }
    }

}



int main() {
    int* arrOriginal = nullptr;
    int* arrTrabajo = nullptr;
    int n = 0;
    int limiteAleatorio = 0;
    int opcion;

    double tiempo_seq_burbuja = 0, tiempo_par_burbuja = 0;
    double tiempo_seq_quick = 0, tiempo_par_quick = 0;

    omp_set_num_threads(omp_get_max_threads());

    do {
        printf("\n=== MENU DE ORDENAMIENTO OMP ===\n");
        printf("1. Llenar arreglo peque (100 elementos - Imprime consola)\n");
        printf("2. Llenar arreglo intermedio (50,000 elementos - IDEAL PARA BURBUJA)\n");
        printf("3. Llenar arreglo masivo (10,000,000 elementos - SOLO QUICK SORT)\n");
        printf("4. Burbuja Secuencial\n");
        printf("5. Burbuja Paralela (Par-Impar)\n");
        printf("6. Quick Sort Secuencial\n");
        printf("7. Quick Sort Paralelo (Tasks)\n");
        printf("8. Mostrar Speedups / Comparativa\n");
        printf("9. Salir del programa\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
            case 2:
            case 3: {
                if (arrOriginal != nullptr) {
                    delete[] arrOriginal;
                    delete[] arrTrabajo;
                }

                if (opcion == 1) n = 100;
                else if (opcion == 2) n = 50000;
                else n = 10000000;

                limiteAleatorio = (opcion == 1) ? 200 : 2000000;

                arrOriginal = new int[n];
                arrTrabajo = new int[n];

                double inicio = omp_get_wtime();

                #pragma omp parallel
                {
                    int id_hilo = omp_get_thread_num();
                    std::mt1993
                    7 gen(time(NULL) ^ id_hilo);
                    std::uniform_int_distribution<int> dist(0, limiteAleatorio);

                    #pragma omp for
                    for (int i = 0; i < n; i++) {
                        arrOriginal[i] = dist(gen);
                    }
                }

                double fin = omp_get_wtime();
                printf("\n-> Arreglo de %d elementos creado en %f segundos.\n", n, fin - inicio);
                if (n == 100) {
                    printf("Arreglo original desordenado: \n");
                    imprimirArreglo(arrOriginal, n);
                }
                tiempo_seq_burbuja = tiempo_par_burbuja = tiempo_seq_quick = tiempo_par_quick = 0;
                break;
            }
            case 4:
            case 5: {
                if (!arrOriginal) { printf("Llene el arreglo primero.\n"); break; }
                if (n > 50000) {
                    printf("¡ALERTA! La Burbuja tardaria dias con %d elementos. Usa la opcion 2 para evaluar Burbuja.\n", n);
                    break;
                }
                copiarArreglo(arrOriginal, arrTrabajo, n);
                double inicio = omp_get_wtime();

                if (opcion == 4) {
                    bubbleSortSecuencial(arrTrabajo, n);
                    tiempo_seq_burbuja = omp_get_wtime() - inicio;
                    printf("\n-> Burbuja Secuencial finalizada en %f s.\n", tiempo_seq_burbuja);
                } else {
                    bubbleSortParalelo(arrTrabajo, n);
                    tiempo_par_burbuja = omp_get_wtime() - inicio;
                    printf("\n-> Burbuja Paralela finalizada en %f s.\n", tiempo_par_burbuja);
                }

                if (n == 100) imprimirArreglo(arrTrabajo, n);
                break;
            }
            case 6:
            case 7: {
                if (!arrOriginal) { printf("Llene el arreglo primero.\n"); break; }
                copiarArreglo(arrOriginal, arrTrabajo, n);
                double inicio = omp_get_wtime();

                if (opcion == 6) {
                    quickSortSecuencial(arrTrabajo, 0, n - 1);
                    tiempo_seq_quick = omp_get_wtime() - inicio;
                    printf("\n-> Quick Sort Secuencial finalizado en %f s.\n", tiempo_seq_quick);
                } else {
                    quickSortParalelo(arrTrabajo, n);
                    tiempo_par_quick = omp_get_wtime() - inicio;
                    printf("\n-> Quick Sort Paralelo finalizado en %f s.\n", tiempo_par_quick);
                }

                if (n == 100) imprimirArreglo(arrTrabajo, n);
                break;
            }
            case 8: {
                printf("\n=== COMPARATIVA DE RENDIMIENTO ===\n");
                if (tiempo_seq_burbuja > 0 && tiempo_par_burbuja > 0) {
                    printf("Speedup Burbuja (Arr: %d): %fx\n", n, (tiempo_seq_burbuja / tiempo_par_burbuja));
                } else {
                    printf("No hay datos de Burbuja para comparar.\n");
                }

                if (tiempo_seq_quick > 0 && tiempo_par_quick > 0) {
                    printf("Speedup Quick Sort (Arr: %d): %fx\n", n, (tiempo_seq_quick / tiempo_par_quick));
                } else {
                    printf("No hay datos de Quick Sort para comparar.\n");
                }
                break;
            }
            case 9:
                printf("Saliendo y liberando memoria...\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 9);

    if (arrOriginal != nullptr) {
        delete[] arrOriginal;
        delete[] arrTrabajo;
    }

    return 0;
}
