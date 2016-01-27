#include "stdafx.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include "Matrix.h"
#include "time.h"


using namespace std;

int main(int argc, char * argv[])
{
	Matrix mat1, mat2,res;
	if (argc != 4)
	{
		cout << "Enter command arguments" << endl;
		return -1;
	}
	else
	{	
		try {
		Matrix objMatrix;
		objMatrix.readLoadFile(argv[1],mat1);
		objMatrix.readLoadFile(argv[2],mat2);
		cout << "matrix loaded \n";
		clock_t start, end;
		start = clock();
		objMatrix.paralell(mat1, mat2, res);	
		cout << "parallel \n "; 
		end = clock();
		printf(" %1.4f second(s)\n", ((double)end - start) / ((double)CLOCKS_PER_SEC));
		cout << "results are saved";
		objMatrix.writeInFile(argv[3],res); 
		}
		catch (char*str)
		{
			cout << str;
			return -1;
		}
	}
	return 0;
}