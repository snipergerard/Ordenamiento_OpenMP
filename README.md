# Ordenamiento Iterativo y Recursivo con OpenMP

**Asignatura:** Programación Paralela
**Equipo:** 09

### Integrantes
* Gutiérrez Hurtado, Argenis Adán
* Rubio Calderón, Gerardo Josué

### Algoritmos Seleccionados
1. **Algoritmo Iterativo:** Bubble Sort
   * *Versión Paralela:* Implementación Par-Impar (Odd-Even Transposition).
2. **Algoritmo Recursivo:** Quick Sort
   * *Versión Paralela:* Implementación mediante sistema de tareas asíncronas.

### Directivas OpenMP Utilizadas
* `#pragma omp parallel`: Creación de la región paralela y despliegue de hilos.
* `#pragma omp for`: Distribución del llenado del arreglo y de las fases iterativas.
* `#pragma omp barrier`: Sincronización estricta entre la fase par e impar en la Burbuja.
* `#pragma omp atomic write`: Prevención de condiciones de carrera al actualizar la bandera de estado.
* `#pragma omp single nowait`: Asignación del arranque recursivo a un solo hilo sin detener al resto.
* `#pragma omp task`: Creación de tareas independientes para las sub-particiones del Quick Sort.
* `#pragma omp taskwait`: Sincronización jerárquica para garantizar la finalización de subtareas recursivas.

### Instrucciones de Compilación y Ejecución
Este código está diseñado en C++ empleando memoria dinámica y la librería `<random>`.

**Desde terminal (GCC/MinGW):**
1. Compilar habilitando OpenMP:
   ```bash
   g++ -fopenmp ordenamiento_omp.cpp -o ordenamiento
