#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <raylib.h>
#include <string.h>


#define V2(x, y) (Vector2){x, y}
#define R(x, y, w, h) (Rectangle){x, y, w, h}

#define ScreenWidth 400
#define ScreenHeight 500

#define AREA_W 10
#define AREA_H 20

#define MAX_BLKS (AREA_W * AREA_H)

#define BS 25
#define FPS 60
#define N 4


int clampValue(int *var, int min, int max, int type){
	if(type){
		if((*var) <= max){ (*var)++; } else { return 1; }
	} else {
		if((*var) >= min){ (*var)--; } else { return 1; }
	}
	return 0;
}


void sort(int arr[], int n){
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - 1 - i; j++) {
			if (arr[j] > arr[j + 1]) {
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}



void drawText(Vector2 pos, int font_size, Color color, char *fmt, ...){
	static char vout_buffer[256] = { 0 };
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(vout_buffer, fmt, arg_ptr);
	DrawText(vout_buffer, pos.x, pos.y, font_size, color);
	va_end(arg_ptr);
}


void rotate90(int mat[N][N]) {
	int res[N][N] = {0}; 

	// Flip the matrix clockwise using nested loops
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			res[j][N - i - 1] = mat[i][j];
		}
	}

	// Copy result back to mat
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			mat[i][j] = res[i][j];
		}
	}
}


// static int shapes[7][N][N] = {
// 	{ {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0} }, // I
// 	{ {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} }, // J
// 	{ {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} }, // L
// 	{ {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} }, // O
// 	{ {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0} }, // Z
// 	{ {0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0} }, // T
// 	{ {0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0} }, // S
// };

// static int shapes[7][N][N] = {
// 	{ {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },  // I
// 	{ {0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0} },  // J
// 	{ {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0} },  // L
// 	{ {1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },  // O
// 	{ {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },  // Z
// 	{ {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },  // T
// 
// 	// {
// 	// 	{0, 1, 0, 0},
// 	// 	{1, 1, 1, 0},
// 	// 	{0, 0, 0, 0},
// 	// 	{0, 0, 0, 0}
// 	// },
// 
// 	// {
// 	// 	{0, 1, 0, 0},
// 	// 	{1, 1, 0, 0},
// 	// 	{1, 1, 0, 0},
// 	// 	{0, 0, 0, 0}
// 	// },
// 	{ {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} },  // S
// };

// static int shapes[7][N][N] = {
// 	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1} },  // I
// 	{ {0, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0} },  // J
// 	{ {0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0} },  // L
// 	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0} },  // O
// 	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1} },  // Z
// 	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 1} },  // T
// 	{ {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0} },  // S
// };


typedef enum {
	Shape_I,
	Shape_J,
	Shape_L,
	Shape_O,
	Shape_Z,
	Shape_T,
	Shape_S,
} shape_t;


typedef enum {
	Direction_RIGTH,
	Direction_DOWN,
	Direction_LEFT,
	Direction_UP,
} direction_t;



static int cshape[N][N] = { 0 };


void drawRect(Vector2 pos, Color clr, Color outline){
	DrawRectangle(pos.x * BS, pos.y * BS, BS, BS, clr);
	DrawRectangleLines((pos.x * BS) + 1, (pos.y * BS) + 1, BS - 1, BS - 1, outline);
	// DrawRectangleLines(pos.x * BS, pos.y * BS, BS, BS, outline);
}


static Color shape_colors[7] = { YELLOW, BLUE, RED, GREEN, ORANGE, PURPLE, VIOLET };

// void drawShape(Vector2 pos, shape_t shape, direction_t direction){
// 	memcpy(cshape, shapes[shape], N * N * N);
// 
// 	Color outline_colors[7] = { BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };
// 
// 
// 	for(int i = 0; i < (int)direction; i++){ rotate90(cshape); }
// 
// 	for(int x = 0; x < N; x++){
// 		for(int y = 0; y < N; y++){
// 
// 			if(cshape[y][x] == 1){
// 				drawRect(V2(pos.x + (x), pos.y + (y)), shape_colors[shape], outline_colors[shape]);
// 
// 			}
// 		}
// 	}
// }


static int _c_shp = 0;
static int _c_dir = 0;
static int _c_col = 5;
static int _c_row = 0;


typedef struct {
	int set;
	int movement;
	Color color;
	int x;
	int y;
} block_t;

// static block_t buffer[AREA_H][AREA_W] = { 0 };
// static block_t buffer[MAX_BLKS + AREA_W + AREA_W + AREA_W] = { 0 };
static block_t buffer[MAX_BLKS] = { 0 };
static int fps_cntr = 0;


static int shapes[7][N] = {
	{ 0, 1,  2,  3  },  // I
	{ 0, 10, 19, 20 },  // J
	{ 0, 10, 20, 21 },  // L
	{ 0, 1,  10, 11 },  // O
	{ 0, 1,  11, 12 },  // Z
	{ 0, 9,  10, 11 },  // T
	{ 0, 1,  9,  10 },  // S
};

static int _c_shp_bf[N] = { 0 };


int randint(int min, int max) {
	return min + (rand() % (max - min + 1));
}


int contains(int arr[], int size, int item){
	for(int i = 0; i < size; i++){
		if(arr[i] == item){
			return 1;
		}
	}
	return 0;
}


int move_down(){
	int points_y[N] = { 0 };
	int points_x[N] = { 0 };

	int points_filled_y[AREA_W] = { 0 };
	int points_filled_x[AREA_H] = { 0 };

	int cntr = 0;
	for(int i = 0; i < MAX_BLKS; i++){ if(buffer[i].movement){ points_y[cntr++] = buffer[i].y; }}

	cntr = 0;
	for(int i = 0; i < MAX_BLKS; i++){ if(buffer[i].movement){ points_x[cntr++] = buffer[i].x; }}



	cntr = 0;

	for(int i = 0; i < MAX_BLKS; i++){
		if(buffer[i].set && contains(points_filled_y, AREA_W, buffer[i].y) == 0){
			points_filled_y[cntr++] = buffer[i].y;
		}
	}

	cntr = 0;

	for(int i = 0; i < MAX_BLKS; i++){
		if(buffer[i].set && contains(points_filled_x, AREA_H, buffer[i].x) == 0){
			points_filled_x[cntr++] = buffer[i].x;
		}
	}



	sort(points_y, N);
	sort(points_x, N);
	sort(points_filled_x, AREA_H);
	sort(points_filled_y, AREA_W);


	for(int i = 0; i < AREA_W; i++){
		printf("%d, ", points_filled_y[i]);
	}
	printf("Y\n");

	for(int i = 0; i < AREA_H; i++){
		printf("%d, ", points_filled_x[i]);
	}
	printf("X\n");


	// if((points_filled_y[AREA_W - 1] == points[N - 1] + 2) && contains(points_filled_x, AREA_H, _c_col)){

	
	int cntis = 0;
	for(int i = 0; i < N; i++){
		cntis = contains(points_filled_x, AREA_H, points_x[i] + 1);
		if(cntis){ break; }
	}

	int cntis2 = 0;
	for(int i = 0; i < N; i++){
		cntis2 = contains(points_filled_y, AREA_W, points_y[i] + 1);
		if(cntis2){ break; }
	}

	// if(points_filled_y[AREA_W - 1] == points_y[N - 1] + 1 && cntis){
	// if(contains(points_filled_y, AREA_W, points_y[N - 1] + 1) && cntis){

	if(cntis2 && cntis){
		printf("ALRET! -------------------->\n");
	}


	// int corss = contains(points_filled_y, AREA_W, points_y[N - 1] + 1) && cntis;
	int corss = cntis2 && cntis;

	// int can_break = (points_filled_y[AREA_W - 1] == points[N - 1] + 2) && contains(points_filled_x, AREA_H, _c_col);



	// if((points[N - 1] - 1 < AREA_H - 2) && (points_filled_y[AREA_W - 1] != points[N - 1] + 2)){

	// if(points_y[N - 1] - 1 < AREA_H - 2){
	if((points_y[N - 1] - 1 < AREA_H - 2) && corss == 0){
		_c_row++;
	} else {

		for(int i = 0; i < MAX_BLKS; i++){
			if(buffer[i].movement){ buffer[i].set = 1; }
		}

		return 1;
	}


	// if(points[N - 1] - 1 < AREA_H - 2){
	// 	_c_row++;
	// } else {
	// 	return 1;
	// }

	return 0;
}


void callback(){
	printf("Callback !\n");
	// int hit = clampValue(&_c_row, 0, AREA_H - 2, 1);
	int hit = move_down();

	if(hit){


		_c_shp = randint(0, 6);
		memcpy(_c_shp_bf, shapes[_c_shp], sizeof(_c_shp_bf));

		_c_row = 0;
		_c_col = (int)AREA_W / 2;
	}
}




// static int callback_duration = 100;  // 10 MS
static int callback_duration = 20;  // 10 MS


int main(void){
	SetTraceLogLevel(LOG_NONE);
	InitWindow(ScreenWidth, ScreenHeight, "Tetris");
	SetTargetFPS(FPS);


	_c_shp = randint(0, 6);
	memcpy(_c_shp_bf, shapes[_c_shp], sizeof(_c_shp_bf));



	for(int i = 0; i < MAX_BLKS; i++){
		buffer[i].x = i % AREA_W;
		buffer[i].y = (int)(i / AREA_W);
	}


	// buffer[125].set = 1;

	while (!WindowShouldClose()){




		BeginDrawing();
		ClearBackground(BLACK);


		// int out = 0;
		// for(int i = 0; i < MAX_BLKS; i++){
		// 	drawRect(V2(i % AREA_W, out), (Color){ 25, 25, 25, 255 }, BLACK);
		// 	if(((i + 1) % (AREA_W)) == 0){
		// 		out++;
		// 	}
		// }



		// int posX = (_c_row + 2) * (AREA_W + 1);
		// int posX = (_c_row + 2) * (AREA_W + 1);
		// int posX = _c_row;

		int posX = _c_col + (_c_row * AREA_W);


		// int shape_i[N] = { 0, 1,  2,  3  };  // I
		// int shape_j[N] = { 0, 10, 19, 20 };  // J
		// int shape_l[N] = { 0, 10, 20, 21 };  // L
		// int shape_o[N] = { 0, 1,  10, 11 };  // O
		// int shape_z[N] = { 0, 1,  11, 12 };  // Z
		// int shape_t[N] = { 0, 9,  10, 11 };  // T
		// int shape_s[N] = { 0, 1,  9,  10 };  // S



		for(int i = 0; i < N; i++){
			buffer[posX + _c_shp_bf[i]].movement = 1;
			buffer[posX + _c_shp_bf[i]].color = shape_colors[_c_shp];
		}

		if(fps_cntr == callback_duration){
			callback();
			fps_cntr = 0;
		}


		for(int i = 0; i < MAX_BLKS; i++){
			drawRect(V2(buffer[i].x, buffer[i].y), (buffer[i].set || buffer[i].movement) ? buffer[i].color : (Color){ 25, 25, 25, 255 }, BLACK);
		}




		drawText(V2(280, 50), 30, WHITE, "%d, %d", _c_col, _c_row);


		EndDrawing();


		// Exit
		if(IsKeyPressed(KEY_Q)){ break; }


		if(IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)){
			int points[4] = { 0 };
			int cntr = 0;
			for(int i = 0; i < MAX_BLKS; i++){ if(buffer[i].movement){ points[cntr++] = buffer[i].x; }}

			sort(points, 4);


			if(points[N - 1] < (AREA_W - 1)) _c_col++;
		}

		if(IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)){

			int points[4] = { 0 };
			int cntr = 0;
			for(int i = 0; i < MAX_BLKS; i++){ if(buffer[i].movement){ points[cntr++] = buffer[i].x; }}

			sort(points, 4);


			if(points[0] > 0) _c_col--;

			// if(_c_col > 0) _c_col--;
		}



		// NO movement
		for(int i = 0; i < MAX_BLKS; i++){ buffer[i].movement = 0; }

		fps_cntr++;
	}


	CloseWindow();
	return 0;
}

