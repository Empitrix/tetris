#include <stdarg.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "src/utils.h"
#include "src/types.h"



#define WIDTH 400
#define HEIGHT 500
#define COLS 10
#define ROWS 20
#define MAX_BLKS (COLS * ROWS)
#define BACKGROUND (Color){ 25, 25, 25, 255 }
#define BS 25
#define FPS 60
#define N 4



static int fps_cntr = 0;
static int callback_duration = 10;  // each 10 MS
static block_t table[ROWS][COLS] = { 0 };
static block_t tmp[ROWS][COLS] = { 0 };
static int shape_idx = 0;
static int next_shape_idx = 0;








void calc_center(int *px, int *py){
	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].movement){
				*px = *px + x;
				*py = *py + y;
			}
		}
	}
	*px = *px / N;
	*py = *py / N;
}



void drawRect(Vector2 pos, Color clr, Color outline){
	DrawRectangle(pos.x * BS, pos.y * BS, BS, BS, clr);
	DrawRectangleLines((pos.x * BS) + 1, (pos.y * BS) + 1, BS - 1, BS - 1, outline);
}


void draw_table(){
	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].set || table[y][x].movement){
				drawRect(V2(x, y), table[y][x].color, BLACK);
			} else {
				if(table[y][x].reflect){
					drawRect(V2(x, y), BACKGROUND, WHITE);
				} else {
					drawRect(V2(x, y), BACKGROUND, BLACK);
				}
			}
		}
	}
}




// void rotate(int matrix[N][N]) {
// 	// Rotate the matrix 90 degrees clockwise in place
// 	for (int i = 0; i < N / 2; i++) {
// 		for (int j = i; j < N - i - 1; j++) {
// 			// Save the current element
// 			int temp = matrix[i][j];
// 
// 			// Move values from right to top
// 			matrix[i][j] = matrix[N - j - 1][i];
// 
// 			// Move values from bottom to right
// 			matrix[N - j - 1][i] = matrix[N - i - 1][N - j - 1];
// 
// 			// Move values from left to bottom
// 			matrix[N - i - 1][N - j - 1] = matrix[j][N - i - 1];
// 
// 			// Move saved top value to left
// 			matrix[j][N - i - 1] = temp;
// 		}
// 	}
// }




void rotate_matrix(int cx, int cy, int size){
	int px = 0;
	int py = 0;
	calc_center(&px, &py);
}


int move_down(){

	int reached_down = 0;

	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].movement && ((y == ROWS - 1) || (table[y + 1][x].set))){
				reached_down = 1;
			}
		}
	}

	if(reached_down){
		for(int x = 0; x < COLS; x++){
			for(int y = 0; y < ROWS; y++){
				if(table[y][x].movement){
					table[y][x].movement = 0;
					table[y][x].set = 1;
				}
			}
		}
		return 1;

	} else {
		for(int x = 0; x < COLS; x++){
			for(int y = ROWS - 1; y >= 0; y--){
				if(table[y][x].movement){
					table[y][x].movement = 0;
					table[y + 1][x].movement = 1;
					table[y + 1][x].color = table[y][x].color;
				}
			}
		}
	}

	return 0;
}


int any_row_set(int y_set){
	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].set && y == y_set){
				return 1;
			}
		}
	}
	return 0;
}

int movement_y_fill(int y_set){
	int m = 0;
	block_t moving[N] = { 0 };
	// for(int x = 0; x < COLS; x++){
	// 	for(int y = 0; y < ROWS; y++){
	// 		if(table[y][x].set && y == y_set){
	// 			return 1;
	// 		}
	// 	}
	// }

	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].movement){
				moving[m++] = table[y][x];
			}
		}
	}

	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){

			if(table[y][x].set && y == y_set){
				return 1;
				// for(int i = 0; i < N; i++){
				// 	if(moving[i].x == x && moving[i].y == y_set){
				// 		return 1;
				// 	}
				// }
			}

			// if(table[y][x].set && y == y_set){
			// 	return 1;
			// }
		}
	}
	return 0;
}


void reflect_down() {
	int min_dist = ROWS;

	// Find the minimum distance any moving block can fall
	for(int x = 0; x < COLS; x++){
		for(int y = ROWS - 1; y >= 0; y--){
			if(table[y][x].movement) {
				for(int i = 0; i < ROWS; i++){
					int v = y + i;
					// v = v + (shape_idx == 6 ? 1 : 0);
					if(v == ROWS - 1 || table[v + 1][x].set){
						if(i < min_dist){
							min_dist = i;
						}
						break;
					}
				}
			}
		}
	}

	// Apply the reflection to the predicted landing position
	for (int x = 0; x < COLS; x++) {
		for (int y = 0; y < ROWS; y++) {
			if (table[y][x].movement) {
				table[y + min_dist][x].reflect = 1;
			}
		}
	}
}

void reflect_down2(){

	// int dist = 0;
	// for(int x = COLS - 1; x >= 0; x--){
	// 	for(int y = ROWS - 1; y >= 0; y--){
	// 		if(table[y][x].movement){
	// 			for(int i = 0; i < ROWS; i++){
	// 				int v = i + y;
	// 				// int at_max = v == ROWS - (shape_idx == 6 ? 2 : 1);
	// 				v = v + (shape_idx == 6 ? 1 : 0);
	// 				int at_max = v == ROWS - 1;
	// 				// if(at_max || table[v + 1][x].set){
	// 				if(at_max || movement_y_fill(v + 1)){
	// 					dist = i;
	// 					goto loop_out;
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	int distA = 0;
	int distB = 0;

	int dist = 0;

	for(int x = COLS - 1; x >= 0; x--){
		for(int y = ROWS - 1; y >= 0; y--){
			if(table[y][x].movement){
				for(int i = 0; i < ROWS; i++){
					int v = i + y;
					v = v + (shape_idx == 6 ? 1 : 0);
					int at_max = v == ROWS - 1;
					if(at_max || table[v + 1][x].set){
						distA = i;
						goto first_loop_out;
					}
				}
			}
		}
	}
first_loop_out:


	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].movement){
				for(int i = 0; i < ROWS; i++){
					int v = i + y;
					v = v + (shape_idx == 6 ? 1 : 0);
					int at_max = v == ROWS - 1;
					if(at_max || table[v + 1][x].set){
						distB = i;
						goto second_loop_out;
					}
				}
			}
		}
	}
second_loop_out:


	// dist = distB + 1;
	dist = distA;

	// if(distB > distA){
	// 	dist = distB;
	// } else {
	// 	dist = distA;
	// }
	// dist = distB;
	printf("%d\n", dist);

	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].movement){
				table[y + dist][x].reflect = 1;
			}
		}
	}

}




void import_shape(){
	shape_idx = next_shape_idx;
	next_shape_idx = randint(0, MAX_SHAPES - 1);
	Color clr = shape_clr[shape_idx];
	// printf("#%X%X%X\n", clr.r, clr.g, clr.b);

	int bp = (COLS / 2) - 1;

	for(int i = 0; i < 4; i++){
		if(shapes[shape_idx].c[i]){
			table[0][bp + i].movement = 1;
			table[0][bp + i].color = clr;
		}
	}
	move_down();
	for(int i = 0; i < 4; i++){
		if(shapes[shape_idx].b[i]){
			table[0][bp + i].movement = 1;
			table[0][bp + i].color = clr;
		}
	}
	move_down();
	for(int i = 0; i < 4; i++){
		if(shapes[shape_idx].a[i]){
			table[0][bp + i].movement = 1;
			table[0][bp + i].color = clr;
		}
	}


}




int callback(){
	printf("Callback !\n");
	int hit = move_down();

	if(hit){
		import_shape();
	}

	return hit;
}



void move_right(){
	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].movement && x == COLS -1){
				return;
			}
		}
	}

	for(int x = COLS - 2; x >= 0; x--){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].movement){
				table[y][x].movement = 0;
				table[y][x + 1].movement = 1;
				table[y][x + 1].color = table[y][x].color;
			}
		}
	}
}



void move_left(){
	for(int x = 0; x < COLS; x++) {
		for(int y = 0; y < ROWS; y++) {
			if(table[y][x].movement && x == 0){
				return;
			}
		}
	}

	for (int x = 1; x < COLS; x++) {
		for (int y = 0; y < ROWS; y++) {
			if (table[y][x].movement) {
				table[y][x].movement = 0;
				table[y][x - 1].movement = 1;
				table[y][x - 1].color = table[y][x].color;
			}
		}
	}
}



void draw_next_shape() {
	int bx = 11, by = 5;
	const shape *shape = &shapes[next_shape_idx];
	for(int x = 0; x < SHAPE_W; x++){
		for(int y = 0; y < SHAPE_H; y++){
			if((y == 2 && shape->c[x]) ||
				(y == 1 && shape->b[x]) ||
				(y == 0 && shape->a[x])){
				drawRect(V2(bx + x, by + y), shape_clr[next_shape_idx], BLACK);
			}
		}
	}
}



void rotate(void) {
	int min_x = COLS, min_y = ROWS, max_x = 0, max_y = 0;
	
	// Find the bounding box of the moving blocks
	for (int x = 0; x < COLS; x++) {
		for (int y = 0; y < ROWS; y++) {
			if (table[y][x].movement) {
				if (x < min_x) min_x = x;
				if (y < min_y) min_y = y;
				if (x > max_x) max_x = x;
				if (y > max_y) max_y = y;
			}
		}
	}
	
	int size = (max_x - min_x > max_y - min_y) ? (max_x - min_x + 1) : (max_y - min_y + 1);
	block_t temp[size][size];
	memset(temp, 0, sizeof(temp));
	
	// Copy blocks into a temporary matrix
	for (int x = min_x; x <= max_x; x++) {
		for (int y = min_y; y <= max_y; y++) {
			if (table[y][x].movement) {
				int new_x = y - min_y;
				int new_y = size - 1 - (x - min_x);
				temp[new_y][new_x] = table[y][x];
			}
		}
	}
	
	// Apply the rotated blocks back to the table
	for (int x = min_x; x <= max_x; x++) {
		for (int y = min_y; y <= max_y; y++) {
			table[y][x] = (block_t){0}; // Clear original blocks
		}
	}
	
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			if (temp[y][x].movement) {
				table[min_y + y][min_x + x] = temp[y][x];
			}
		}
	}
}


int main(void){
	SetTraceLogLevel(LOG_NONE);
	InitWindow(WIDTH, HEIGHT, "Tetris");
	SetTargetFPS(FPS);


	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			table[y][x].x = x;
			table[y][x].y = y;
		}
	}

	shape_idx = randint(0, MAX_SHAPES - 1);
	import_shape();

	// table[2][2].movement = 1;
	// table[2][2].color = WHITE;

	// table[1][1].movement = 1;
	// table[1][1].color = WHITE;

	// table[2][1].movement = 1;
	// table[2][1].color = WHITE;

	// table[2][0].movement = 1;
	// table[2][0].color = WHITE;

	// next_shape_idx = randint(0, MAX_SHAPES - 1);

	while (!WindowShouldClose()){


		BeginDrawing();
		ClearBackground(BLACK);

		if(fps_cntr == callback_duration){
			callback();
			fps_cntr = 0;
		}




		drawText(V2(280, 50), 30, WHITE, "%d, [%d]", fps_cntr, next_shape_idx);


		reflect_down();
		draw_table();
		draw_next_shape();

		EndDrawing();



		// Exit
		if(IsKeyPressed(KEY_Q)){ break; }
		if(IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)){ move_right(); }
		if(IsKeyPressed(KEY_LEFT)  || IsKeyPressedRepeat(KEY_LEFT)){  move_left();  }

		if(IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)){
			rotater();
			fps_cntr = 0;

			// rotate_submatrix_clockwise(px, py, 3);
			// rotate_tetromino();
			// rotate_matrix(2, 2, 5);
		}

		if(IsKeyPressed(KEY_SPACE)){
			int hit_status = 0;
			while(hit_status == 0){
				hit_status = callback();
			}
		}



	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			table[y][x].reflect = 0;
		}
	}


		fps_cntr++;
	}


	CloseWindow();
	return 0;
}

