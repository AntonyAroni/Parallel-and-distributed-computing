import matplotlib.pyplot as plt
import numpy as np
import subprocess
import re

def ejecutar_prueba_busy_dentro():
    """Ejecuta la prueba específica de busy-waiting dentro"""
    print("🚀 EJECUTANDO PRUEBA ESPECÍFICA DE BUSY-WAITING DENTRO...")

    # Compilar
    compile_result = subprocess.run([
        'g++', '-std=c++11', '-pthread', '-O2', 'prueba_busy_dentro.cpp', '-o', 'prueba_busy_dentro'
    ], capture_output=True, text=True)

    if compile_result.returncode != 0:
        print("❌ Error de compilación:")
        print(compile_result.stderr)
        return None

    # Ejecutar
    result = subprocess.run(['./prueba_busy_dentro'], capture_output=True, text=True)
    print(result.stdout)

    # Extraer datos de la salida
    return extraer_datos_resultado(result.stdout)

def extraer_datos_resultado(output):
    """Extrae los datos de tiempo y resultados del output"""
    lines = output.split('\n')
    datos = {
        'tiempo_secuencial': 0,
        'tiempo_busy_dentro': 0,
        'pi_secuencial': 0,
        'pi_busy_dentro': 0,
        'factor_mas_lento': 0
    }

    for line in lines:
        if 'Tiempo secuencial:' in line:
            match = re.search(r'(\d+\.\d+)', line)
            if match:
                datos['tiempo_secuencial'] = float(match.group(1))
        elif 'Tiempo busy-waiting dentro:' in line:
            match = re.search(r'(\d+\.\d+)', line)
            if match:
                datos['tiempo_busy_dentro'] = float(match.group(1))
        elif 'π secuencial:' in line:
            match = re.search(r'(\d+\.\d+)', line)
            if match:
                datos['pi_secuencial'] = float(match.group(1))
        elif 'π busy-waiting dentro:' in line:
            match = re.search(r'(\d+\.\d+)', line)
            if match:
                datos['pi_busy_dentro'] = float(match.group(1))
        elif 'veces MÁS LENTO' in line:
            match = re.search(r'(\d+\.\d+)', line)
            if match:
                datos['factor_mas_lento'] = float(match.group(1))

    return datos

def crear_grafico_busy_dentro(datos):
    """Crea gráficos específicos para busy-waiting dentro"""
    if not datos or datos['tiempo_secuencial'] == 0:
        print("❌ No hay datos suficientes para graficar")
        return

    # Configurar estilo
    plt.style.use('seaborn-v0_8')
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # Gráfico 1: Comparación de tiempos
    estrategias = ['Secuencial', 'Busy-Waiting Dentro']
    tiempos = [datos['tiempo_secuencial'], datos['tiempo_busy_dentro']]
    colores = ['#27AE60', '#E74C3C']

    bars = ax1.bar(estrategias, tiempos, color=colores, alpha=0.8, edgecolor='black')
    ax1.set_title('COMPARACIÓN DE TIEMPOS\nBusy-Waiting DENTRO vs Secuencial',
                  fontsize=14, fontweight='bold', pad=20)
    ax1.set_ylabel('Tiempo (segundos)', fontweight='bold')
    ax1.grid(axis='y', alpha=0.3)

    # Añadir valores en las barras
    for bar in bars:
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height + max(tiempos)*0.01,
                f'{height:.4f}s', ha='center', va='bottom', fontweight='bold')

    # Añadir anotación de comparación
    if datos['factor_mas_lento'] > 0:
        ax1.annotate(f'{datos["factor_mas_lento"]:.1f}x más lento',
                    xy=(1, datos['tiempo_busy_dentro']),
                    xytext=(1.2, datos['tiempo_busy_dentro'] * 1.1),
                    arrowprops=dict(arrowstyle='->', color='red', lw=1.5),
                    fontweight='bold', color='red', fontsize=12)

    # Gráfico 2: Análisis conceptual del problema
    categorias = ['Trabajo Útil', 'Sincronización', 'Espera Activa']
    secuencial = [95, 5, 0]  # Porcentajes estimados
    busy_dentro = [10, 5, 85]  # Porcentajes estimados

    x = np.arange(len(categorias))
    width = 0.35

    bars1 = ax2.bar(x - width/2, secuencial, width, label='Secuencial',
                   color='#27AE60', alpha=0.8)
    bars2 = ax2.bar(x + width/2, busy_dentro, width, label='Busy-Waiting Dentro',
                   color='#E74C3C', alpha=0.8)

    ax2.set_title('DISTRIBUCIÓN DEL TIEMPO DE CPU\nAnálisis Conceptual',
                  fontsize=14, fontweight='bold', pad=20)
    ax2.set_xlabel('Tipo de Actividad', fontweight='bold')
    ax2.set_ylabel('Porcentaje del Tiempo (%)', fontweight='bold')
    ax2.set_xticks(x)
    ax2.set_xticklabels(categorias, rotation=45)
    ax2.legend()
    ax2.grid(axis='y', alpha=0.3)

    # Añadir valores en barras
    for bars in [bars1, bars2]:
        for bar in bars:
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height + 1,
                    f'{height:.0f}%', ha='center', va='bottom', fontsize=9)

    # Ajustar layout
    plt.tight_layout(pad=3.0)

    # Título general
    fig.suptitle('ANÁLISIS DEL ANTI-PATRÓN: BUSY-WAITING DENTRO DEL BUCLE\n' +
                 '10,000 términos | 4 hilos | Cálculo de π',
                 fontsize=16, fontweight='bold', y=1.02)

    # Guardar
    plt.savefig('analisis_busy_waiting_dentro.png', dpi=300, bbox_inches='tight')
    plt.show()

    return datos

def generar_reporte_tecnico(datos):
    """Genera un reporte técnico detallado"""
    print("\n" + "="*80)
    print("REPORTE TÉCNICO: BUSY-WAITING DENTRO DEL BUCLE")
    print("="*80)

    print(f"\n📊 DATOS OBTENIDOS:")
    print(f"   • Tiempo secuencial: {datos['tiempo_secuencial']:.6f} segundos")
    print(f"   • Tiempo busy-waiting dentro: {datos['tiempo_busy_dentro']:.6f} segundos")
    print(f"   • Factor de lentitud: {datos.get('factor_mas_lento', 0):.2f}x")
    print(f"   • π calculado secuencial: {datos['pi_secuencial']:.10f}")
    print(f"   • π calculado busy-waiting: {datos['pi_busy_dentro']:.10f}")

    print(f"\n🔍 ANÁLISIS TÉCNICO:")
    print(f"   1. OVERHEAD DE SINCRONIZACIÓN:")
    print(f"      - 10,000 términos × 4 hilos = ~40,000 operaciones de sincronización")
    print(f"      - Cada término requiere espera activa y cambio de contexto")
    print(f"      - El costo de sincronización domina sobre el cálculo útil")

    print(f"\n   2. PROBLEMAS IDENTIFICADOS:")
    print(f"      • SERIALIZACIÓN: Solo un hilo trabaja a la vez")
    print(f"      • ESPERA ACTIVA: Consumo innecesario de CPU")
    print(f"      • CONTENCIÓN: Todos los hilos compiten por el mismo recurso")
    print(f"      • INEFICIENCIA: Más lento que la versión secuencial")

    print(f"\n🎯 RECOMENDACIONES DE DISEÑO:")
    print(f"   ✅ BUSY-WAITING FUERA del bucle:")
    print(f"      - Cálculo paralelo independiente")
    print(f"      - Una sola sincronización al final")
    print(f"      - Máximo aprovechamiento del paralelismo")

    print(f"\n   ✅ MUTEX para secciones críticas:")
    print(f"      - Sincronización controlada por el SO")
    print(f"      - No consume CPU en espera")
    print(f"      - Eficiente para operaciones agrupadas")

    print(f"\n   ❌ EVITAR busy-waiting dentro de bucles:")
    print(f"      - Anti-patrón demostrado")
    print(f"      - Serialización completa")
    print(f"      - Pérdida total de beneficios del paralelismo")

# Ejecutar análisis completo
if __name__ == "__main__":
    # Ejecutar prueba
    datos = ejecutar_prueba_busy_dentro()

    if datos and datos['tiempo_secuencial'] > 0:
        # Crear gráficos
        crear_grafico_busy_dentro(datos)

        # Generar reporte
        generar_reporte_tecnico(datos)

        print(f"\n🎉 PRUEBA COMPLETADA!")
        print(f"   - Gráfico: analisis_busy_waiting_dentro.png")
        print(f"   - Demostración práctica del anti-patrón")
    else:
        print("❌ No se pudieron obtener datos de la prueba")