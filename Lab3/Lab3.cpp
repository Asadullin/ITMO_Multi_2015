#include "stdafx.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream> 
#include <vector>
#include <string>
#include <sstream>
using namespace std;


void read(vector<int> &vals, ifstream &fin)
{
	string str, item;
	stringstream ss, itemsStrm;
	float num;
	getline(fin, str);
	ss.str(str);
	while (getline(ss, item, ' '))
	{
		if (item.empty())
		{
			continue;
		}
		itemsStrm.clear();
		itemsStrm.str(item);
		itemsStrm >> num;
		if (!ss.fail() || ss.eof())
		{
			vals.push_back(num);
		}
	}
}
int getPivot(int* array, int size) {
	if (size == 0) {
		return 0;
	}
	return array[size / 2];
}

int sort(const void *x, const void *y) {
	return (*(int*)x - *(int*)y);
}
int main(int argc, char * argv[])
{
	double time, start;
	int Procnum, id, recv, pivot;
	vector<int> mat, Vals, theirs, tmp;
	int i = 0;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	/*All for the sending / recieving */
	MPI_Comm_size(MPI_COMM_WORLD, &Procnum);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	ifstream fin("d:\\1.txt");
	read(mat, fin);
	int size = mat.size();
	theirs.resize(size + Procnum);
	tmp.resize(size + Procnum);
	int N = size / Procnum;
	int nprocWorld = 0;
	int iprocWorld = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &nprocWorld);
	MPI_Comm_rank(MPI_COMM_WORLD, &iprocWorld);
	if (iprocWorld == 0) {
		time = MPI_Wtime();
	}

	for (int i = N*id; i < N*id + N; i++) {
		Vals.push_back(mat[i]);
	}
	qsort(&Vals[0], N, sizeof(int), sort);
	/*Create the communicator for use throughout*/
	MPI_Comm comm;
	MPI_Comm_split(MPI_COMM_WORLD, 1, id, &comm);
	int group = Procnum;
	start = 0;
	int numOfIter = 0;
	if (group == 1) {
		ofstream fout("d:\\2.txt");
		for (int i = 0; i < size; i++) {
			fout << Vals[i] << " ";
		}
		fout.close();
	}

	while (group>1) {
		MPI_Comm_size(comm, &Procnum);
		MPI_Comm_rank(comm, &id);

		pivot = getPivot(&Vals[0], N);
		if (id == 0) {
			for (i = 1; i<Procnum; i++) {
				MPI_Recv(&recv, 1, MPI_INT, i, 0, comm, &status);
				tmp[i] = recv;
			}
			tmp[0] = pivot;
			qsort(&tmp[0], Procnum, sizeof(int), sort);
			pivot = tmp[(Procnum / 2)];
			for (i = 1; i<Procnum; i++) {
				MPI_Send(&pivot, 1, MPI_INT, i, 0, comm);
			}
		}
		else {
			MPI_Send(&pivot, 1, MPI_INT, 0, 0, comm);
			MPI_Recv(&pivot, 1, MPI_INT, 0, 0, comm, &status);
		}
		int send = 0;
		for (i = 0; i<N; i++) {
			if (id < Procnum / 2) {
				if (Vals[i] >= pivot) {
					tmp[send] = Vals[i];
					send++;
				}
			}
			else {
				if (Vals[i] < pivot) {
					tmp[send] = Vals[i];
					send++;
				}
			}
		}
		//smalls send first
		if (id < Procnum / 2) {
			//send the size then the values
			MPI_Send(&send, 1, MPI_INT, id + (Procnum / 2), 0, comm);
			MPI_Send(&tmp[0], send, MPI_INT, id + (Procnum / 2), 0, comm);
			//we recieve the two
			MPI_Recv(&recv, 1, MPI_INT, id + (Procnum / 2), 0, comm, &status);
			MPI_Recv(&theirs[0], recv, MPI_INT, id + (Procnum / 2), 0, comm, &status);
		}
		else {
			//we recieve the two
			MPI_Recv(&recv, 1, MPI_INT, id - (Procnum / 2), 0, comm, &status);
			MPI_Recv(&theirs[0], recv, MPI_INT, id - (Procnum / 2), 0, comm, &status);
			//send the size then the values
			MPI_Send(&send, 1, MPI_INT, id - (Procnum / 2), 0, comm);
			MPI_Send(&tmp[0], send, MPI_INT, id - (Procnum / 2), 0, comm);
		}
		//now we combine the theirs and what is left of ours.
		if (id<Procnum / 2) {
			N -= send;

			for (i = 0; i<recv; i++) {
				Vals[N] = theirs[i];
				N++;
			}
		}
		else {
			int off = 0;
			for (i = 0; i<N; i++) {
				if (Vals[i] >= pivot) {
					theirs[recv + off] = Vals[i];
					off++;
				}
			}
			vector<int> temp = Vals;
			Vals = theirs;
			theirs = temp;
			N = recv + (N - send);
		}
		qsort(&Vals[0], N, sizeof(int), sort);
		group /= 2;
		if (group <= 1) {
			if (iprocWorld == 0) {
				int curProcSize = 0;
				int allProcSize = N;
				for (int i = 1; i < nprocWorld; i++) {
					MPI_Recv(&curProcSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
					MPI_Recv(&theirs[0], curProcSize, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
					int cur = 0;
					for (int j = allProcSize; j < allProcSize + curProcSize; j++) {
						Vals[j] = theirs[cur];
						cur++;
					}
					allProcSize = allProcSize + curProcSize;
				}
			}
			else {
				MPI_Send(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				MPI_Send(&Vals[0], N, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
			MPI_Barrier(MPI_COMM_WORLD);
			if (iprocWorld == 0) {
				ofstream fout("d:\\2.txt");
				for (int i = 0; i < size; i++) {
					fout << Vals[i] << " ";
				}
				fout.close();
			}
		}
		//reset the size of the group
		MPI_Comm_split(comm, id < Procnum / 2, id, &comm);
	}	
	if (iprocWorld == 0) {
		time = MPI_Wtime() - time;
		cout << time << endl;
	}
	MPI_Finalize();
	return 0;
}
