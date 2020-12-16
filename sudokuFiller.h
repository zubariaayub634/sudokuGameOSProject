#include "util.h"

#define UNASSIGNED 0  

struct CheckSafeArguments
{
	int ** grid;
	int num;
	int row;
	int col;
	int boxStartRow;
	int boxStartCol;
	CheckSafeArguments(int** grid, int num, int row, int col, int boxStartRow, int boxStartCol)
	{
		this->grid=grid;
		this->num=num;
		this->row=row;
		this->col=col;
		this->boxStartRow=boxStartRow;
		this->boxStartCol=boxStartCol;
	}
};

bool FindUnassignedLocation(int** grid,  
                            int &row, int &col)  
{  
	for (row = 0; row < N; row++)  
		for (col = 0; col < N; col++)  
			if (grid[row][col] == UNASSIGNED)  
				return true;  
	return false;
}  

void* UsedInRow(void* args)  
{
	CheckSafeArguments* a = (CheckSafeArguments*) args;
	for (int col = 0; col < N; col++)  
		if (a->grid[a->row][col] == a->num)  
			return (void*)!NULL;  
	return (void*)NULL;
}

void* UsedInCol(void* args)  
{  
	CheckSafeArguments* a = (CheckSafeArguments*) args;
	for (int row = 0; row < N; row++)  
		if (a->grid[row][a->col] == a->num)  
			return (void*)!NULL;  
	return (void*)NULL;  
}

void* UsedInBox(void* args)  
{
	CheckSafeArguments* a = (CheckSafeArguments*) args;
	for (int row = 0; row < 3; row++)  
		for (int col = 0; col < 3; col++)  
			if (a->grid[row + a->boxStartRow] 
				[col + a->boxStartCol] == a->num)  
				return (void*)!NULL;  
	return (void*)NULL;
}

bool isSafe(int** grid, int row,  
                   int col, int num)  
{
	CheckSafeArguments* c=new CheckSafeArguments(grid,num, row,col,row - row % 3,col - col % 3);
	void* args=(void*)&c;
	pthread_t threadIds[3];
	pthread_create(&threadIds[0],NULL,UsedInBox,c);
	pthread_create(&threadIds[1],NULL,UsedInCol,c);
	pthread_create(&threadIds[2],NULL,UsedInRow,c);
	bool res=grid[row][col] == UNASSIGNED;
	for (int i=0; i<3; i++)
	{
		void* t;
		threadsCreatedForEachInvalidEntry[row][col].push_back(threadIds[i]);
		pthread_join(threadIds[i],&t);
		res=res & !(bool)t;
	}
	return res;
}  

bool SolveSudoku(int** grid)  
{  
    int row, col;  
  
    // If there is no unassigned location, 
    // we are done
    if (!FindUnassignedLocation(grid, row, col))  
    return true; // success!  
  
    // consider digits 1 to 9  
    for (int num = 1; num <= 9; num++)  
    {  
        // if looks promising  
        if (isSafe(grid, row, col, num))  
        {  
            // make tentative assignment  
            grid[row][col] = num;  
  
            // return, if success, yay!  
            if (SolveSudoku(grid))  
                return true;  
  
            // failure, unmake & try again  
            grid[row][col] = UNASSIGNED;  
        }  
    }
    return false; // this triggers backtracking  
}
