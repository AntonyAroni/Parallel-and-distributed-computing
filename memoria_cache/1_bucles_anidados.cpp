#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int MAX = 2000;

double A[MAX][MAX];
double x[MAX], y[MAX];

int main() {
    for (int i = 0; i < MAX; i++) {
        x[i] = 1.0;
        for (int j = 0; j < MAX; j++) {
            A[i][j] = static_cast<double>(i + j) / MAX;
        }
    }

    /* -------- Primer par de bucles -------- */
    for (int i = 0; i < MAX; i++)
        y[i] = 0.0;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(end - start).count();
    cout << "Tiempo Primer par de bucles (row-major): "
         << duration1 << " ms" << endl;

    /* -------- Segundo par de bucles -------- */
    for (int i = 0; i < MAX; i++)
        y[i] = 0.0;

    start = high_resolution_clock::now();
    for (int j = 0; j < MAX; j++) {
        for (int i = 0; i < MAX; i++) {
            y[i] += A[i][j] * x[j];
        }
    }
    end = high_resolution_clock::now();
    auto duration2 = duration_cast<milliseconds>(end - start).count();
    cout << "Tiempo Segundo par de bucles (col-major): "
         << duration2 << " ms" << endl;

    return 0;
}
