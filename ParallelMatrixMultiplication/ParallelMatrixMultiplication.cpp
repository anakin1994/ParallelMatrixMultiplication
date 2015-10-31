// ParallelMatrixMultiplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <omp.h>

using namespace std;


#define N 500
#define RUNS 50

//global declarations
float a[N][N] = { 0 };
float b[N][N] = { 0 };
float c1[N][N] = { 0 };
float c2[N][N] = { 0 };
float c3[N][N] = { 0 };
float c4[N][N] = { 0 };

//check if 2 matrices are the same
bool checkTheSame(float m1[N][N], float m2[N][N]) {
	for (int a = 0; a < N; a++) {
		for (int b = 0; b < N; b++) {
			if (m1[a][b] != m2[a][b]) {
				cout << "Wynik niepoprawny !" << endl;
				return 0;
			}
		}
	}
	return 1;
}

void resetMatricies()
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			c1[i][j] = 0;
			c2[i][j] = 0;
			c3[i][j] = 0;
			c4[i][j] = 0;
		}
	}
}

time_t multiplyMatriciesSeq()
{
	//declare clocks
	clock_t start, stop;

	//start the clock

	start = clock();

	//multiply a and b and store the result in c
	for (int j = 0; j < N; j++) {
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N; k++) {
				c1[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	//stop the clock

	stop = clock();

	cout << "Czas przetwarzania sekwencyjnego wynosi " << ((double)(stop - start) / 1000.0) << " sekund." << endl;

	return stop - start;
}

time_t multiplyMatriciesPar1()
{
	//declare clocks
	clock_t start, stop;

	//start the clock

	start = clock();

#pragma omp parallel
	{
		// init loop variables
		int i, j, k;
		//multiply a and b and store the result in c
#pragma omp for
		for (j = 0; j < N; j++) {
			for (i = 0; i < N; i++) {
				for (k = 0; k < N; k++) {
					c2[i][j] += a[i][k] * b[k][j];
				}
			}
		}
	}

	//stop the clock
	stop = clock();

	cout << "Czas przetwarzania rownoleglego1 wynosi " << ((double)(stop - start) / 1000.0) << " sekund." << endl;

	return stop - start;
}

time_t multiplyMatriciesPar2()
{
	//declare clocks
	clock_t start, stop;

	//start the clock

	start = clock();

#pragma omp parallel
	{
		// init loop variables
		int i, j, k;

		//multiply a and b and store the result in c
		for (j = 0; j < N; j++) {
#pragma omp for
			for (i = 0; i < N; i++) {
				for (k = 0; k < N; k++) {
					c3[i][j] += a[i][k] * b[k][j];
				}
			}
		}
	}

	//stop the clock
	stop = clock();

	cout << "Czas przetwarzania rownoleglego2 wynosi " << ((double)(stop - start) / 1000.0) << " sekund." << endl;

	return stop - start;
}

time_t multiplyMatriciesPar3()
{
	//declare clocks
	clock_t start, stop;

	//start the clock

	start = clock();

#pragma omp parallel
	{
		// init loop variables
		int i, j, k;

		//multiply a and b and store the result in c
		for (j = 0; j < N; j++) {
			for (i = 0; i < N; i++) {
#pragma omp for
				for (k = 0; k < N; k++) {
					c4[i][j] += a[i][k] * b[k][j];
				}
			}
		}
	}

	//stop the clock
	stop = clock();

	cout << "Czas przetwarzania rownoleglego3 wynosi " << ((double)(stop - start) / 1000.0) << " sekund." << endl;

	return stop - start;
}

//print a matrix
void printMatrix(float m[N][N]) {
	cout << "Matrix:" << endl;
	for (int c = 0; c < N; c++) {
		for (int d = 0; d < N; d++) {
			cout << m[c][d] << " ";
		}
		cout << endl;
	}
}

int main()
{
	FILE *result_file;
	if ((fopen_s(&result_file, "results.csv", "a")) != 0) {
		fprintf(stderr, "nie mozna otworzyc pliku wyniku \n");
		perror("classic");
		return(EXIT_FAILURE);
	}

	time_t times[RUNS][4];
	for (int i = 0; i < RUNS; i++)
	{
		resetMatricies();
		//fill matrices with random numbers [0;1]
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				a[i][j] = (float)rand() / (RAND_MAX);
				b[i][j] = (float)rand() / (RAND_MAX);
			}
		}

		times[i][0] = multiplyMatriciesSeq();
		fprintf(result_file, "%d;", times[i][0]);

		times[i][1] = multiplyMatriciesPar1();
		fprintf(result_file, "%d;", times[i][1]);
		checkTheSame(c1, c2);

		times[i][2] = multiplyMatriciesPar2();
		fprintf(result_file, "%d;", times[i][2]);
		checkTheSame(c1, c3);

		times[i][3] = multiplyMatriciesPar3();
		fprintf(result_file, "%d\n", times[i][3]);
		checkTheSame(c1, c4);

	}

	//Count average times
	double  t0 = 0, t1 = 0, t2 = 0, t3 = 0;
	for (int i = 0; i < RUNS; i++)
	{

		t0 += times[i][0];
		t1 += times[i][1];
		t2 += times[i][2];
		t3 += times[i][3];
	}
	t0 /= (double)RUNS;
	cout << "Sredni czas przetwarzania sekwencyjnego: " << t0 / 1000.0 << " sekund" << endl;
	t1 /= (double)RUNS;
	cout << "Sredni czas przetwarzania rownoleglego1 : " << t1 / 1000.0 << " sekund" << endl;
	t2 /= (double)RUNS;
	cout << "Sredni czas przetwarzania rownoleglego2 : " << t2 / 1000.0 << " sekund" << endl;
	t3 /= (double)RUNS;
	cout << "Sredni czas przetwarzania rownoleglego3 : " << t3 / 1000.0 << " sekund" << endl;

	fclose(result_file);

	system("pause");
	return 0;
}

