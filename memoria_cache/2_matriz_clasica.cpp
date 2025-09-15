#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

// Multiplicación clásica: C = A * B
void matrixMultiply(const vector<vector<double>> &A,
                    const vector<vector<double>> &B,
                    vector<vector<double>> &C,
                    int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int sizes[] = {100, 200, 400, 800, 1000};

    for (int N : sizes) {
        vector<vector<double>> A(N, vector<double>(N));
        vector<vector<double>> B(N, vector<double>(N));
        vector<vector<double>> C(N, vector<double>(N));

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = (rand() % 100) / 100.0;
                B[i][j] = (rand() % 100) / 100.0;
            }
        }

        //tiempo de multiplicación
        auto start = high_resolution_clock::now();
        matrixMultiply(A, B, C, N);
        auto end = high_resolution_clock::now();

        auto duration = duration_cast<milliseconds>(end - start).count();
        cout << "N = " << N << " => Tiempo: "
             << duration << " ms" << endl;
    }

    return 0;
}
