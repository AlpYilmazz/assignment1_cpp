#include <iostream>
#include <cstdio>
#include <fstream>

using namespace std; 

struct Cell{
	char type;
	int center_r;
	int center_c;
	int size;
};

struct Instruction{
	char direction[3];
	int cell_r;
	int cell_c;
	int amount;
};

typedef Cell cell;
typedef Instruction moveby;

bool border_err(cell *node, int rows, int cols);
bool conflict_err(cell *node, cell **grid, int rows, int cols);
void place(cell *node, cell **grid, char empty);
void remove_sign(cell **grid, int r, int c);

int main(int argc, char* argv[]){
	char* grid_file = argv[1];
	char* intruction_file = argv[2];
	//char grid_file[] = "grid.txt";
	//char intruction_file[] = "instructions.txt";
	ifstream fget;
	cell **grid;
	
	int i, j;
	int rows, cols;
	
	fget.open(grid_file);
	if(!fget.is_open()){
		cerr << "File could not be opened." << endl;
		return 1;
	}
	
	fget >> rows >> cols;
	grid = new cell*[rows+1];
	for(i = 0; i < rows+1; i++){
		grid[i] = new cell[cols+1];
	}
	
	for(i = 1; i <= rows; i++){
		for(j = 1; j <= cols; j++){
			(grid[i][j]).type = '.';
		}
	}
	printf("A grid is created: %d %d\n", rows, cols);
	
	while(1){
		bool err = false;
		cell node;
		if(!(fget >> node.type >> node.center_r >> node.center_c >> node.size)) break;
		if(border_err(&node, rows, cols)){
			printf("BORDER ERROR: Operator %c with size %d can not be placed on (%d,%d).\n",
					node.type, node.size, node.center_r, node.center_c);
			err = true;
		}
		if(conflict_err(&node, grid, rows, cols)){
			printf("CONFLICT ERROR: Operator %c with size %d can not be placed on (%d,%d).\n",
					node.type, node.size, node.center_r, node.center_c);
			err = true;
		}
		if(err) continue;
		place(&node, grid, 'n');
		printf("SUCCESS: Operator %c with size %d is placed on (%d,%d).\n",
				node.type, node.size, node.center_r, node.center_c);
	}

	fget.close();
	fget.open(intruction_file);
	if(!fget.is_open()){
		cerr << "File could not be opened." << endl;
		return 1;
	}
	
	while(1){
		bool err = false;
		moveby move;
		cell node;
		cell node_keep;
		if(!(fget >> move.direction >> move.cell_r >> move.cell_c >> move.amount)) break;
		node_keep = grid[move.cell_r][move.cell_c];
		node = grid[move.cell_r][move.cell_c];
		remove_sign(grid, move.cell_r, move.cell_c);
		if(move.direction[2] == 'L' || move.direction[2] == 'l'){
			node.center_c -= move.amount;
		}
		else if(move.direction[2] == 'R' || move.direction[2] == 'r'){
			node.center_c += move.amount;
		}
		else if(move.direction[2] == 'U' || move.direction[2] == 'u'){
			node.center_r -= move.amount;
		}
		else if(move.direction[2] == 'D' || move.direction[2] == 'd'){
			node.center_r += move.amount;
		}

		if(border_err(&node, rows, cols)){
			printf("BORDER ERROR: %c can not be moved from (%d,%d) to (%d,%d).\n",
					node.type, node_keep.center_r, node_keep.center_c, node.center_r, node.center_c);
			err = true;
		}
		if(conflict_err(&node, grid, rows, cols)){
			printf("CONFLICT ERROR: %c can not be moved from (%d,%d) to (%d,%d).\n",
					node.type, node_keep.center_r, node_keep.center_c, node.center_r, node.center_c);
			err = true;
		}

		if(err){
			place(&node_keep, grid, 'n');
			continue;
		}
		place(&node, grid, 'n');
		printf("SUCCESS: %c moved from (%d,%d) to (%d,%d).\n",
				node_keep.type, node_keep.center_r, node_keep.center_c, node.center_r, node.center_c);
	}
	
	fget.close();
	
	for(i = 0; i < rows; i++){
		delete[] grid[i];
	}
	delete[] grid;
	
	return 0;
}

/**
 * Checks whether there is a border error or not
 *
 * @param node center node of a sign which will be put into the grid
 * @param rows number of rows in the grid
 * @param cols number of colums in the grid
 * @return true if there is an border error, false if not
*/

bool border_err(cell *node, int rows, int cols){
	if(node->type == '+' || node->type == 'x' || node->type == '/'){
		if(node->center_r + node->size > rows
			|| node->center_r - node->size < 1
			|| node->center_c + node->size > cols
			|| node->center_c - node->size < 1){
			return true;
		}
	}
	else if(node->type == '-'){
		if(node->center_c + node->size > rows
			|| node->center_c - node->size < 1){
			return true;
		}
	}
	return false;
}

/**
 * Checks whether there is a conflict error or not
 *
 * @param node center node of a sign which will be put into the grid
 * @param grid the grid that signs will be put in
 * @param rows number of rows in the grid
 * @param cols number of colums in the grid
 * @return true if there is an conflict error, false if not
*/

bool conflict_err(cell *node, cell **grid, int rows, int cols){
	int i;
	if(node->type == '+'){
		for(i = -(node->size); i <= node->size; i++){
			if(node->center_r + i >= 1 && node->center_r + i <= rows){
				if(grid[node->center_r + i][node->center_c].type != '.'){
					return true;
				}
			}
			if(node->center_c + i >= 1 && node->center_c + i <= cols){
				if(grid[node->center_r][node->center_c + i].type != '.'){
					return true;
				}
			}
		}
	}
	else if(node->type == '-'){
		for(i = -(node->size); i <= node->size; i++){
			if(node->center_c + i >= 1 && node->center_c + i <= cols){
				if(grid[node->center_r][node->center_c + i].type != '.'){
					return true;
				}
			}
		}
	}
	else if(node->type == 'x'){
		for(i = -(node->size); i <= node->size; i++){
			if(node->center_r + i >= 1 && node->center_r + i <= rows
				&& node->center_c + i >= 1 && node->center_c + i <= cols){
				if(grid[node->center_r + i][node->center_c + i].type != '.'){
					return true;
				}
			}
			if(node->center_r + i >= 1 && node->center_r + i <= rows
				&& node->center_c - i >= 1 && node->center_c - i <= cols){
				if(grid[node->center_r + i][node->center_c - i].type != '.'){
					return true;
				}
			}
		}
	}
	else if(node->type == '/'){
		for(i = -(node->size); i <= node->size; i++){
			if(node->center_r + i >= 1 && node->center_r + i <= rows
				&& node->center_c - i >= 1 && node->center_c - i <= cols){
				if(grid[node->center_r + i][node->center_c - i].type != '.'){
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * Places the sign into the grid
 * 
 * @param node center node of a sign which will be put into the grid
 * @param grid the grid that signs will be put in
 * @param empty '.' if an empty sign is to be put, 'n' if not
 * @return no return
*/

void place(cell *node, cell **grid, char empty){
	int i;
	if(node->type == '+'){
		if(empty == '.') node->type = empty;
		for(i = -(node->size); i <= node->size; i++){
			grid[node->center_r + i][node->center_c] = *node;
			grid[node->center_r][node->center_c + i] = *node;
		}
	}
	else if(node->type == '-'){
		if(empty == '.') node->type = empty;
		for(i = -(node->size); i <= node->size; i++){
			grid[node->center_r][node->center_c + i] = *node;
		}
	}
	else if(node->type == 'x'){
		if(empty == '.') node->type = empty;
		for(i = -(node->size); i <= node->size; i++){
			grid[node->center_r + i][node->center_c + i] = *node;
			grid[node->center_r + i][node->center_c - i] = *node;
		}
	}
	else if(node->type == '/'){
		if(empty == '.') node->type = empty;
		for(i = -(node->size); i <= node->size; i++){
			grid[node->center_r + i][node->center_c - i] = *node;
		}
	}
}

/**
 * Removes a sign from the grid by overwriting the sign with empty nodes
 * 
 * @param grid the grid that signs will be put in
 * @param r the row value of the node that is given to be moved
 * @param c the column value of the node that is given to be moved
 * @return no return
*/

void remove_sign(cell **grid, int r, int c){
	if(grid[r][c].type == '.') return; // this case was not spesified so it is empty
	place(&(grid[r][c]), grid, '.');
}
