#include<iostream>
#include<pthread.h>
#include<stdlib.h>
#include<fstream>
#include<vector>
#include<cstring>

using namespace std;

#define totalSectionsOfSudoku 27

int** matrix;

char* failureReason[2]={"Repeated Values", "Invalid Digits"};

enum SectionOfSudoku { row0, row1, row2, row3, row4, row5, row6, row7, row8, col0, col1, col2, col3, col4, col5, col6, col7, col8, square0, square1, square2, square3, square4, square5, square6, square7, square8 };

struct SectionData
{
	int** vertices;
	int sum;
	int fReason;
	int presentValues[9];
	vector<int> invalidEntries;
	SectionData():vertices(NULL),sum(0),fReason(0)
	{
		for (int i=0; i<9; i++) 
			presentValues[i]=0;
	}
};

int strTok(char* line, char delimeter, char**& toks)
{
	int nToks=1;
	for (int i=0; i<strlen(line); i++)
	{
		if (line[i]==delimeter)
			nToks++;
	}
	toks=new char* [nToks+1];
	int j=0; //index of toks being accessed
	char* temp=new char[strlen(line)];
	int k=0; //index of temp being accessed
	for (int i=0; i<strlen(line); i++)
	{
		if (line[i]==delimeter)
		{
			temp[k]=0;
			toks[j]=temp;
			j++;
			temp=new char[strlen(line)];
			k=0;
			continue;
		}
		temp[k++]=line[i];
	}
	temp[k]=0;
	toks[j]=temp;
	return nToks;
}
void* getVertices(void* a)
{//based on passed integer (enum), return int** with all included vertices
	int sec=*((int*)a);
	int** vertex=new int*[9];
	for (int i=0; i<9; i++)
	{
		vertex[i]=new int[2];
	}
	if (sec>=0 && sec<9)
	{//row
		for (int i=0; i<9; i++)
		{
			vertex[i][0]=sec;
			vertex[i][1]=i;
		}
	}
	else if (sec<18)
	{//column
		for (int i=0; i<9; i++)
		{
			vertex[i][1]=sec-9; //col no
			vertex[i][0]=i; //row no
		}
	}
	else if (sec<27)
	{//square
		vertex[0][0]=(sec-18)/3;
		vertex[0][1]=((sec-18)%3)*3;
		int k=0;
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++,k++)
			{
				vertex[k][0]=((sec-18)/3)*3 + i;
				vertex[k][1]=((sec-18)%3)*3+j;
			}
		}
	}
	pthread_exit((void*) vertex);
}
int toInt(char* a)
{
	int b=0;
	bool neg=false;
	if (a[0]=='-')
	{
		neg=true;
		a++;
	}
	while (strlen(a)>0)
	{
		b=b*10+a[0]-'0';
		a++;
	}
	if (neg)
		b*=-1;
	return b;
}
void* readFromFile(void* m) //reads the matrix from the file "matrix.txt"
{
	ifstream in("matrix.txt");
	for (int i=0; i<9; i++)
	{
		char* a=new char[100];
		in>>a;
		char** toks;
		strTok(a, ',', toks);
		for (int j=0; j<9; j++)
		{
			matrix[i][j]=toInt(toks[j]);
		}
		delete [] a;
	}
}
void* getSum(void* args)
{
	SectionData* sec=(SectionData*) args;
	sec->sum=0;
	for (int i=0; i<9; i++)
	{
		sec->sum+=matrix[sec->vertices[i][0]][sec->vertices[i][1]];
		if (matrix[sec->vertices[i][0]][sec->vertices[i][1]] < 0 || matrix[sec->vertices[i][0]][sec->vertices[i][1]] > 9)
		{
			sec->fReason=1;
			sec->invalidEntries.push_back(matrix[sec->vertices[i][0]][sec->vertices[i][1]]);
		}
		else
			sec->presentValues[matrix[sec->vertices[i][0]][sec->vertices[i][1]]]++;
	}
	if (sec->sum != 45)
		pthread_exit((void*)NULL);
	pthread_exit((void*)!NULL);
}
