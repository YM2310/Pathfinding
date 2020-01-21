#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <Windows.h>

#define ROWS 22
#define COLS 22
#define GAMESPEED 50
char board[ROWS][COLS];
// cooredinates on map
typedef struct place {
	int row;
	int col;
}PLACE;
//tree figure, contains coordinates and growth status (1- young 't', 2- old 'T')
typedef struct tree {
	PLACE place;
	char status;
}TREE;

typedef struct path {
	PLACE place;
	int distance;
	int stepnum;
	struct path* next;
	struct path* previous;
}PATH;

typedef struct man {
	PLACE place;
	int inventory;
	char inventory_type;
	PATH* currentpath;
}MAN;



//create a map
//generate villigares
//collect resources
//build a house
void initBoard();
void printBoard();
MAN generateMap(PLACE* house);
void genTrees();
void placeBoard(PLACE place, char symbol);
void gotoxy(PLACE place);
void init(void);
int* randomSample(int size, int originsize);
PLACE genHouse();
MAN genMan(PLACE house);
PATH* newNode(PLACE place);
void leaveHouse(MAN* man);
void moveMan(MAN* man, int direction);
void removeCurs();
PATH* findPath(PLACE current, char target, PLACE tree);
PLACE findCloseTree(PLACE man);
PLACE checkSquare(PLACE man, char target, int layer);
PATH* removeHead(PATH* head);
void addAdjacent(PLACE* adjacent, PLACE current);
PATH* getNode(PATH* path, PLACE place);
PATH* reversePath(PATH* path);
void removeNode(PATH* path, PATH* remove);
bool checkIfInList(PATH* path, PLACE check);
void addNodelast(PATH* head, PLACE place);
void addNodeFfirst(PATH** head, PATH* new);
void followPath(MAN* man, PATH* path);
void goHome(MAN* man, PATH* path);
void collectAll(MAN* man, PATH* path);

int main() {
	for (int i = 0; i < 100; i++) {
		time_t t;
		srand((unsigned)time(&t));
		init();
		initBoard();
		printBoard();
		MAN man;
		PLACE house;
		PATH* path;
		PLACE tree;
		man = generateMap(&house);
		do {
			tree = findCloseTree(man.place);
			if (tree.col != 0 || tree.row != 0) {
				path = findPath(man.place, 'T', tree);
				if (path == NULL) {
					printf("NULL");
					Sleep(1000);
					break;
				}
				path = reversePath(path);

				collectAll(&man, path);
				//goHome(&man, path);
			}
		} while (tree.col != 0 || tree.row != 0);
		printf("NO TREES");
		Sleep(1000);
		PLACE place;
		place.row = ROWS + 1;
		place.col = COLS + 1;
		gotoxy(place);
	}
}
void goHome(MAN* man, PATH* path) {
	{
		while (path->next != NULL) {
			path = path->next;
		}
		while(path!=NULL){
			Sleep(GAMESPEED);
			placeBoard(man->place, ' ');
			removeCurs();
			man->place = path->place;
			placeBoard(man->place, 'O');
			removeCurs();
			path = path->previous;
		}
	}
}

PATH* findPath(PLACE current, char target, PLACE tree) {
	
	PATH* currentstep=newNode(current);
	int distance;
	int step = 0;
	bool pathfound = FALSE;
	distance= abs(tree.col- current.col)+abs(tree.row - current.row);
	PATH* open = newNode(current);
	open->distance = distance;
	open->stepnum = step;
	PATH* closed = newNode(current);
	PATH* shortest = open;
	PATH* temp;
	PLACE* adjacent = (PLACE*)calloc(sizeof(PLACE), 4);
	int count = 0;
	do {
		count++;
		currentstep = open;		
		//placeBoard(currentstep->place, currentstep->stepnum + 48);
		addNodelast(closed, currentstep->place);
		open = open->next;
		//open=removeHead(open);
		if (currentstep->place.row == tree.row && currentstep->place.col == tree.col||board[currentstep->place.row][currentstep->place.col]=='T') {
			pathfound = true;
			break;
		}
		addAdjacent(adjacent, currentstep->place);
		
		for (int i = 0; i < 4; i++) {
			if (adjacent[i].col != 0 && adjacent[i].row != 0) {
				if (checkIfInList(closed, adjacent[i])) {
					adjacent[i].col = 0;
					adjacent[i].row = 0;
				}
				else
				{
					PATH* newpath = newNode(adjacent[i]);
					newpath->distance = abs(tree.col - adjacent[i].col) + abs(tree.row - adjacent[i].row);
					newpath->stepnum = currentstep->stepnum+1;
					newpath->previous = currentstep;
					if (checkIfInList(open, adjacent[i]) == false) {
						addNodeFfirst(&open, newpath);
					}
					else {
						PATH* compare = getNode(open, adjacent[i]);
						if ((newpath->distance + newpath->stepnum) < (compare->distance + compare->stepnum)) {
							removeNode(open, compare);
							addNodeFfirst(&open, newpath);
						}
					}
				}
			}
	
		}

		//init the adjacent arr
		for (int i = 0; i < 4; i++) {
			adjacent[i].col = 0;
			adjacent[i].row = 0;
		}

	} while (open != NULL);
	if (pathfound == false) {
		return NULL;
	}
	else {
		return currentstep;
	}
}

PATH* reversePath(PATH* path) {
	PATH* collect = newNode(path->place);
	while (path != NULL) {
		addNodelast(collect, path->place);
		path = path->previous;
	}
	while (collect->next != NULL) {
		collect = collect->next;
	}
	PATH* temp = newNode(collect->place);
	PATH* newpath = temp;
	while (collect->previous!=NULL) {
		collect = collect->previous;
		addNodelast(temp, collect->place);
		temp = temp->next;


	}
	return newpath;
}
void removeNode(PATH* path, PATH* remove) {
	PATH* temp;
	while (path->next != NULL) {
		if (path->next->place.col == remove->place.col && path->next->place.row == remove->place.row) {
			temp = path->next;
			path->next = path->next->next;
			free(temp);
			break;
		}
		else {
			path = path->next;
		}
	}
}


PATH* getNode(PATH* path, PLACE place) {
	while (path != NULL) {
		if (path->place.col == place.col && path->place.col == place.col) {
			return path;
		}
		else {
			path = path->next;
		}
	}
	return NULL;
}

bool checkIfInList(PATH* path, PLACE check) {
	while (path != NULL) {
		if (path->place.row == check.row && path->place.col == check.col) {
			return true;
		}
		else {
			path = path->next;
		}
	}
	return false;
}

void addAdjacent(PLACE* adjacent, PLACE current) {
	PLACE temp;
	temp = current;
	temp.row--;
	int i = 0;
	if (board[temp.row][temp.col] == 'T'|| board[temp.row][temp.col] == ' ') {
		adjacent[i] = temp;
		i++;
	}

	temp = current;
	temp.col++;
	if (board[temp.row][temp.col] == 'T' || board[temp.row][temp.col] == ' ') {
		adjacent[i] = temp;
		i++;
	}

	temp= current;
	temp.row++;
	if (board[temp.row][temp.col] == 'T' || board[temp.row][temp.col] == ' ') {
		adjacent[i] = temp;
		i++;
	}

	temp = current;
	temp.col--;
	if (board[temp.row][temp.col] == 'T' || board[temp.row][temp.col] == ' ') {
		adjacent[i] = temp;
		i++;
	}
}
 
PLACE findCloseTree(PLACE man) {
	int i = 1;
	PLACE tree;
	while (i < ROWS*2) {
		tree = checkSquare(man, 't', i);
		if (tree.col == man.col && tree.row == man.row) {
			i++;
		}
		else {
			break;
		}
	}
	if (i == ROWS * 2) {
		tree.col = 0;
		tree.row = 0;
		return tree;
	}

	return tree;
}

PLACE checkSquare(PLACE man, char target, int i) {
	PLACE check;
	check = man;
	check.row -= i;
	bool tag = false;
	for (int j = 0; j < i; j++){
		if (check.col > 0 && check.row > 0) {
			if (board[check.row][check.col] == 'T') {
				man = check;
				tag = true;
				break;
			}
		}

			check.col++;

			check.row++;

		
		
	}
	if (tag == false) {
		for (int j = 0; j < i; j++) {
			if (check.col > 0 && check.row > 0) {
				if (board[check.row][check.col] == 'T') {
					man = check;
					tag = true;
					break;
				}
			}

				check.col--;

				check.row++;

			
		}
	}
	if (tag == false) {
		for (int j = 0; j < i; j++) {
			if (check.col > 0 && check.row > 0) {
				if (board[check.row][check.col] == 'T') {
					man = check;
					tag = true;
					break;
				}
			}


				check.col--;

				check.row--;

			
		}
	}
	if (tag == false) {
		for (int j = 0; j < i; j++) {
			if (check.col > 0 && check.row > 0) {
				if (board[check.row][check.col] == 'T') {
					man = check;
					tag = true;
					break;
				}
			}

				check.col++;
				check.row--;

				
			
		}
	}
	return man;
}

void leaveHouse(MAN* man) {
	for(int i=0;i<3;i++)
	moveMan(man, 6);
}

void followPath(MAN* man, PATH* path)
{
	while (path != NULL) {

		Sleep(GAMESPEED);
		placeBoard(man->place, ' ');
		removeCurs();
		man->place = path->place;
		placeBoard(man->place, 'O');
		removeCurs();
		path = path->next;
	}
}

void collectAll(MAN* man, PATH* path)
{
	PLACE tree;
	while (path != NULL) {
		//for (int i = 0; i <2; i++) {
			Sleep(GAMESPEED);
			placeBoard(man->place, ' ');
			removeCurs();
			man->place = path->place;
			placeBoard(man->place, 'O');
			removeCurs();
			path = path->next;
		//}
		tree = findCloseTree(man->place);
		if (tree.col != 0 || tree.row != 0) {
			path = findPath(man->place, 'T', tree);
			if (path == NULL) {
				break;
			}
			path = reversePath(path);
			path = path->next;
		}
		else {
			break;
		}
	}
}

void moveMan(MAN* man,int direction) {
	placeBoard(man->place, ' ');
	switch (direction)
	{
	case 12: {
		man->place.row--;
		break;
	}

	case 3: {
		man->place.col++;
		break;
	}

	case 6: {
		man->place.row++;
		break;
	}

	case 9: {
		man->place.col--;
		break;
	}
	default:
		break;
	}
	placeBoard(man->place, 'O');
	removeCurs();
}

PATH* newNode(PLACE place) {
	PATH* new = (PATH*)malloc(sizeof(PATH));
	if (new == NULL) {
		printf("MALLOC ERROR");
	}
	else {
		new->previous = NULL;
		new->next = NULL;
		new->place = place;
		new->distance = 0;
		new->stepnum = 0;
		return new;
	}
}

void addNodelast(PATH* head, PLACE place) {
	PATH* new;
	if (head != NULL) {
		while (head->next != NULL) {
			head = head->next;
		}
	}
	new = newNode(place);
	head->next = new;
	new->previous = head;
}

void addNodeFfirst(PATH** head, PATH* new) {
	//adds node by distance (small to large)
	PATH* temp_head = *head;
	PATH* temp;
	{
		if ((*head) == NULL || (*head)->distance + (*head)->stepnum > new->distance + new->stepnum) {
			new->next = *head;
			*head = new;
		}
		else {
			temp = (*head);
			while (temp->next != NULL && temp->next->distance + temp->next->stepnum < new->distance + new->stepnum) {
				temp = temp->next;
			}
			new->next = temp->next;
			temp->next = new;
		}
	}
}




PATH* removeHead(PATH* head) {
	PATH* temp = head;

	head = head->next;
	free(temp);

	return head;
}

MAN generateMap(PLACE* house) {
	int i = 0;
	genTrees();
	PLACE test;
	*house=genHouse();
	MAN man=genMan(*house);
	removeCurs();
	return man;
	
}

void removeCurs() {
	PLACE place;
	place.row = ROWS + 1;
	place.col = COLS + 1;
	gotoxy(place);
}

MAN genMan(PLACE house) {
	MAN man;
	house.row++;
	house.col += 2;
	placeBoard(house, 'O');
	man.place = house;
	man.inventory = 0;
	man.inventory_type = 'N';
	man.currentpath = NULL;
	return man;
}

PLACE genHouse() {
	PLACE center,print;
	center.col = COLS / 2;
	center.row = ROWS / 2;
	print.row = center.row - 1;
	print.col = center.col - 2;
	for (int i = 0; i < 4; i++){
		if (i == 0||i==3) {
			if (i == 0) {
				for (int j = 0; j < 5; j++) {
					placeBoard(print, 'H');
					print.col++;
				}
			}
			if (i == 3) {
				for (int j = 0; j < 5; j++) {
					placeBoard(print, 'H');		
					if (j == 2) {
						placeBoard(print, ' ');
					}
					print.col++;
				}
				print.col -= 5;
				print.row++;
				for (int j = 0; j < 5; j++) {
					placeBoard(print, ' ');
					print.col++;
				}
			}
		}
		else {
			for (int j = 0; j < 5; j++) {
				if (j == 0 || j == 4) {
					placeBoard(print, 'H');
					print.col++;
				}
				else {
					placeBoard(print, ' ');
					print.col++;
				}
			}
		}
		print.col -= 5;
		print.row++;
	}
	print.row = center.row - 1;
	print.col = center.col - 2;
	return print;
}

void genTrees() {
	int* arr;
	int quarter = 5;
	int totalquarters = pow(((ROWS - 2) / quarter), 2);
	PLACE* quarters_arr = (PLACE*)calloc(sizeof(PLACE), totalquarters);
	PLACE place;
	TREE cent_tree;
	int k = 0;
	for (int i = 1; i < ROWS - 2; i += ROWS / 4) {
		place.row = i;
		for (int j = 1; j < COLS - 2; j += COLS / 4) {
			place.col = j;
			quarters_arr[k] = place;
			k++;
		}
	}

	for (int j = 0; j < 4; j++) {
		arr = randomSample((int)totalquarters * 0.75, totalquarters);
		for (int i = 0; i < 10; i++) {
			place.row = rand() % (4);
			place.col = rand() % (4);
			place.row += quarters_arr[arr[i]].row;
			place.col += quarters_arr[arr[i]].col;
			placeBoard(place, 'T');
		}

		arr = randomSample((int)totalquarters * 0.75, totalquarters);
		for (int i = 0; i < 10; i++) {
			place.row = rand() % (4);
			place.col = rand() % (4);
			place.row += quarters_arr[arr[i]].row;
			place.col += quarters_arr[arr[i]].col;
			placeBoard(place, 'B');
			place.col++;
			placeBoard(place, 'B');
		}
	}
}



int* randomSample(int size, int originsize) {
	int* arr = (int*)calloc(sizeof(int), size);
	int k = 0;
	int temp;
	for (int i = originsize-size+1; i < originsize; i++) {
		temp = rand() % (i - 1);
		temp++;
		for (int j = 0; j < size; j++) {
			if (arr[j] == temp) {
				temp = i;
				break;
			}
		}
		arr[k] = temp;
		k++;
	}
	return arr;
}

void placeBoard(PLACE place, char symbol) {
	board[place.row][place.col] = symbol;
	gotoxy(place);
	printf("%c", symbol);
}

void initBoard()
{

	int i, j;

	for (i = 0; i < ROWS; i++)
	{
		if (i == 0 || i == ROWS - 1) {
			for (j = 0; j < COLS; j++)
			{
				board[i][j] = '#';
			}
		}
		else {


			for (j = 0; j < COLS; j++)
			{
				if (j == 0 || j == COLS - 1)
				{
					board[i][j] = '#';
					board[i][j] = '#';
				}
				else {
					board[i][j] = ' ';
				}
			}
		}
	}
}

void printBoard() {

	for (int i = 0; i < ROWS; i++) {

		for (int j = 0; j < COLS; j++) {
			printf("%c ", board[i][j]);
		}
		printf("\n");

	}
}

void init(void) {
	system("cls");
}

void gotoxy(PLACE place) {
	int y, x;
	x =place.row+1;
	if (place.col == 1) {
		y = 3;
	}
	else {
		if (place.col == 2) {
			y = 5;
		}
		else {
			y = 2 * place.col + 1;
		}
	}
	printf("\x1b[%d;%df", x, y);
}