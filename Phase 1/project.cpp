#include"util.h"
#include<iomanip>

int main()
{
	pthread_t readFileThread;
	matrix=new int*[9];
	for (int i=0; i<9; i++)
		matrix[i]=new int[9];
	pthread_create(&readFileThread,NULL,readFromFile,NULL); //reads the matrix in "matrix.txt" stored in .csv format
	pthread_t sectionsThreads[totalSectionsOfSudoku];
	int getSectionsArgs[totalSectionsOfSudoku];
	for (int i=0; i<totalSectionsOfSudoku; i++)
	{
		getSectionsArgs[i]=i;
		pthread_create(&sectionsThreads[i],NULL,getVertices,(void*)&getSectionsArgs[i]);
	}
	SectionData uniqueSections[totalSectionsOfSudoku];
	void* temp=NULL;
	pthread_join(readFileThread,&temp);
	for (int i=0; i<totalSectionsOfSudoku; i++)
	{
		pthread_join(sectionsThreads[i],&temp);
		uniqueSections[i].vertices=(int**)temp;
		pthread_create(&sectionsThreads[i]/*reusing array as getSection thread is terminated*/,NULL,getSum,(void*)&uniqueSections[i]);
	}
	int invalidCount=0;
	int invalidationReasons[2]={0};
	for (int i=0; i<totalSectionsOfSudoku; i++)
	{
		pthread_join(sectionsThreads[i],&temp);
		if (!temp)
		{
			invalidCount++;
			if (i<9)
				cout<<setw(20)<<left<<"Invalid Row";
			else if (i<18)
				cout<<setw(20)<<"Invalid Column";
			else
				cout<<setw(20)<<"Invalid Sub-grid";
			cout<<"Thread ID: "<<sectionsThreads[i];
			bool rPrinted=false;
			for (int j=0; j<9; j++)
			{
				if(uniqueSections[i].presentValues[j] > 1)
				{
					if (!rPrinted)
					{
						cout<<"\nFailure Reason: "<<failureReason[0];
						rPrinted=true;
					}
					cout<<endl<<"Digit: "<<j<<" ";
				}
			}
			if (uniqueSections[i].invalidEntries.size()>0)
				cout<<"\nFailure Reason: "<<failureReason[1];
			for (int j=0; j<uniqueSections[i].invalidEntries.size(); j++)
			{
				cout<<endl<<"Digit: "<<uniqueSections[i].invalidEntries[j]<<" ";
			}
			cout<<endl;
			invalidationReasons[uniqueSections[i].fReason]=1;
			cout<<setw(5)<<"Row"<<setw(5)<<"Col"<<setw(5)<<"Val"<<endl;
			for (int j=0; j<9; j++)
			{
				cout<<setw(5)<<uniqueSections[i].vertices[j][0]
					<<setw(5)<<uniqueSections[i].vertices[j][1]
					<<setw(5)<<matrix[uniqueSections[i].vertices[j][0]][uniqueSections[i].vertices[j][1]]<<endl;
			}
			cout<<"\n\n";
		}
		else
		{
			uniqueSections[i].fReason=-1; //no failure so no reason
		}
	}
	cout<<"Summary\nTotal Invalid Count: "<<invalidCount<<endl;
	if (invalidCount>0)
	{
		cout<<"Invalidation Reasons:\n";
		(invalidationReasons[0])?(cout<<failureReason[0]<<endl):(cout<<"");
		(invalidationReasons[1])?(cout<<failureReason[1]<<endl):(cout<<"");
	}
	else
	{
		cout<<"Sudoku is valid\n";
	}
	return 0;
}
