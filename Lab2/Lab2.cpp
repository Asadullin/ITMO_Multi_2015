#define _CRT_SECURE_NO_WARNINGS 
#include "stdafx.h"
#include <iostream> 
#include <cmath> 
#include <fstream> 
#include <mpi.h> 
#include <time.h> 

using namespace std;

int id;
int sizes;
class Jacobi
{
public:
	int N;
	double eps;
	double *X;
	Jacobi(char* f1, char* f2, char* epss)
	{
		eps = std::atof(epss);
		readMatrix(f1);
		readStarts(f2);
	}
	~Jacobi()
	{
		delete[] A;
		delete[] F;
		delete[] X;
	}
private:
	double *A;
	double *F;
	void init(int n)
	{
		N = n;
		if (id == 0) {
			A = new double[n*n];
			F = new double[n];
		}
		X = new double[n];
	}

	double * run(int N, double* A, double* F, double* X)
	{
		double norm, max;
		int RowNum = N / sizes;
		int AddCount = N%sizes;
		int *sendcnts, *displs;
		displs = (int*)malloc(sizes*sizeof(int));
		sendcnts = (int*)malloc(sizes*sizeof(int));
		displs[0] = 0;
		int i, i2;

		for (i2 = 0; i2<sizes; i2++)
		{
			sendcnts[i2] = RowNum;
			if (i2 == sizes - 1)
				sendcnts[i2] += AddCount;
		}

		for (i = 1; i<sizes; i++)
		{
			displs[i] = displs[i - 1] + sendcnts[i - 1];
		}

		//int RowNum = N / sizes; 
		/*
		if (N%sizes != 0)
		{
		cout « "error" «endl;
		throw exception("oops");
		}
		*/
		double* pProcRowsA;
		double* pProcRowsF;
		double* pProcTempX;
		double * tX;
		pProcRowsA = new double[RowNum*N];
		pProcRowsF = new double[RowNum];
		pProcTempX = new double[RowNum];
		tX = new double[N];
		MPI_Bcast(X, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Scatter(A, RowNum*N, MPI_DOUBLE, pProcRowsA, RowNum*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Scatter(F, RowNum, MPI_DOUBLE, pProcRowsF, RowNum, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		do {
			for (int i = 0; i < RowNum; i++) {
				pProcTempX[i] = 0;
				for (int j = 0; j < N; j++) {
					if ((id*RowNum + i) != j) {
						pProcTempX[i] += pProcRowsA[i*N + j] * X[j];
					}
				}
				pProcTempX[i] = (pProcRowsF[i] - pProcTempX[i]) / pProcRowsA[id*RowNum + i*N + i];
			}

			norm = fabs(X[id*RowNum] - pProcTempX[0]);
			for (int i = 0; i < RowNum; i++) {
				if (fabs(X[id*RowNum + i] - pProcTempX[i]) > norm)
					norm = fabs(X[id*RowNum + i] - pProcTempX[i]);
			}

			MPI_Reduce(&norm, &max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
			MPI_Bcast(&max, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			//MPI_Allgatherv(pProcTempX, RowNum, MPI_DOUBLE, X, RowNum, MPI_DOUBLE, MPI_COMM_WORLD); 
			MPI_Allgatherv(pProcTempX, RowNum, MPI_DOUBLE, X, sendcnts, displs, MPI_DOUBLE, MPI_COMM_WORLD);
		} while (max > eps);
		delete[] pProcTempX;
		delete[] pProcRowsA;
		delete[] pProcRowsF;
		return X;
	}
	void readMatrix(string fname)
	{
		ifstream file(fname);
		int n, m;
		file >>m; 
		file>> n;
		init(m);
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (j == (n - 1)) file>> F[i];
				else file>> A[i*N + j];
			}
		}

		file.close();
	}

	void readStarts(string fname)
	{
		ifstream file(fname);
		int m;
		file >>m;
		for (int i = 0; i < m; i++)
		{
			file>> X[i];
		}
		file.close();
	}
public:
	void start()
	{
		this->X = run(N, A, F, X);
	}
};

int main(int argc, char* argv[]) {
//	cout<< "afasfsaf-1"<<endl;
	time_t begin, end;
	int ui = 0;
	char* f1 = "d:\\1.txt";
	char* f2 = "d:\\2.txt";
	char* f3 = "0.001";
//	if (argc != 5) return 0;

	time(&begin);
	//cout<< "afasfsaf0"<<endl;
	Jacobi j("d:\\1.txt", "d:\\2.txt", "0.001");

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &sizes);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
//	cout<< "afasfsaf1" << endl;

	try
	{
		j.start();
	}
	catch (exception e) {
		cout<< "Error: "<< e.what()<< endl;
	}
	//cout<<"afasfsaf2" << endl;
	ofstream out("d:\\3.txt");
	if (id == 0) for (int i = 0; i < j.N; i++) out << j.X[i]<< endl;
	out.close();
	MPI_Finalize();
//	cout << "afasfsaf3"<<endl;

	time(&end);
	cout<< end - begin<<endl;
	return 1;
}