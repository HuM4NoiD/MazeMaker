#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// useful Variables
int grid; // total size of arrays
int SIZE; // user specified size
int cells; // total visitable cells
char *m; //maze array
int b[4] = {0,0,0,0}; //gives the block status while carving maze
int visitCount = 0; //to track visited cells while carving
int **visitSolve; //to track cells while solving

//-------------------------------------------------------------------------

// for filling array with wall characters ASCII 219
void initializeM(int n);

void displayM();

//allocate visitSolve 
void makeVisitSolve();

// To fill visitSolve to match contents of the maze array
void fillVisitSolve();

void printV();

// To set the visited status of neighbours of a given cell, 0 for available, 1 for not available 
void setBlocked(int x, int y);

// To check for dead ends at a cell position
int deadEnd(int x, int y);

// To get a random direction towards non visited neighbours, 0 for up, 1 for down, 2 for left, 3 for right
int getDirection(int x, int y);

// Main DFS carving function, calls itself recursively based on the direction from getDirection()
void carvePath(int x, int y);

//Solve the maze in temporary visited array
int dfsMaze(int x, int y);

//Apply the solution from scrap visited to actual maze
void getSolution();

//Test the functions
void main(){
	int i;
	printf("Enter a size : ");
	scanf("%d",&SIZE);
	initializeM(SIZE);
	srand(time(NULL));
	carvePath(1,1); //start carving at this postion as it is the entry
	
	makeVisitSolve();
	fillVisitSolve();    //fill visitSolve according to the newly carved array
	
	*(m + (0*grid) + 1) =  ' ';
	*(m + (2*SIZE*grid) + grid-2) = ' ';
	displayM(); printf("\n");
	FILE *f;
	f = fopen("genMaze.txt","w");
	if(f != NULL){
		fprintf(f, "%d\n", SIZE);
		for(int i = 0; i < grid; ++i){
			for(int j = 0; j < grid; ++j){
				if (*(m + (i*grid) + j) == ' ')
				{
					fprintf(f, "%c", *(m + (i*grid) + j));
				} else if (*(m + (i*grid) + j) == (char) 219) {
					fprintf(f, "\u2588");
				} else {
					fprintf(f, "\u2592");
				}
			}
			fprintf(f, "%c", '\n');
		}
		printf("Writing to file successful, saved to genMaze.txt\n");
	} else {
		printf("Error Writing to file\n");
	}
	fclose(f);

	dfsMaze(1,1);

	printf("\n");

	getSolution();
	*(m + ((2*SIZE-1)*grid) + grid-2) = (char) 177;
	displayM();
	
	f = fopen("solution.txt","w");
	if(f != NULL){
		fprintf(f, "%d\n", SIZE);
		for(int i = 0; i < grid; ++i){
			for(int j = 0; j < grid; ++j){
				if (*(m + (i*grid) + j) == ' ')
				{
					fprintf(f, "%c", *(m + (i*grid) + j));
				} else if (*(m + (i*grid) + j) == (char) 219) {
					fprintf(f, "\u2588");
				} else {
					fprintf(f, "\u2592");
				}
			}
			fprintf(f, "%c", '\n');
		}
		printf("Writing to file successful, saved to solution.txt\n");
	} else {
		printf("Error Writing to file\n");
	}
	fclose(f);

	free(m);
	for(i = 0; i < grid; ++i) free(visitSolve[i]);
}

//allocate visitSolve 
void makeVisitSolve(){
	visitSolve = (int**)malloc(grid * sizeof(int *));
	int i;
	for(i = 0; i < grid; ++i){
		visitSolve[i] = (int*) malloc(grid * sizeof(int));
	}
}

// for filling array with wall characters ASCII 219
void initializeM(int n){
	int i, j; 
	grid = 2*n+1;
	cells = n * n;
	m = (char *) malloc(grid*grid*sizeof(char));
	for(i = 0; i < grid; ++i){
		for(j = 0; j < grid; ++j){
			*(m + (i*grid) + j) = (char) 219;
		}
	} 
}

void displayM(){
	int i, j;
	for(i = 0; i<grid; ++i){
		for(j = 0; j<grid; ++j){
			printf("%c%c",*(m + (i*grid) + j),*(m + (i*grid) + j));
		}
		printf("\n");
	}
	printf("\n");
}

// To set the visited status of neighbours of a given cell, 0 for available, 1 for not available 
void setBlocked(int x, int y){
	int i;
	for(i = 0; i < 4; ++i) b[i] = 0;
	if(y == 1 || *(m + ((y-2)*grid) + x) == ' ') b[0] = 1;
	if(y == grid - 2 || *(m + ((y+2)*grid) + x) == ' ') b[1] = 1;
	if(x == 1 || *(m + (y*grid) + x - 2) == ' ') b[2] = 1;
	if(x == grid - 2 || *(m + (y*grid) + x + 2) == ' ') b[3] = 1;
}

// To get a random direction towards non visited neighbours, 0 for up, 1 for down, 2 for left, 3 for right
int getDirection(int x, int y){
	
	setBlocked(x,y);
	int r;
	r = rand() / (RAND_MAX / 4);
	while(1){
		if(b[r] == 0){
			break;
		} else {
			r = rand() % 4;
		}
	}
	return r;
}

// To check for dead ends at a cell position
int deadEnd(int x, int y){
	int i,flag = 0;
	setBlocked(x,y);
	for(i = 0; i < 4; ++i){
		if(b[i] == 0){
			flag = 0;
			break;
		} else {
			flag = 1;
		}
	}
	return flag;
}

// Main DFS carving function, calls itself recursively based on the direction from getDirection()
void carvePath(int x, int y){
	if(*(m + (y*grid) + x) != ' '){
		++visitCount;
	}
	*(m + (y*grid) + x) = ' ';
	if(visitCount != cells){
		while(deadEnd(x,y) == 0){
			int dir = getDirection(x,y);
			switch(dir) {
				case 0: *(m + ((y-1)*grid) + x) = ' '; carvePath(x,y - 2); break; //up case
				case 1: *(m + ((y+1)*grid) + x) = ' '; carvePath(x,y + 2); break; //down
				case 2: *(m + (y*grid) + x - 1) = ' '; carvePath(x - 2,y); break; //left
				case 3: *(m + (y*grid) + x + 1) = ' '; carvePath(x + 2,y); break; //right
			}
		}
	}
}

// To fill visitSolve to match contents of the maze array
void fillVisitSolve(){
	int i, j;
	for(i = 0; i < grid; ++i){
		for(j = 0; j < grid; ++j){
			if(*(m + (i*grid) + j) == (char) 219){ visitSolve[i][j] = 1; }
			else if(*(m + (i*grid) + j) == ' ') { visitSolve[i][j] = 0; }
		}
	}
}

/*
visitSolve has 3 states
0 : representing free path
1 : representing wall
3 : representing a mark left, ie visited
*/

//Solve the maze in temporary visited array
int dfsMaze(int x, int y){
	int *c = &visitSolve[y][x];
	if(x == grid-2 && y == grid-2){
		return 1;
	}
	if(*c == 0){
		*c = 1;  // if we fill this with a wall, other path deviations are suppressed in solving
		// as using dfs leads to may deviations
		// also while going in a paritcular direction, we mark it visited
		if(dfsMaze(x-1,y)){ *c = 3; return 1;}
		if(dfsMaze(x+1,y)){ *c = 3; return 1;}
		if(dfsMaze(x,y-1)){ *c = 3; return 1;}
		if(dfsMaze(x,y+1)){ *c = 3; return 1;}
	}
	return 0;
}

void printV(){ int i, j;
	for(i = 0; i < grid; ++i){
		for(j = 0; j < grid; ++j){
			printf("%d",visitSolve[i][j]);
		}
		printf("\n");
	}
}

//Apply the solution from scrap visited to actual maze
void getSolution(){
	int i, j;
	for(i = 0; i < grid ; ++i){
		for(j = 0; j < grid; ++j){
			if(visitSolve[i][j] == 3){
				*(m + (i*grid) + j) = (char) 177;
			}
		}
	}
	*(m + (0*grid) + 1) = (char) 177;
	*(m + (2*SIZE*grid) + grid-2) = (char) 177; 
}