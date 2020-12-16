#include"sudokuFiller.h"
#include<iomanip>

int main()
{
	sem_init(&matrixMutualExclusion,0,1);
	pthread_t readFileThread;
	pthread_t displayMatrixThread;
	cout<<"Old Matrix:\n";
/*w*/	sem_wait(&matrixMutualExclusion);
	matrix=new int*[9];
	for (int i=0; i<9; i++)
		matrix[i]=new int[9];
/*p*/	sem_post(&matrixMutualExclusion);
	pthread_create(&readFileThread,NULL,readFromFile,NULL); //reads the matrix in "matrix.txt" stored in .csv format
	pthread_t sectionsThreads[totalSectionsOfSudoku];
	int getSectionsArgs[totalSectionsOfSudoku];
	for (int i=0; i<totalSectionsOfSudoku; i++)
	{
		getSectionsArgs[i]=i;
		pthread_create(&sectionsThreads[i],NULL,getVertices,(void*)&getSectionsArgs[i]);
	}
	pthread_join(readFileThread,NULL);
	pthread_create(&displayMatrixThread,NULL,displayMatrix,NULL);
	SectionData uniqueSections[totalSectionsOfSudoku];
	void* temp=NULL;
	pthread_join(displayMatrixThread,NULL);
	for (int i=0; i<totalSectionsOfSudoku; i++)
	{
		pthread_join(sectionsThreads[i],&temp);
		uniqueSections[i].vertices=(int**)temp;
		pthread_create(&sectionsThreads[i]/*reusing array as getSection thread is terminated*/,NULL,clearInvalid,(void*)&uniqueSections[i]);
//		pthread_join(sectionsThreads[i],NULL);
	}
	int invalidCount=0;
	for (int i=0; i<27/*checks row wise*//*totalSectionsOfSudoku*/; i++)
	{
		pthread_join(sectionsThreads[i],&temp);
	}
/*w*/	sem_wait(&matrixMutualExclusion);
	SolveSudoku(matrix);
/*p*/	sem_post(&matrixMutualExclusion);
	for (int i=0; i<9; i++)
	{
		for (int j=0; j<9; j++)
		{
			for (int k=0; k<threadsCreatedForEachInvalidEntry[i][j].size(); k++)
			{
				if (k==0)
				{
					cout<<"Invalid Entry was at ("<<i<<","<<j<<")\nThreads Created:\n";
					invalidCount++;
				}
				cout<<threadsCreatedForEachInvalidEntry[i][j][k]<<endl;
			}
		}
	}
	pthread_create(&displayMatrixThread,NULL,displayMatrix,NULL);
	pthread_join(displayMatrixThread,NULL);
	cout<<endl<<"Total Invalid Entries: "<<invalidCount<<endl;	
	return 0;
}
