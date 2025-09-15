# Análisis de Memoria Caché

Este directorio contiene experimentos sobre optimización de memoria caché en operaciones matriciales.

## Archivos

### 1. Bucles Anidados (`1_bucles_anidados.cpp`)
Comparación entre acceso row-major vs column-major en multiplicación matriz-vector:
- **Row-major**: Acceso secuencial a memoria (más eficiente)
- **Column-major**: Acceso no secuencial (menos eficiente por cache misses)

### 2. Multiplicación Clásica (`2_matriz_clasica.cpp`)
Implementación estándar de multiplicación de matrices con análisis de rendimiento para diferentes tamaños (100x100 hasta 1000x1000).

### 3. Comparación Bloques vs Clásica (`3_matriz_bloques_x_clasica.cpp`)
Análisis comparativo entre:
- **Algoritmo clásico**: Orden ijk estándar
- **Algoritmo por bloques**: División en sub-matrices para mejor localidad de caché

#### Resultados de Benchmarks
- Tamaños probados: 256x256, 512x512, 768x768
- Tamaños de bloque: 16, 32, 64
- Métrica: Speedup relativo al método clásico

### 4. Análisis de Profiling (`4_analisis.cpp`)
Herramientas de análisis de rendimiento y profiling de memoria.

## Archivos de Resultados
- `profile_report_*.txt`: Reportes de profiling con Cachegrind
- `cachegrind.out.*`: Archivos de salida de Valgrind

## Compilación y Ejecución

```bash
# Compilación optimizada
g++ -O3 -march=native -std=c++20 archivo.cpp -o ejecutable

# Profiling con Cachegrind
valgrind --tool=cachegrind ./ejecutable
```

## Conceptos Clave

- **Localidad temporal**: Reutilización de datos recientemente accedidos
- **Localidad espacial**: Acceso a datos contiguos en memoria
- **Cache blocking**: División en bloques que caben en caché L1/L2
- **Row-major order**: Almacenamiento por filas (C/C++)
- **Cache miss penalty**: Costo de acceder a memoria principal