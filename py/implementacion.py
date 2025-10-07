import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import subprocess
import os

def ejecutar_cpp_y_obtener_datos():
    """Ejecuta el programa C++ y obtiene los datos reales"""
    print("🔨 Compilando y ejecutando programa C++...")

    # Compilar
    compile_result = subprocess.run([
        'g++', '-std=c++11', '-pthread', '-O2', 'pi_analisis.cpp', '-o', 'pi_analisis'
    ], capture_output=True, text=True)

    if compile_result.returncode != 0:
        print("❌ Error de compilación:")
        print(compile_result.stderr)
        return None

    # Ejecutar
    print("🚀 Ejecutando cálculo de π...")
    ejecucion_result = subprocess.run(['./pi_analisis'], capture_output=True, text=True)
    print(ejecucion_result.stdout)

    if ejecucion_result.returncode != 0:
        print("❌ Error en ejecución:")
        print(ejecucion_result.stderr)
        return None

    # Leer resultados del CSV
    if os.path.exists('resultados_pi.csv'):
        return pd.read_csv('resultados_pi.csv')
    else:
        print("❌ No se encontró el archivo de resultados")
        return None

def crear_graficos_desde_datos_reales(df):
    """Crea gráficos profesionales desde los datos reales"""
    if df is None or df.empty:
        print("❌ No hay datos para graficar")
        return

    print("\n📊 DATOS REALES OBTENIDOS:")
    print(df)

    # Configurar estilo
    plt.style.use('seaborn-v0_8')
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))

    # Colores
    colors = plt.cm.Set3(np.linspace(0, 1, len(df)))

    # Gráfico 1: Tiempos de ejecución
    bars1 = ax1.bar(df['Estrategia'], df['Tiempo_s'], color=colors, alpha=0.8, edgecolor='black')
    ax1.set_title('TIEMPOS DE EJECUCIÓN REALES\npor Estrategia de Sincronización',
                  fontsize=14, fontweight='bold', pad=20)
    ax1.set_ylabel('Tiempo (segundos)', fontweight='bold')
    ax1.tick_params(axis='x', rotation=45)
    ax1.grid(axis='y', alpha=0.3)

    # Valores en barras
    for bar in bars1:
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height + max(df['Tiempo_s'])*0.01,
                 f'{height:.4f}s', ha='center', va='bottom', fontweight='bold', fontsize=9)

    # Gráfico 2: Speedup comparativo
    bars2 = ax2.bar(df['Estrategia'], df['Speedup'], color=colors, alpha=0.8, edgecolor='black')
    ax2.set_title('SPEEDUP RELATIVO REAL\nvs Implementación Secuencial',
                  fontsize=14, fontweight='bold', pad=20)
    ax2.set_ylabel('Speedup (x)', fontweight='bold')
    ax2.axhline(y=1, color='red', linestyle='--', alpha=0.7, label='Línea base (Secuencial)')
    ax2.tick_params(axis='x', rotation=45)
    ax2.grid(axis='y', alpha=0.3)
    ax2.legend()

    # Valores en barras
    for bar in bars2:
        height = bar.get_height()
        ax2.text(bar.get_x() + bar.get_width()/2., height + max(df['Speedup'])*0.01,
                 f'{height:.3f}x', ha='center', va='bottom', fontweight='bold', fontsize=9)

    # Gráfico 3: Error de aproximación
    bars3 = ax3.bar(df['Estrategia'], df['Error'], color=colors, alpha=0.8, edgecolor='black')
    ax3.set_title('ERROR DE APROXIMACIÓN REAL\nvs π Real (3.141592653589793)',
                  fontsize=14, fontweight='bold', pad=20)
    ax3.set_ylabel('Error Absoluto', fontweight='bold')
    ax3.tick_params(axis='x', rotation=45)
    if max(df['Error']) > min(df['Error']) * 100:  # Usar escala log si hay mucha variación
        ax3.set_yscale('log')
    ax3.grid(axis='y', alpha=0.3)

    # Valores en barras
    for bar in bars3:
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height * 1.1,
                 f'{height:.2e}', ha='center', va='bottom', fontweight='bold', fontsize=9)

    # Gráfico 4: Eficiencia de paralelización (asumiendo 4 hilos)
    eficiencia = (df['Speedup'] / 4) * 100  # 4 hilos
    bars4 = ax4.bar(df['Estrategia'], eficiencia, color=colors, alpha=0.8, edgecolor='black')
    ax4.set_title('EFICIENCIA DE PARALELIZACIÓN REAL\n(4 Hilos - Ideal: 25% por hilo)',
                  fontsize=14, fontweight='bold', pad=20)
    ax4.set_ylabel('Eficiencia (%)', fontweight='bold')
    ax4.axhline(y=25, color='green', linestyle='--', alpha=0.7, label='Eficiencia base por hilo')
    ax4.tick_params(axis='x', rotation=45)
    ax4.grid(axis='y', alpha=0.3)
    ax4.legend()

    # Valores en barras
    for bar in bars4:
        height = bar.get_height()
        ax4.text(bar.get_x() + bar.get_width()/2., height + 1,
                 f'{height:.1f}%', ha='center', va='bottom', fontweight='bold', fontsize=9)

    # Ajustar layout
    plt.tight_layout(pad=4.0)

    # Título general
    fig.suptitle('ANÁLISIS COMPARATIVO REAL: ESTRATEGIAS DE SINCRONIZACIÓN\n' +
                 'Datos obtenidos de ejecución real del programa C++',
                 fontsize=16, fontweight='bold', y=1.02)

    # Guardar
    plt.savefig('comparativa_estrategias_reales.png', dpi=300, bbox_inches='tight',
                facecolor='white', edgecolor='black')
    plt.show()

    return df, eficiencia

def analizar_resultados_reales(df, eficiencia):
    """Analiza los resultados reales obtenidos"""
    print("\n" + "="*70)
    print("ANÁLISIS ESTADÍSTICO DE RESULTADOS REALES")
    print("="*70)

    print(f"\n📊 MÉTRICAS DE RENDIMIENTO REALES:")
    mejor_tiempo_idx = df['Tiempo_s'].idxmin()
    mejor_speedup_idx = df['Speedup'].idxmax()

    print(f"   Estrategia más rápida: {df.loc[mejor_tiempo_idx, 'Estrategia']} "
          f"({df.loc[mejor_tiempo_idx, 'Tiempo_s']:.4f}s)")
    print(f"   Mejor speedup: {df.loc[mejor_speedup_idx, 'Estrategia']} "
          f"({df.loc[mejor_speedup_idx, 'Speedup']:.3f}x)")

    if len(df) > 1:
        diferencia = df['Tiempo_s'].max() / df['Tiempo_s'].min()
        print(f"   Diferencia máxima: {diferencia:.2f}x más lenta")

    print(f"\n🎯 PRECISIÓN NUMÉRICA:")
    mejor_precision_idx = df['Error'].idxmin()
    print(f"   Mejor precisión: {df.loc[mejor_precision_idx, 'Estrategia']} "
          f"(Error: {df.loc[mejor_precision_idx, 'Error']:.2e})")

    print(f"\n📈 EFICIENCIA DE PARALELIZACIÓN:")
    mejor_eficiencia_idx = eficiencia.idxmax()
    print(f"   Mejor eficiencia: {df.loc[mejor_eficiencia_idx, 'Estrategia']} "
          f"({eficiencia[mejor_eficiencia_idx]:.1f}%)")

    print(f"\n💡 INTERPRETACIÓN DE RESULTADOS REALES:")
    for _, row in df.iterrows():
        print(f"   {row['Estrategia']}: {row['Tiempo_s']:.4f}s, Speedup: {row['Speedup']:.3f}x")

    print(f"\n🚀 RECOMENDACIONES BASADAS EN DATOS REALES:")
    if 'MUTEX' in df['Estrategia'].values:
        print("   ✅ MUTEX: Generalmente mejor balance rendimiento/facilidad de uso")
    if 'BUSY-WAITING_FUERA' in df['Estrategia'].values:
        print("   ⚠️  BUSY-WAITING_FUERA: Buen rendimiento pero consume CPU en espera")
    if 'BUSY-WAITING_DENTRO' in df['Estrategia'].values:
        print("   ❌ BUSY-WAITING_DENTRO: Evitar - serialización completa")

# Ejecutar análisis completo
if __name__ == "__main__":
    # Obtener datos reales ejecutando el C++
    df = ejecutar_cpp_y_obtener_datos()

    if df is not None:
        # Crear gráficos con datos reales
        df, eficiencia = crear_graficos_desde_datos_reales(df)

        # Analizar resultados
        analizar_resultados_reales(df, eficiencia)

        print(f"\n🎉 ANÁLISIS COMPLETADO CON DATOS REALES!")
        print(f"   - Gráficos guardados: comparativa_estrategias_reales.png")
        print(f"   - Datos crudos: resultados_pi.csv")
    else:
        print("❌ No se pudieron obtener datos para el análisis")