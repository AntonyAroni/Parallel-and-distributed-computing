#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <vector>

// Configuración para prueba específica
constexpr long long NUM_TERMINOS = 10000LL;  // Solo 10,000 para prueba
constexpr int NUM_HILOS = 4;
constexpr double PI_REAL = 3.14159265358979323846;

// ============================================
// ESTRUCTURAS
// ============================================

struct ThreadData {
    int id;
    long long n_terminos;
};

struct BusyWaitData {
    volatile long flag;
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
// BUSY-WAITING DENTRO DEL BUCLE
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

    std::cout << "   Hilo " << my_rank << ": procesando términos " << my_first_i << " a " << (my_last_i-1) << "\n";

    // BUSY-WAITING DENTRO: Sincroniza cada término individualmente
    for (long long i = my_first_i; i < my_last_i; i++) {
        // ESPERAR TURNO - Esto serializa completamente el proceso
        while (shared->flag != my_rank) {
            // Busy-waiting: el hilo consume CPU esperando
        }

        // REGIÓN CRÍTICA - Solo un hilo puede estar aquí a la vez
        shared->suma_global += factor / (2 * i + 1);
        factor = -factor;

        // PASAR AL SIGUIENTE HILO
        shared->flag = (my_rank + 1) % NUM_HILOS;

        // Mostrar progreso cada 1000 términos
        if ((i - my_first_i) % 1000 == 0) {
            std::cout << "   Hilo " << my_rank << ": término " << i << " completado\n";
        }
    }

    std::cout << "   Hilo " << my_rank << ": FINALIZADO\n";
    return nullptr;
}

double calcular_pi_busy_waiting_dentro(long long n) {
    std::cout << "🚀 INICIANDO BUSY-WAITING DENTRO con " << n << " términos y " << NUM_HILOS << " hilos\n";
    std::cout << "⚠️  ADVERTENCIA: Esta estrategia SERIALIZA completamente el cálculo\n";
    std::cout << "   Cada hilo espera su turno para procesar UN solo término\n\n";

    pthread_t hilos[NUM_HILOS];
    BusyWaitDentroArgs args[NUM_HILOS];
    BusyWaitData shared_data;

    shared_data.flag = 0;  // Empieza el hilo 0
    shared_data.suma_global = 0.0;

    std::cout << "🧵 CREANDO HILOS...\n";
    for (int i = 0; i < NUM_HILOS; i++) {
        args[i].thread_data.id = i;
        args[i].thread_data.n_terminos = n;
        args[i].shared_data = &shared_data;

        pthread_create(&hilos[i], nullptr, thread_busy_waiting_dentro, &args[i]);
    }

    std::cout << "⏳ ESPERANDO QUE LOS HILOS TERMINEN...\n";
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], nullptr);
    }

    std::cout << "✅ TODOS LOS HILOS HAN TERMINADO\n";
    return 4.0 * shared_data.suma_global;
}

// ============================================
// FUNCIÓN SECUENCIAL PARA COMPARACIÓN
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
// ANÁLISIS DETALLADO
// ============================================
void analizar_busy_waiting_dentro() {
    std::cout << "=================================================\n";
    std::cout << "   ANÁLISIS DETALLADO: BUSY-WAITING DENTRO\n";
    std::cout << "=================================================\n\n";

    std::cout << "🔍 CÓMO FUNCIONA BUSY-WAITING DENTRO:\n";
    std::cout << "   1. Cada hilo calcula un rango de términos\n";
    std::cout << "   2. Pero debe ESPERAR SU TURNO para cada término individual\n";
    std::cout << "   3. Solo un hilo puede trabajar a la vez\n";
    std::cout << "   4. Los demás hilos consumen CPU esperando\n";
    std::cout << "   5. El procesamiento es COMPLETAMENTE SERIAL\n\n";

    std::cout << "📊 CÁLCULO DEL OVERHEAD:\n";
    std::cout << "   - Términos: " << NUM_TERMINOS << "\n";
    std::cout << "   - Hilos: " << NUM_HILOS << "\n";
    std::cout << "   - Cambios de contexto: " << NUM_TERMINOS * NUM_HILOS << " (aproximado)\n";
    std::cout << "   - Synchronization points: " << NUM_TERMINOS << "\n\n";

    // Calcular secuencial primero
    std::cout << "🔄 CALCULANDO VERSIÓN SECUENCIAL...\n";
    Timer timer_sec;
    double pi_secuencial = calcular_pi_secuencial(NUM_TERMINOS);
    double tiempo_secuencial = timer_sec.elapsed();

    std::cout << "   π secuencial: " << std::fixed << std::setprecision(10) << pi_secuencial << "\n";
    std::cout << "   Tiempo secuencial: " << std::setprecision(6) << tiempo_secuencial << "s\n\n";

    // Calcular busy-waiting dentro
    std::cout << "🔄 CALCULANDO BUSY-WAITING DENTRO...\n";
    Timer timer_bw;
    double pi_bw_dentro = calcular_pi_busy_waiting_dentro(NUM_TERMINOS);
    double tiempo_bw_dentro = timer_bw.elapsed();

    std::cout << "\n   π busy-waiting dentro: " << std::fixed << std::setprecision(10) << pi_bw_dentro << "\n";
    std::cout << "   Tiempo busy-waiting dentro: " << std::setprecision(6) << tiempo_bw_dentro << "s\n\n";

    // Análisis comparativo
    std::cout << "📈 ANÁLISIS COMPARATIVO:\n";
    std::cout << "   Tiempo secuencial: " << tiempo_secuencial << "s\n";
    std::cout << "   Tiempo busy-waiting dentro: " << tiempo_bw_dentro << "s\n";

    if (tiempo_bw_dentro > tiempo_secuencial) {
        double mas_lento = tiempo_bw_dentro / tiempo_secuencial;
        std::cout << "   ⚠️  Busy-waiting dentro es " << std::setprecision(2) << mas_lento
                  << " veces MÁS LENTO que secuencial\n";
    } else {
        std::cout << "   ✅ Busy-waiting dentro es más rápido (caso raro)\n";
    }

    std::cout << "   Error absoluto: " << std::scientific << std::abs(pi_bw_dentro - PI_REAL) << "\n\n";

    // Explicación detallada
    std::cout << "💡 EXPLICACIÓN DEL PROBLEMA:\n";
    std::cout << "   El busy-waiting dentro DEL bucle:\n";
    std::cout << "   - Serializa el trabajo completamente\n";
    std::cout << "   - Añade overhead de sincronización por CADA término\n";
    std::cout << "   - Los hilos pasan más tiempo esperando que calculando\n";
    std::cout << "   - Consume recursos de CPU innecesariamente\n\n";

    std::cout << "🎯 CUÁNDO USAR (Y CUÁNDO NO):\n";
    std::cout << "   ❌ NUNCA usar busy-waiting dentro para cálculos numéricos\n";
    std::cout << "   ❌ Evitar cuando hay muchas operaciones simples\n";
    std::cout << "   ⚠️  Solo considerar para operaciones MUY costosas\n";
    std::cout << "   ✅ Mejor alternativa: busy-waiting FUERA del bucle\n";
    std::cout << "   ✅ Mejor alternativa: mutex para secciones críticas\n";
}

// ============================================
// PRUEBA ADICIONAL: DIFERENTES CONFIGURACIONES
// ============================================
void prueba_diferentes_configuraciones() {
    std::cout << "\n=================================================\n";
    std::cout << "   PRUEBA CON DIFERENTES NÚMEROS DE HILOS\n";
    std::cout << "=================================================\n";

    std::vector<int> config_hilos = {2, 4, 8};

    for (int hilos : config_hilos) {
        std::cout << "\n🧪 PROBANDO CON " << hilos << " HILOS:\n";

        // Versión simplificada para la prueba
        pthread_t hilos_arr[hilos];
        BusyWaitDentroArgs args_arr[hilos];
        BusyWaitData shared_data;

        shared_data.flag = 0;
        shared_data.suma_global = 0.0;

        Timer timer;

        // Crear hilos
        for (int i = 0; i < hilos; i++) {
            args_arr[i].thread_data.id = i;
            args_arr[i].thread_data.n_terminos = NUM_TERMINOS;
            args_arr[i].shared_data = &shared_data;
            pthread_create(&hilos_arr[i], nullptr, thread_busy_waiting_dentro, &args_arr[i]);
        }

        // Esperar hilos
        for (int i = 0; i < hilos; i++) {
            pthread_join(hilos_arr[i], nullptr);
        }

        double tiempo = timer.elapsed();
        double pi = 4.0 * shared_data.suma_global;

        std::cout << "   Resultado: π ≈ " << std::fixed << std::setprecision(10) << pi << "\n";
        std::cout << "   Tiempo: " << std::setprecision(6) << tiempo << "s\n";
        std::cout << "   Eficiencia: " << (hilos > 1 ? "BAJA (serializado)" : "N/A") << "\n";
    }
}

int main() {
    std::cout << "=================================================\n";
    std::cout << "   PRUEBA ESPECÍFICA: BUSY-WAITING DENTRO\n";
    std::cout << "=================================================\n";
    std::cout << "Términos: " << NUM_TERMINOS << " | π real: " << std::fixed << std::setprecision(15) << PI_REAL << "\n\n";

    // Análisis principal
    analizar_busy_waiting_dentro();

    // Prueba adicional opcional (comentar si es muy lento)
    // prueba_diferentes_configuraciones();

    std::cout << "=================================================\n";
    std::cout << "   CONCLUSIONES FINALES\n";
    std::cout << "=================================================\n";
    std::cout << "🔴 BUSY-WAITING DENTRO ES UN ANTI-PATRÓN:\n";
    std::cout << "   1. Serializa el trabajo en lugar de paralelizarlo\n";
    std::cout << "   2. Añade overhead enorme de sincronización\n";
    std::cout << "   3. Consume recursos de CPU innecesariamente\n";
    std::cout << "   4. Es MÁS LENTO que la versión secuencial\n";
    std::cout << "   5. NO USAR en código de producción\n\n";

    std::cout << "💡 ALTERNATIVAS RECOMENDADAS:\n";
    std::cout << "   • Busy-waiting FUERA del bucle\n";
    std::cout << "   • Mutex para secciones críticas\n";
    std::cout << "   • Semáforos para control de acceso\n";
    std::cout << "   • Barreras para sincronización grupal\n";

    return 0;
}