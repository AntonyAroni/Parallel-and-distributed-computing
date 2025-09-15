//# Compilación estándar
//g++ -O2 -g -std=c++20 matrix_mult_optimized.cpp -o matrix_mult



// # Compilar
// g++ -O2 -g -std=c++20 4_analisis.cpp -o analisis
//
// # Ejecutar normalmente
// ./analisis
//
// # Ejecutar con análisis de caché
// valgrind --tool=cachegrind ./analisis
//
// # Visualizar resultados
// kcachegrind cachegrind.out.*

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>
#include <numeric>
#include <fstream>

using namespace std;
using namespace std::chrono;
using real = double;

// Índice en arreglo aplanado row-major
inline size_t idx(size_t i, size_t j, size_t N) { return i * N + j; }

// Multiplicación clásica C = A * B (orden ijk)
void matmul_classic(const vector<real>& A, const vector<real>& B,
                   vector<real>& C, size_t N) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            real sum = 0.0;
            for (size_t k = 0; k < N; ++k) {
                sum += A[idx(i,k,N)] * B[idx(k,j,N)];
            }
            C[idx(i,j,N)] = sum;
        }
    }
}

// Multiplicación por bloques optimizada
void matmul_blocked(const vector<real>& A, const vector<real>& B,
                   vector<real>& C, size_t N, size_t block_size) {
    fill(C.begin(), C.end(), 0.0);

    for (size_t ii = 0; ii < N; ii += block_size) {
        for (size_t jj = 0; jj < N; jj += block_size) {
            for (size_t kk = 0; kk < N; kk += block_size) {
                // Límites del bloque
                size_t i_end = min(ii + block_size, N);
                size_t j_end = min(jj + block_size, N);
                size_t k_end = min(kk + block_size, N);

                // Multiplicación del bloque
                for (size_t i = ii; i < i_end; ++i) {
                    for (size_t j = jj; j < j_end; ++j) {
                        real sum = C[idx(i,j,N)];
                        for (size_t k = kk; k < k_end; ++k) {
                            sum += A[idx(i,k,N)] * B[idx(k,j,N)];
                        }
                        C[idx(i,j,N)] = sum;
                    }
                }
            }
        }
    }
}

// Estructura para almacenar resultados de benchmark
struct BenchResult {
    string method;
    size_t block_size;
    double avg_time;
    double speedup;
};

// Función para medir tiempo de ejecución
double benchmark_algorithm(function<void()> algo, int repeats = 3) {
    double total_time = 0.0;
    for (int i = 0; i < repeats; ++i) {
        auto start = high_resolution_clock::now();
        algo();
        auto end = high_resolution_clock::now();
        total_time += duration_cast<duration<double>>(end - start).count();
    }
    return total_time / repeats;
}

// Inicializar matrices con valores aleatorios
void init_matrices(vector<real>& A, vector<real>& B, size_t N, mt19937_64& rng) {
    uniform_real_distribution<real> dist(0.0, 1.0);
    for (size_t i = 0; i < N * N; ++i) {
        A[i] = dist(rng);
        B[i] = dist(rng);
    }
}

// Función para generar reporte detallado de profiling
void generate_profiling_report(const string& algorithm, size_t N, size_t block_size = 0) {
    string filename = "profile_report_" + algorithm + "_N" + to_string(N);
    if (block_size > 0) {
        filename += "_B" + to_string(block_size);
    }
    filename += ".txt";

    ofstream report(filename);
    report << "=== REPORTE DE PROFILING ===" << endl;
    report << "Algoritmo: " << algorithm << endl;
    report << "Tamaño matriz: " << N << "x" << N << endl;
    if (block_size > 0) {
        report << "Tamaño bloque: " << block_size << endl;
    }
    report << "Fecha: " << __DATE__ << " " << __TIME__ << endl;
    report << endl;
    report << "Para obtener métricas detalladas ejecutar:" << endl;
    report << "valgrind --tool=cachegrind --cache-sim=yes ./" << endl;
    report << "kcachegrind cachegrind.out.*" << endl;
    report.close();

    cout << "Reporte generado: " << filename << endl;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Tamaños reducidos para profiling detallado
    const vector<size_t> sizes = {256, 512};
    const vector<size_t> block_sizes = {16, 32, 64};
    const int repeats = 1; // Reducido para profiling

    mt19937_64 rng(123456);

    cout << "=== ANÁLISIS CON VALGRIND/KCACHEGRIND ===\n";
    cout << "Ejecutar con: valgrind --tool=callgrind --cache-sim=yes ./matrix_mult\n\n";
    cout << fixed << setprecision(3);
    cout << setw(6) << "N" << setw(12) << "Método" << setw(8) << "Bloque"
         << setw(10) << "Tiempo(s)" << setw(15) << "Profiling" << "\n";
    cout << string(55, '-') << "\n";

    for (size_t N : sizes) {
        vector<real> A(N*N), B(N*N), C(N*N);
        vector<BenchResult> results;

        init_matrices(A, B, N, rng);

        cout << "Ejecutando profiling para N=" << N << "..." << endl;

        // Profiling clásico
        double classic_time = benchmark_algorithm([&]() {
            fill(C.begin(), C.end(), 0.0);
            matmul_classic(A, B, C, N);
        }, repeats);

        generate_profiling_report("clasico", N);
        results.push_back({"Clásico", 0, classic_time, 1.0});

        // Profiling bloques - solo el mejor tamaño (16)
        size_t best_block = 16;

        double blocked_time = benchmark_algorithm([&]() {
            matmul_blocked(A, B, C, N, best_block);
        }, repeats);

        double speedup = classic_time / blocked_time;
        generate_profiling_report("bloques", N, best_block);
        results.push_back({"Bloques", best_block, blocked_time, speedup});

        // Mostrar resultados
        for (const auto& result : results) {
            cout << setw(6) << N << setw(12) << result.method;
            if (result.block_size > 0) {
                cout << setw(8) << result.block_size;
            } else {
                cout << setw(8) << "-";
            }
            cout << setw(10) << result.avg_time << setw(15) << "✓" << "\n";
        }
        cout << string(55, '-') << "\n";
    }

    cout << "\n=== INSTRUCCIONES DE ANÁLISIS ===\n";
    cout << "1. Ejecutar: valgrind --tool=cachegrind ./analisis > cache_report.txt\n";
    cout << "2. Visualizar: kcachegrind cachegrind.out.*\n";
    cout << "3. Para callgrind: valgrind --tool=callgrind ./analisis\n";
    cout << "4. Visualizar: kcachegrind callgrind.out.*\n";
    cout << "\nArchivos de reporte generados en el directorio actual.\n";

    return 0;
}