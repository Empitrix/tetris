#include <stdio.h>
#include <stdarg.h>
#include <raylib.h>
#include <string.h>


#define V2(x, y) (Vector2){x, y}
#define R(x, y, w, h) (Rectangle){x, y, w, h}

#define ScreenWidth 400
#define ScreenHeight 500

#define AREA_W 10
#define AREA_H 20

#define BS 25
#define FPS 60
#define N 4


void clampValue(int *var, int min, int max, int type){
	if(type){
		if((*var) < max){ (*var)++; } 
	} else {
		if((*var) > min){ (*var)--; }
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


static int shapes[7][N][N] = {
	{ {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0} }, // I
	{ {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} }, // J
	{ {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} }, // L
	{ {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} }, // O
	{ {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0} }, // Z
	{ {0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0} }, // T
	{ {0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0} }, // S
};


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


void drawShape(Vector2 pos, shape_t shape, direction_t direction){
	int cshape[N][N] = { 0 };
	memcpy(cshape, shapes[shape], N * N * N);

	Color colors[7] =    { YELLOW, BLUE, RED, GREEN, ORANGE, PURPLE, VIOLET };
	Color colors_bg[7] = { BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };


	for(int i = 0; i < (int)direction; i++){ rotate90(cshape); }

	for(int x = 0; x < N; x++){
		for(int y = 0; y < N; y++){
			if(cshape[x][y] == 1){
				DrawRectangle(((x + pos.x) - 1) * BS, ((y + pos.y) - 1) * BS, BS, BS, colors[shape]);
				DrawRectangleLines((x + pos.x) * BS, (y + pos.y) * BS, BS, BS, colors_bg[shape]);
			}
		}
	}
}


static int _c_dir = 0;
static int _c_col = 4;
static int _c_row = 0;

typedef struct {
	int set;
	Color color;
	int x;
	int y;
} block_t;

static block_t buffer[AREA_H][AREA_W] = { 0 };
static int fps_cntr = 0;
static int trigger = 0;

int main(void){
	SetTraceLogLevel(LOG_NONE);
	InitWindow(ScreenWidth, ScreenHeight, "Tetris");
	SetTargetFPS(FPS);

		// buffer[1][1].set = 1;
		// buffer[2][2].set = 1;
		// buffer[2][1].set = 1;
		// buffer[2][0].set = 1;

	while (!WindowShouldClose()){

		if((fps_cntr % (FPS / 4)) == 0){ trigger = 1; fps_cntr = 0; }



		// if(trigger){
		// 	if(_c_row < (AREA_H - 1)) _c_row++;
		// }


		BeginDrawing();
		ClearBackground(BLACK);

		// if(trigger){
		// 	if(_c_row < (AREA_H - 2)) _c_row++;
		// }
		//


		// if(_c_row == AREA_H){ }

		for(int x = 0; x < AREA_W; x++){
			for(int y = 0; y < AREA_H; y++){
				DrawRectangle(x * BS, y * BS, BS, BS, buffer[y][x].set ? BLUE : RED);
				DrawRectangleLines(x * BS, y * BS, BS, BS, BLACK);
			}
		}

		drawShape(V2(_c_col, _c_row), Shape_I, (direction_t)_c_dir);

		drawText(V2(280, 50), 30, WHITE, "%d, %d", _c_col, _c_row);

		EndDrawing();








		// Exit
		if(IsKeyPressed(KEY_Q)){ break; }

		if(IsKeyPressed(KEY_UP)){ _c_dir++; if(_c_dir >= 4){ _c_dir = 0; } }
		if(IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT)){ clampValue(&_c_col, 0, 10, 1); }
		if(IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT)){ clampValue(&_c_col, 0, 10, 0); }

		trigger = 0;
		fps_cntr++;

	}

	CloseWindow();
	return 0;
}

