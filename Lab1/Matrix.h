#pragma once
#include <vector>
#include <fstream>
#include <exception>
#include <string>

using namespace std;

class Matrix
{
	typedef vector <vector<float>> MatrixType;
public:
	Matrix() {}
	void paralell(Matrix & mat, Matrix & mat1, Matrix & res);
	void readLoadFile(char *str, Matrix & mat);
	void writeInFile(char *str, Matrix &mat);	
private:
	MatrixType Mat;
};

