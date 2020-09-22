#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

template <class T>
void	readMatrix(FILE *f, vector <vector <T> > &matrix) {
	size_t N, M;

	fread(&N, sizeof(size_t), 1, f);
	fread(&M, sizeof(size_t), 1, f);
	matrix.resize(N);
	for (int i = 0; i < N; i++) {
		matrix[i].resize(M);
		for (int j = 0; j < M; j++)
			fread(&(matrix[i][j]), sizeof(T), 1, f);
	}
}

template <class T>
void	writeMatrix(char *file, size_t N, size_t M, vector <vector <T> > &matrixC) {
	FILE *f;

	f = fopen(file, "wb");
	char typeF = 'f', typeD = 'd';
	if (sizeof(matrixC[0][0]) == sizeof(float))
		fwrite(&typeF, sizeof(char), 1, f);
	else if (sizeof(matrixC[0][0]) == sizeof(double))
		fwrite(&typeD, sizeof(char), 1, f);
	fwrite(&N, sizeof(size_t), 1, f);
	fwrite(&M, sizeof(size_t), 1, f);
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
			fwrite(&(matrixC[i][j]), sizeof(T), 1, f);
	fclose(f);
}

template <class T>
void	algorithm(char *type, char *file, vector <vector <T> > matrixA, vector <vector <T> > matrixB) {
	vector <vector <T> > matrixC;

	matrixC.resize(matrixA.size());
	for (int i = 0; i < matrixC.size(); i++)
		matrixC[i].resize(matrixB[0].size());
	if (!strcmp(type, "ijk"))
		for (int i = 0; i < matrixC.size(); i++)
			for (int j = 0; j < matrixC[0].size(); j++)
				for (int k = 0; k < matrixB.size(); k++)
					matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
	else if (!strcmp(type, "ikj"))
		for (int i = 0; i < matrixC.size(); i++)
			for (int k = 0; k < matrixB.size(); k++)
				for (int j = 0; j < matrixC[0].size(); j++)
					matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
	else if (!strcmp(type, "kij"))
		for (int k = 0; k < matrixB.size(); k++)
			for (int i = 0; i < matrixC.size(); i++)
				for (int j = 0; j < matrixC[0].size(); j++)
					matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
	else if (!strcmp(type, "jik"))
		for (int j = 0; j < matrixC[0].size(); j++)
			for (int i = 0; i < matrixC.size(); i++)
				for (int k = 0; k < matrixB.size(); k++)
					matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
	else if (!strcmp(type, "jki"))
		for (int j = 0; j < matrixC[0].size(); j++)
			for (int k = 0; k < matrixB.size(); k++)
				for (int i = 0; i < matrixC.size(); i++)
					matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
	else if (!strcmp(type, "kji"))
		for (int k = 0; k < matrixB.size(); k++)
			for (int j = 0; j < matrixC[0].size(); j++)
				for (int i = 0; i < matrixC.size(); i++)
					matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
	writeMatrix(file, matrixC.size(), matrixC[0].size(), matrixC);
}

int main(int argc, char **argv)
{
	FILE *f;
	f = fopen(argv[1], "rb");
	char type;

	fread(&type, sizeof(char), 1, f);
	if (type == 'f') {
		vector <vector <float> > matrixA;
		vector <vector <float> > matrixB;
		readMatrix(f, matrixA);
		fclose(f);
		f = fopen(argv[2], "rb");
		fread(&type, sizeof(char), 1, f);
		readMatrix(f, matrixB);
		algorithm(argv[4], argv[3], matrixA, matrixB);
	} else if (type == 'd') {
		vector <vector <double> > matrixA;
		vector <vector <double> > matrixB;
		readMatrix(f, matrixA);
		fclose(f);
		f = fopen(argv[2], "rb");
		fread(&type, sizeof(char), 1, f);
		readMatrix(f, matrixB);
		algorithm(argv[4], argv[3], matrixA, matrixB);
	}
	fclose(f);
	return 0;
}