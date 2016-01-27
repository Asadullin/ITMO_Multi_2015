#include "stdafx.h"
#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <omp.h>
#include <sstream>
#include <string>

using namespace std;


void Matrix::paralell(Matrix &mat, Matrix &mat1, Matrix &res)
{
	int ij, i, j;
	//res.Mat.resize(mat.Mat.size());
	res.Mat.resize(1);
	if (mat.Mat[0].size() != mat1.Mat.size())
	{
		throw "size matrix error ";
	}
		for (int a = 0; a < 1; a++)
	{
		res.Mat[a].resize(2000 * 2000);
	}
		for (int i = 0; i < 1; i++)
		{
			for (int j = 0; j < 2000 * 2000; j++)
			{
				
				res.Mat[i][j] = rand() % 100;
					
			}
		}
	/*for (int a = 0; a < mat.Mat.size(); a++)
		{
			res.Mat[a].resize(mat1.Mat[0].size());
		}

#pragma omp parallel
	{
#pragma omp for private (ij , i, j) schedule(guided, 1)
		for (ij = 0; ij < mat.Mat.size() * mat1.Mat[0].size(); ij++)
		{				
				i = ij / mat1.Mat[0].size();
				j = ij % mat1.Mat[0].size();		
			float sum = 0;
			for (int k = 0; k < mat.Mat[0].size(); ++k)
			{
				sum += mat.Mat[i][k] * mat1.Mat[k][j];
			}
			res.Mat[i][j] = sum;
			sum = 0;
		}
	}*/
}

void Matrix::readLoadFile(char *str, Matrix & mat)
{
	ifstream in(str);
	string buf;
	float a;
	vector<float> vec;
	int i = 0;
	while (getline(in, buf))
	{
		vec.clear();
		stringstream ss(buf);
		while (ss >> a)
		{
				if (ss.good() || ss.eof())
				{
					vec.push_back(a);
				}
				else
				{
					throw "incorrect data";
				}
		}
		i++;
		mat.Mat.push_back(vec);
	}
}

void Matrix::writeInFile(char *str, Matrix &mat )
{

	ofstream infile(str);
		if (infile.is_open())
		{
			for (int i = 0; i < mat.Mat.size(); i++)
			{
				//infile << "\n";
				for (int j = 0; j < mat.Mat[0].size(); j++)
				{
					infile << mat.Mat[i][j] << " ";
				}
				infile << endl;
			}
			infile.close();
		}
		else
		{
			throw "Can't open file";
		}
}