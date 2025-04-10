#include <stdarg.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "src/utils.h"
#include "src/types.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>





// No valid platform defined!, Using default!
#if !defined(PLATFORM_WEB) && !defined(PLATFORM_ANDROID) && !defined(PLATFORM_DESKTOP)
#ifndef DEKSTOP_PLATFORM
#define DEKSTOP_PLATFORM
#endif
#endif


// Stop page scrolling
#if defined (PLATFORM_WEB) || defined (WEB_PLATFORM)
#include <emscripten.h>
EM_JS(void, disable_scroll_keys, (), {
	document.addEventListener('keydown', function (e) {
		if (["ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight", " "].includes(e.key)) {
			e.preventDefault();
		}
	}, false);
});
#endif

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
static int callback_duration = 50;  // each 10 MS
static block_t table[ROWS][COLS] = { 0 };
static block_t tmp[ROWS][COLS] = { 0 };
static int shape_idx = 0;
static int next_shape_idx = 0;
static int nlines = 0;
static int rr = 0;
static int running = 1;


void drawRect(Vector2 pos, Color clr, Color outline) {
	DrawRectangleRounded((Rectangle){pos.x * BS, pos.y * BS, BS, BS}, 0.1f, 16, clr);

	int half = BS / 2;
	Color innerClr = (Color){ clr.r >= 100 ? clr.r - 100 : clr.r,
		clr.g >= 100 ? clr.g - 100 : clr.g,
		clr.b >= 100 ? clr.b - 100 : clr.b,
		clr.a };
	DrawRectangleRounded((Rectangle){(pos.x * BS) + (int)(half / 2), (pos.y * BS) + (int)(half / 2), half, half}, 0.1f, 8, innerClr);
	DrawRectangleLinesEx((Rectangle){pos.x * BS, pos.y * BS, BS, BS}, 1.8, outline);
}

/*
void drawRect(Vector2 pos, Color clr, Color outline){

	DrawRectangle(pos.x * BS, pos.y * BS, BS, BS, clr);

	int half = BS / 2;
	clr.r = clr.r >= 100 ? clr.r - 100 : clr.r;
	clr.g = clr.g >= 100 ? clr.g - 100 : clr.g;
	clr.b = clr.b >= 100 ? clr.b - 100 : clr.b;
	DrawRectangle((pos.x * BS) + (int)(half / 2), (pos.y * BS) + (int)(half / 2), half, half, clr);

	DrawRectangleLines((pos.x * BS) + 1, (pos.y * BS) + 1, BS - 1, BS - 1, outline);
}
*/


void draw_table(){
	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			if(table[y][x].set || table[y][x].movement){
				drawRect(V2(x, y), table[y][x].color, BLACK);
			} else {
				if(table[y][x].reflect){
					// drawRect(V2(x, y), BACKGROUND, WHITE);
					drawRect(V2(x, y), (Color){50, 50, 50, 100}, WHITE);
				} else {
					drawRect(V2(x, y), BACKGROUND, BLACK);
				}
			}
		}
	}
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
			}
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


void import_shape(){
	shape_idx = next_shape_idx;
	next_shape_idx = randint(0, MAX_SHAPES - 1);
	Color clr = shape_clr[shape_idx];

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



int check_fill_row(int y_set){
	for(int x = 0; x < COLS; x++){
		if(table[y_set][x].set == 0){
			return 0;
		}
	}
	return 1;
}


void clear_stack(){
	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			table[y][x] = (block_t){ 0 };
			table[y][x].set = 0;
			table[y][x].movement = 0;
			table[y][x].color = BACKGROUND;
			table[y][x].reflect = 0;
		}
	}

	next_shape_idx = randint(0, MAX_SHAPES - 1);
	import_shape();
	nlines = 0;
	rr = 0;
}

int any_col_full(void){
	for(int x = 0; x < COLS; x++){
		int full = 1;
		for(int y = 0; y < ROWS; y++){
			if((table[y][x].set || table[y][x].movement) == 0){
				full = 0;
				break;
			}
		}
		if(full){
			return 1;
		}
	}
	return 0;
}

void check_rows(void) {
	for (int y = ROWS - 1; y >= 0; y--) {
		int full = 1;
		int empty = 1;
		
		// Check if row is full or empty
		for (int x = 0; x < COLS; x++) {
			if (!table[y][x].set) {
				full = 0;
			} else {
				empty = 0;
			}
		}
		
		// If row is full, remove it and shift everything down
		if (full) {
			for (int new_y = y; new_y > 0; new_y--) {
				for (int x = 0; x < COLS; x++) {
					table[new_y][x] = table[new_y - 1][x];
				}
			}
			// Clear the top row
			for (int x = 0; x < COLS; x++) {
				table[0][x] = (block_t){0};
			}
			y++; // Re-check the same row after shifting
			nlines++;

			// row_full_audio();
		}
		
		// If row is empty, break early (no more blocks above)
		if (empty) {
			break;
		}
	}
}


void game_over_func(){
	running = 0;
}



int callback(){
	// block_down_audio();
	int game_over = any_col_full();
	if(game_over){
		game_over_func();
	}

	int hit = move_down();

	if(hit){
		check_rows();
		import_shape();
		check_rows();
	}

	return hit;
}


void move_left(void) {
	for (int x = 0; x < COLS; x++) {
		for (int y = 0; y < ROWS; y++) {
			if (table[y][x].movement) {
				if (x == 0 || table[y][x - 1].set) {
					return;
				}
			}
		}
	}

	for (int x = 1; x < COLS; x++) {
		for (int y = 0; y < ROWS; y++) {
			if (table[y][x].movement) {
				table[y][x - 1] = table[y][x];
				table[y][x] = (block_t){0};
			}
		}
	}
}

void move_right(void) {
	for (int x = COLS - 1; x >= 0; x--) {
		for (int y = 0; y < ROWS; y++) {
			if (table[y][x].movement) {
				if (x == COLS - 1 || table[y][x + 1].set) {
					return;
				}
			}
		}
	}

	for (int x = COLS - 2; x >= 0; x--) {
		for (int y = 0; y < ROWS; y++) {
			if (table[y][x].movement) {
				table[y][x + 1] = table[y][x];
				table[y][x] = (block_t){0};
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
	
	// int center_x = (min_x + max_x + 1) / 2;
	// int center_y = (min_y + max_y + 1) / 2;

	int center_x = (min_x + max_x + 1) / 2;
	int center_y = (min_y + max_y + 1) / 2;
	
	block_t temp[ROWS][COLS];
	memset(temp, 0, sizeof(temp));
	
	// Rotate each moving block around the center
	for (int x = min_x; x <= max_x; x++) {
		for (int y = min_y; y <= max_y; y++) {
			if (table[y][x].movement) {
				int rel_x = x - center_x;
				int rel_y = y - center_y;
				int new_x = center_x - rel_y;
				int new_y = center_y + rel_x;
				
				// Check if within bounds
				if (new_x >= 0 && new_x < COLS && new_y >= 0 && new_y < ROWS) {
					temp[new_y][new_x] = table[y][x];
				} else {
					return; // Cancel rotation if out of bounds
				}
			}
		}
	}
	
	// Clear original positions
	for (int x = min_x; x <= max_x; x++) {
		for (int y = min_y; y <= max_y; y++) {
			if (table[y][x].movement) {
				table[y][x] = (block_t){0};
			}
		}
	}
	
	// Apply rotated positions
	for (int x = 0; x < COLS; x++) {
		for (int y = 0; y < ROWS; y++) {
			if (temp[y][x].movement) {
				table[y][x] = temp[y][x];
			}
		}
	}

	rr++;
}

void rotate2(void) {
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


void play_again(void){
	next_shape_idx = randint(0, MAX_SHAPES - 1);
	clear_stack();
	running = 1;
}

int main(void){

#ifdef PLATFORM_WEB
	disable_scroll_keys();
#endif

	SetTraceLogLevel(LOG_NONE);
	InitWindow(WIDTH, HEIGHT, "Tetris");
	SetTargetFPS(FPS);

	// Initialize background audio
	InitAudioDevice();
	pthread_t melodyThread;
	pthread_create(&melodyThread, NULL, PlayMelodyThread, (void *)tetrisMelody);
	pthread_detach(melodyThread);


	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			table[y][x].x = x;
			table[y][x].y = y;
		}
	}

	next_shape_idx = randint(0, MAX_SHAPES - 1);
	import_shape();


	while (!WindowShouldClose()){


		if(running == 0 && !WindowShouldClose()){

			BeginDrawing();
			ClearBackground(BLACK);

			char buffer[128] = { 0 };
			sprintf(buffer, "Game Over, Play again? (Y/N)");
			char score[128] = { 0 };
			sprintf(score, "%d", nlines);
			int len = strlen(buffer);
			DrawText(score, 10, 10, 50, WHITE);
			drawText(V2((int)(WIDTH / 2) - (int)((len * (int)(GetFontDefault().baseSize)) / 2), (int)(HEIGHT / 2) - (int)(GetFontDefault().baseSize / 2)),
				20, WHITE, buffer);
			drawButton("Play Again?", V2(125, 350), V2(140, 50), 20, play_again);

			if(IsKeyPressed(KEY_R) || IsKeyPressed(KEY_Y)){
				play_again();
			}

#if defined(DEKSTOP_PLATFORM)
			if(IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_N)){
				goto out_of_frame;
			}
#endif

			EndDrawing();

			continue;
		}

		BeginDrawing();
		ClearBackground(BLACK);

		if(fps_cntr == callback_duration){
			callback();
			fps_cntr = 0;
		}


		drawText(V2(280, 50), 30, WHITE, "%d, [%d]", fps_cntr, next_shape_idx);
		drawText(V2(280, 250), 30, WHITE, "Lines");
		drawText(V2(280, 300), 30, WHITE, "%d", nlines);


		reflect_down();
		draw_table();
		draw_next_shape();

		EndDrawing();


		int Q_PRESSED = 0;
		int RIGHT_PRESSED = 0;
		int LEFT_PRESSED = 0;
		int UP_PRESSED = 0;
		int DOWN_PRESSED = 0;
		int SPACE_PRESSED = 0;

#if defined (DEKSTOP_PLATFORM)
		Q_PRESSED = IsKeyPressed(KEY_Q);
		RIGHT_PRESSED = IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT);
		LEFT_PRESSED = IsKeyPressed(KEY_LEFT)  || IsKeyPressedRepeat(KEY_LEFT);
		UP_PRESSED = IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP);
		DOWN_PRESSED = IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN);
		SPACE_PRESSED = IsKeyPressed(KEY_SPACE);
#elif defined (ANDROID_PLATFORM)
		// Q_PRESSED = IsKeyPressed(KEY_Q);
		RIGHT_PRESSED = IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT);
		LEFT_PRESSED = IsKeyPressed(KEY_LEFT)  || IsKeyPressedRepeat(KEY_LEFT);
		UP_PRESSED = IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP);
		DOWN_PRESSED = IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN);
		SPACE_PRESSED = IsKeyPressed(KEY_SPACE);
#elif defined (WEB_PLATFORM)
		// // Q_PRESSED = IsKeyDown(KEY_Q);
		// RIGHT_PRESSED = IsKeyDown(KEY_RIGHT);
		// LEFT_PRESSED = IsKeyDown(KEY_LEFT);
		// UP_PRESSED = IsKeyDown(KEY_UP);
		// DOWN_PRESSED = IsKeyDown(KEY_DOWN);
		// SPACE_PRESSED = IsKeyDown(KEY_SPACE);

		UpdateKeyStates();

		// Q_PRESSED = IsKeyPressedEmu(KEY_Q);
		RIGHT_PRESSED = IsKeyPressedEmu(KEY_RIGHT);
		LEFT_PRESSED = IsKeyPressedEmu(KEY_LEFT);
		UP_PRESSED = IsKeyPressedEmu(KEY_UP);
		DOWN_PRESSED = IsKeyPressedEmu(KEY_DOWN);
		SPACE_PRESSED = IsKeyPressedEmu(KEY_SPACE);
#endif

		if(Q_PRESSED){ break; }
		if(RIGHT_PRESSED){ move_right(); }
		if(LEFT_PRESSED){  move_left();  }

		if(UP_PRESSED){
			rotate();
			if(rr == ((shape_idx == (int)Shape_O) ? 1 : 2)){
				move_left();
				rr = 0;
			}
			fps_cntr = 0;
		}

		if(DOWN_PRESSED){
			callback();
			fps_cntr = 0;
		}

		if(SPACE_PRESSED){
			int hit_status = 0;
			int cntr = 0;
			while(hit_status == 0){
				if(cntr++ >= (COLS + (N * N))){
					game_over_func();
					hit_status = 0;
					break;
				}
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

out_of_frame:
	CloseWindow();
	CloseAudioDevice();
	return 0;
}

