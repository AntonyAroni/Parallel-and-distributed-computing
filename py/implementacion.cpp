#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>

// Configuración
constexpr long long NUM_TERMINOS = 10000000LL;
constexpr int NUM_HILOS = 4;
constexpr double PI_REAL = 3.14159265358979323846;

// ============================================
// ESTRUCTURAS
// ============================================

struct ThreadData {
    int id;
    long long n_terminos;
    double suma_local;
};

struct BusyWaitData {
    volatile long flag;
    double suma_global;
};

struct MutexData {
    pthread_mutex_t mutex;
    double suma_global;
};

// ============================================
// CLASE PARA TIMING
// ============================================
class Timer {
    std::chrono::high_resolution_clock::time_point start_;
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}

    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start_).count();
    }
};

// ============================================
// 1. SECUENCIAL (sin threads)
// ============================================
double calcular_pi_secuencial(long long n) {
    double suma = 0.0;
    double factor = 1.0;

    for (long long i = 0; i < n; i++) {
        suma += factor / (2 * i + 1);
        factor = -factor;
    }

    return 4.0 * suma;
}

// ============================================
// 2. BUSY-WAITING DENTRO DEL BUCLE
// ============================================
struct BusyWaitDentroArgs {
    ThreadData thread_data;
    BusyWaitData* shared_data;
};

void* thread_busy_waiting_dentro(void* arg) {
    BusyWaitDentroArgs* args = static_cast<BusyWaitDentroArgs*>(arg);
    int my_rank = args->thread_data.id;
    long long n = args->thread_data.n_terminos;
    BusyWaitData* shared = args->shared_data;

    long long my_n = n / NUM_HILOS;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    double factor = (my_first_i % 2 == 0) ? 1.0 : -1.0;

    for (long long i = my_first_i; i < my_last_i; i++) {
        while (shared->flag != my_rank);
        shared->suma_global += factor / (2 * i + 1);
        factor = -factor;
        shared->flag = (my_rank + 1) % NUM_HILOS;
    }

    return nullptr;
}

double calcular_pi_busy_waiting_dentro(long long n) {
    pthread_t hilos[NUM_HILOS];
    BusyWaitDentroArgs args[NUM_HILOS];
    BusyWaitData shared_data;

    shared_data.flag = 0;
    shared_data.suma_global = 0.0;

    for (int i = 0; i < NUM_HILOS; i++) {
        args[i].thread_data.id = i;
        args[i].thread_data.n_terminos = n;
        args[i].shared_data = &shared_data;
        pthread_create(&hilos[i], nullptr, thread_busy_waiting_dentro, &args[i]);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], nullptr);
    }

    return 4.0 * shared_data.suma_global;
}

// ============================================
// 3. BUSY-WAITING FUERA DEL BUCLE
// ============================================
struct BusyWaitFueraArgs {
    ThreadData thread_data;
    BusyWaitData* shared_data;
};

void* thread_busy_waiting_fuera(void* arg) {
    BusyWaitFueraArgs* args = static_cast<BusyWaitFueraArgs*>(arg);
    int my_rank = args->thread_data.id;
    long long n = args->thread_data.n_terminos;
    BusyWaitData* shared = args->shared_data;

    long long my_n = n / NUM_HILOS;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    double factor = (my_first_i % 2 == 0) ? 1.0 : -1.0;
    double my_sum = 0.0;

    for (long long i = my_first_i; i < my_last_i; i++) {
        my_sum += factor / (2 * i + 1);
        factor = -factor;
    }

    while (shared->flag != my_rank);
    shared->suma_global += my_sum;
    shared->flag = (my_rank + 1) % NUM_HILOS;

    return nullptr;
}

double calcular_pi_busy_waiting_fuera(long long n) {
    pthread_t hilos[NUM_HILOS];
    BusyWaitFueraArgs args[NUM_HILOS];
    BusyWaitData shared_data;

    shared_data.flag = 0;
    shared_data.suma_global = 0.0;

    for (int i = 0; i < NUM_HILOS; i++) {
        args[i].thread_data.id = i;
        args[i].thread_data.n_terminos = n;
        args[i].shared_data = &shared_data;
        pthread_create(&hilos[i], nullptr, thread_busy_waiting_fuera, &args[i]);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], nullptr);
    }

    return 4.0 * shared_data.suma_global;
}

// ============================================
// 4. MUTEX
// ============================================
struct MutexArgs {
    ThreadData thread_data;
    MutexData* shared_data;
};

void* thread_mutex(void* arg) {
    MutexArgs* args = static_cast<MutexArgs*>(arg);
    int my_rank = args->thread_data.id;
    long long n = args->thread_data.n_terminos;
    MutexData* shared = args->shared_data;

    long long my_n = n / NUM_HILOS;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    double factor = (my_first_i % 2 == 0) ? 1.0 : -1.0;
    double my_sum = 0.0;

    for (long long i = my_first_i; i < my_last_i; i++) {
        my_sum += factor / (2 * i + 1);
        factor = -factor;
    }

    pthread_mutex_lock(&shared->mutex);
    shared->suma_global += my_sum;
    pthread_mutex_unlock(&shared->mutex);

    return nullptr;
}

double calcular_pi_mutex(long long n) {
    pthread_t hilos[NUM_HILOS];
    MutexArgs args[NUM_HILOS];
    MutexData shared_data;

    pthread_mutex_init(&shared_data.mutex, nullptr);
    shared_data.suma_global = 0.0;

    for (int i = 0; i < NUM_HILOS; i++) {
        args[i].thread_data.id = i;
        args[i].thread_data.n_terminos = n;
        args[i].shared_data = &shared_data;
        pthread_create(&hilos[i], nullptr, thread_mutex, &args[i]);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], nullptr);
    }

    pthread_mutex_destroy(&shared_data.mutex);

    return 4.0 * shared_data.suma_global;
}

// ============================================
// FUNCIONES PARA ANÁLISIS
// ============================================

struct Resultado {
    std::string nombre;
    double pi_calculado;
    double tiempo;
    double error;
    double speedup;
};

void guardar_resultados_csv(const std::vector<Resultado>& resultados, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir " << filename << " para escribir resultados\n";
        return;
    }

    // Encabezado CSV
    file << "Estrategia,Pi_Calculado,Tiempo_s,Error,Speedup\n";

    // Datos
    for (const auto& res : resultados) {
        file << res.nombre << ","
             << std::fixed << std::setprecision(15) << res.pi_calculado << ","
             << std::setprecision(6) << res.tiempo << ","
             << res.error << ","
             << res.speedup << "\n";
    }

    file.close();
    std::cout << "Resultados guardados en: " << filename << "\n";
}

void imprimir_tabla_comparativa(const std::vector<Resultado>& resultados) {
    std::cout << "\n" << std::string(120, '=') << "\n";
    std::cout << "TABLA COMPARATIVA DE ESTRATEGIAS DE SINCRONIZACIÓN\n";
    std::cout << std::string(120, '=') << "\n";

    std::cout << std::left << std::setw(25) << "ESTRATEGIA"
              << std::setw(20) << "π CALCULADO"
              << std::setw(15) << "TIEMPO (s)"
              << std::setw(15) << "ERROR"
              << std::setw(15) << "SPEEDUP"
              << std::setw(25) << "EFICIENCIA" << "\n";

    std::cout << std::string(120, '-') << "\n";

    for (const auto& res : resultados) {
        std::cout << std::left << std::setw(25) << res.nombre
                  << std::fixed << std::setprecision(10)
                  << std::setw(20) << res.pi_calculado
                  << std::setprecision(6)
                  << std::setw(15) << res.tiempo
                  << std::setw(15) << res.error
                  << std::setw(15) << res.speedup
                  << std::setw(25) << (res.speedup / NUM_HILOS * 100) << "%\n";
    }
    std::cout << std::string(120, '=') << "\n";
}

void generar_grafico_ascii_tiempos(const std::vector<Resultado>& resultados) {
    std::cout << "\nGRAFICO DE TIEMPOS DE EJECUCION\n";
    std::cout << "==========================================\n";

    double max_tiempo = std::max_element(resultados.begin(), resultados.end(),
        [](const Resultado& a, const Resultado& b) { return a.tiempo < b.tiempo; })->tiempo;

    const int MAX_BARRAS = 50;

    for (const auto& res : resultados) {
        int longitud_barra = static_cast<int>((res.tiempo / max_tiempo) * MAX_BARRAS);
        std::cout << std::left << std::setw(25) << res.nombre
                  << "[" << std::string(longitud_barra, '#')
                  << std::string(MAX_BARRAS - longitud_barra, ' ') << "] "
                  << std::fixed << std::setprecision(4) << res.tiempo << "s\n";
    }
}

void generar_grafico_ascii_speedup(const std::vector<Resultado>& resultados) {
    std::cout << "\nGRAFICO DE SPEEDUP RELATIVO\n";
    std::cout << "==========================================\n";

    double speedup_max = std::max_element(resultados.begin(), resultados.end(),
        [](const Resultado& a, const Resultado& b) { return a.speedup < b.speedup; })->speedup;

    const int MAX_BARRAS = 40;

    for (const auto& res : resultados) {
        int longitud_barra = static_cast<int>((res.speedup / speedup_max) * MAX_BARRAS);
        std::cout << std::left << std::setw(25) << res.nombre
                  << "[" << std::string(longitud_barra, '>')
                  << std::string(MAX_BARRAS - longitud_barra, ' ') << "] "
                  << std::fixed << std::setprecision(2) << res.speedup << "x\n";
    }
}

// ============================================
// MAIN PRINCIPAL
// ============================================
int main() {
    std::cout << "=================================================\n"
              << "    ANALISIS COMPARATIVO: ESTRATEGIAS PI\n"
              << "=================================================\n"
              << "Terminos: " << NUM_TERMINOS << " | Hilos: " << NUM_HILOS << "\n"
              << "pi real: " << std::fixed << std::setprecision(15) << PI_REAL << "\n"
              << "=================================================\n\n";

    std::vector<Resultado> resultados;
    double tiempo_base = 0.0;

    // 1. SECUENCIAL
    std::cout << "Ejecutando calculo SECUENCIAL...\n";
    Timer timer;
    double pi_secuencial = calcular_pi_secuencial(NUM_TERMINOS);
    double tiempo_secuencial = timer.elapsed();
    tiempo_base = tiempo_secuencial;

    resultados.push_back({
        "SECUENCIAL",
        pi_secuencial,
        tiempo_secuencial,
        std::abs(pi_secuencial - PI_REAL),
        1.0
    });

    // 2. BUSY-WAITING DENTRO (solo para demostración con menos términos)
    if (NUM_TERMINOS <= 100000) {
        std::cout << "Ejecutando BUSY-WAITING DENTRO...\n";
        timer = Timer();
        double pi_bw_dentro = calcular_pi_busy_waiting_dentro(NUM_TERMINOS);
        double tiempo_bw_dentro = timer.elapsed();

        resultados.push_back({
            "BUSY-WAITING_DENTRO",
            pi_bw_dentro,
            tiempo_bw_dentro,
            std::abs(pi_bw_dentro - PI_REAL),
            tiempo_base / tiempo_bw_dentro
        });
    }

    // 3. BUSY-WAITING FUERA
    std::cout << "Ejecutando BUSY-WAITING FUERA...\n";
    timer = Timer();
    double pi_bw_fuera = calcular_pi_busy_waiting_fuera(NUM_TERMINOS);
    double tiempo_bw_fuera = timer.elapsed();

    resultados.push_back({
        "BUSY-WAITING_FUERA",
        pi_bw_fuera,
        tiempo_bw_fuera,
        std::abs(pi_bw_fuera - PI_REAL),
        tiempo_base / tiempo_bw_fuera
    });

    // 4. MUTEX
    std::cout << "Ejecutando MUTEX...\n";
    timer = Timer();
    double pi_mutex = calcular_pi_mutex(NUM_TERMINOS);
    double tiempo_mutex = timer.elapsed();

    resultados.push_back({
        "MUTEX",
        pi_mutex,
        tiempo_mutex,
        std::abs(pi_mutex - PI_REAL),
        tiempo_base / tiempo_mutex
    });

    // GENERAR REPORTES
    imprimir_tabla_comparativa(resultados);
    generar_grafico_ascii_tiempos(resultados);
    generar_grafico_ascii_speedup(resultados);

    // GUARDAR RESULTADOS PARA PYTHON
    guardar_resultados_csv(resultados, "resultados_pi.csv");

    std::cout << "\nResultados guardados en 'resultados_pi.csv' para analisis con Python\n";

    return 0;
}