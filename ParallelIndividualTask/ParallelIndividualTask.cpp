// ParallelIndividualTask.cpp: определяет точку входа для консольного приложения.
// Домбровской Анны
//

#include "stdafx.h"
#include <iostream>
#include <ctime>
#include "omp.h"

const int N = 4;

int Size(int blockSize) {
	int num = N / blockSize;
	int size = (num + 1)*num / 2 * blockSize*blockSize;
	return size;
}

// Create zero matrix
int** CreateMatrix(int size) {
	int** A = new int*[size];
	for (int i = 0; i < size; ++i) {
		A[i] = new int[N];
		for (int j = 0; j < size; ++j) {
			A[i][j] = 0;
		}
	}
	return A;
}

// Random symmetric matrix A(nxn)
int** GetRandomMatrixA() {
	int** A = CreateMatrix(N);
	for (int i = 0; i < N; ++i) {
		for (int j = i; j < N; ++j) {
			int m = rand() % 10;
			A[i][j] = m;
			A[j][i] = m;
		}
	}
	return A;
}

// Random upper matrix B(nxn)
int** GetRandomMatrixB() {
	int** B = CreateMatrix(N);
	for (int i = 0; i < N; ++i) {
		for (int j = i; j < N; ++j) {
			int m = rand() % 10;
			B[i][j] = m;
		}
	}
	return B;
}

// Делим матрицу на блоки по строкам
// A - исходная матрица
// blockSize - размер блоков
// Блоки представлены в виде векторов
int* CreateBlockA(int** A, int blockSize) {
	int num = N / blockSize;
	int S = (num + 1)*num / 2 * blockSize*blockSize;
	//std::cout << S << " - size" << std::endl;
	int* V = new int[S]();
	int k = 0;
	for (int l = 0; l < num; ++l)
		for (int m = l; m < num; ++m)
			for (int i = 0; i < blockSize; ++i)
				for (int j = 0; j < blockSize; ++j) {
					V[k++] = A[i + l * blockSize][j + m * blockSize];
				}
	return V;
}

// Делим матрицу на блоки по столбцам
// B - исходная матрица
// blockSize - размер блоков
// Блоки представлены в виде векторов
int* CreateBlockB(int** B, int blockSize) {
	int num = N / blockSize;
	int S = (num + 1)*num / 2 * blockSize*blockSize;
	//std::cout << blockSize << " - blockSize" << std::endl;
	int* V = new int[S]();
	int k = 0;
	for (int l = 0; l < num; ++l)
		for (int m = l; m < num; ++m)
			for (int i = 0; i < blockSize; ++i)
				for (int j = 0; j < blockSize; ++j) {
					V[k++] = B[j + blockSize * l][i + blockSize * m];
				}
	return V;
}

void Delete(int **M)
{
	for (int i = 0; i < N; i++)
		delete[] M[i];
	delete[] M;
}

// Вывод матрицы размеров (NxN)
void Print(int** M) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			std::cout << M[i][j] << ' ';
		}
		std::cout << std::endl;
	}
}

void PrintAB(int* M, int blockSize, char c) {
	//std::cout << "Block size = "<< blockSize << std::endl;
	std::cout << "Matrix " << c <<  "\t";
	for (int i = 0; i < Size(blockSize); ++i) {
		std::cout << M[i] << ' ';
	}
	std::cout << '\n';
}

void Print(int* M, int blockSize) {
	//std::cout << "\nFor block size = " << blockSize << std::endl;
	std::cout << "Matrix:\t ";
	for (int i = 0; i < N*N; ++i) {
		std::cout << M[i] << ' ';
	}
	std::cout << '\n\n';
}

/*_______________________________________________________________________________________________________*/

int* Multiplication(int* A, int* B, int size)
{
	int* C = new int[N*N];
	for (int i = 0; i < N*N; ++i)
		C[i] = 0;

	double time_begin, time_end;
	time_begin = omp_get_wtime();

	// Количество блоков
	int num = N / size;

	for (int i = 0; i < num; ++i)
		for (int j = 0; j < num; ++j)
		{
			int k = 0;

			while (k < i && k <= j) {
				int *a = A + (k*(num + 1) - k * (k + 1) / 2 + i - k) * size*size,
					*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
					*c = C + i * N*size + j * size*size;

				for (int ii = 0; ii < size; ++ii)
					for (int jj = 0; jj < size; ++jj)
						for (int kk = 0; kk < size; ++kk)
							c[ii*size + jj] += a[kk*size + ii] * b[jj* size + kk];
				++k;
			}

			if (k <= j) {
				int *a = A + (k*(num + 1) - k * (k + 1) / 2 + i - k) * size*size,
					*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
					*c = C + i * N*size + j * size*size;

				for (int ii = 0; ii < size; ++ii)
					for (int jj = 0; jj < size; ++jj)
						for (int kk = 0; kk < size; ++kk)
							if (ii <= kk)
								c[ii*size + jj] += a[ii*size + kk] * b[jj* size + kk];
							else
								c[ii*size + jj] += a[kk*size + ii] * b[jj* size + kk];
			}
			k++;

			while (k <= j) {
				int *a = A + (i*(num + 1) - i * (i + 1) / 2 + k - i) * size*size,
					*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
					*c = C + i * N*size + j * size*size;

				for (int ii = 0; ii < size; ++ii)
					for (int jj = 0; jj < size; ++jj)
						for (int kk = 0; kk < size; ++kk)
							c[ii*size + jj] += a[ii*size + kk] * b[jj* size + kk];

				++k;
			}

		}


	time_end = omp_get_wtime();

	printf("\n NONPARALLEL ");

	printf("\nFor block size %2d: ", size);
	for (size_t j = 0; j < N*N; ++j)
		printf("%4d", C[j]);
	printf("\n");

	auto t = time_end - time_begin;
	printf("\n NONPARALLEL TIME = %f seconds", t);

	return C;
}

/*_______________________________________________________________________________________________________*/

int* MultiplicationInParallelThreads(int* A, int* B, int size)
{
	int* C = new int[N*N];
	for (int i = 0; i < N*N; ++i)
		C[i] = 0;

	double time_begin, time_end;
	time_begin = omp_get_wtime();
#pragma omp parallel num_threads(4) 
	{
		int num = N / size;
#pragma omp for  schedule(static)
		for (int i = 0; i < num; ++i)
			for (int j = 0; j < num; ++j)
			{
				int k = 0;

				while (k < i && k <= j) {
					int *a = A + (k*(num + 1) - k * (k + 1) / 2 + i - k) * size*size,
						*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
						*c = C + i * N*size + j * size*size;

					for (int ii = 0; ii < size; ++ii)
						for (int jj = 0; jj < size; ++jj)
							for (int kk = 0; kk < size; ++kk)
								c[ii*size + jj] += a[kk*size + ii] * b[jj* size + kk];
					++k;
				}

				if (k <= j) {
					int *a = A + (k*(num + 1) - k * (k + 1) / 2 + i - k) * size*size,
						*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
						*c = C + i * N*size + j * size*size;

					for (int ii = 0; ii < size; ++ii)
						for (int jj = 0; jj < size; ++jj)
							for (int kk = 0; kk < size; ++kk)
								//if (ii <= kk)
									c[ii*size + jj] += a[ii*size + kk] * b[jj* size + kk];
								//else
									//c[ii*size + jj] += a[kk*size + ii] * b[jj* size + kk];
				}
				k++;

				while (k <= j) {
					int *a = A + (i*(num + 1) - i * (i + 1) / 2 + k - i) * size*size,
						*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
						*c = C + i * N*size + j * size*size;

					for (int ii = 0; ii < size; ++ii)
						for (int jj = 0; jj < size; ++jj)
							for (int kk = 0; kk < size; ++kk)
								c[ii*size + jj] += a[ii*size + kk] * b[jj* size + kk];

					++k;
				}

			}
	}


	time_end = omp_get_wtime();

	//printf("\n PARALLEL THREADS ");

	printf("\nFor block size %2d: ", size);
	for (size_t j = 0; j < N*N; ++j)
	printf("%4d", C[j]);
	printf("\n");

	auto t = time_end - time_begin;
	printf("\n PARALLEL THREADS TIME = %f seconds", t);

	return C;
}
/*_______________________________________________________________________________________________________*/

int* ParallelMultiplicationForBlocks(int* A, int* B, int size)
{
	int* C = new int[N*N];
	for (int i = 0; i < N*N; ++i)
		C[i] = 0;

	int num = N / size;

	double time_begin, time_end;
	time_begin = omp_get_wtime();
		for (int i = 0; i < num; ++i)
			for (int j = 0; j < num; ++j)
			{
#pragma omp parallel num_threads(4) 
				{
#pragma omp for  schedule(static)
					int k = 0;

					while (k < i && k <= j) {
						int *a = A + (k*(num + 1) - k * (k + 1) / 2 + i - k) * size*size,
							*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
							*c = C + i * N*size + j * size*size;

						for (int ii = 0; ii < size; ++ii)
							for (int jj = 0; jj < size; ++jj)
								for (int kk = 0; kk < size; ++kk)
									c[ii*size + jj] += a[kk*size + ii] * b[jj* size + kk];
						++k;
					}

					if (k <= j) {
						int *a = A + (k*(num + 1) - k * (k + 1) / 2 + i - k) * size*size,
							*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
							*c = C + i * N*size + j * size*size;

						for (int ii = 0; ii < size; ++ii)
							for (int jj = 0; jj < size; ++jj)
								for (int kk = 0; kk < size; ++kk)
									//if (ii <= kk)
										c[ii*size + jj] += a[ii*size + kk] * b[jj* size + kk];
									//else
										//c[ii*size + jj] += a[kk*size + ii] * b[jj* size + kk];
					}
					k++;

					while (k <= j) {
						int *a = A + (i*(num + 1) - i * (i + 1) / 2 + k - i) * size*size,
							*b = B + (j - k + k * (num + 1) - k * (k + 1) / 2)* size*size,
							*c = C + i * N*size + j * size*size;

						for (int ii = 0; ii < size; ++ii)
							for (int jj = 0; jj < size; ++jj)
								for (int kk = 0; kk < size; ++kk)
									c[ii*size + jj] += a[ii*size + kk] * b[jj* size + kk];

						++k;
					}

				}
			}


	time_end = omp_get_wtime();

	printf("\n PARALLEL BLOCK ");

	printf("\nFor block size %2d: ", size);
	for (size_t j = 0; j < N*N; ++j)
	printf("%4d", C[j]);
	printf("\n");

	auto t = time_end - time_begin;
	printf("\n PARALLEL BLOCK TIME = %f seconds \n", t);

	return C;
}
/*_______________________________________________________________________________________________________*/


int main()
{
	printf("\nMatrix sizes (%1dx%1d): \n", N, N);
	
	auto A = GetRandomMatrixA();
	auto B = GetRandomMatrixB();

	/*std::cout << " Matrix A:\n";
	Print(A);
	std::cout << " \n Matrix B:\n";
	Print(B);*/

	for (int size = 1; size <= N; ++size)
	{
		if (N%size == 0)
		{
			printf("\nblock size: %d\n", size);

			int* AA = CreateBlockA(A, size);
			int* BB = CreateBlockB(B, size);

			// проверка верности ввода
			//PrintAB(AA, size, 'A');
			//PrintAB(BB, size, 'B');

			int* C = Multiplication(AA, BB, size);
			delete[] C;
			int* Cp = MultiplicationInParallelThreads(AA, BB, size);
			delete[] Cp;
			int* Cpb = ParallelMultiplicationForBlocks(AA, BB, size);
			delete[] Cpb;

			printf("\n");

			delete[] AA;
			delete[] BB;
		}
	}
	printf("\n");
	system("pause");
	return 0;
}

