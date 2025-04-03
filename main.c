#include <stdarg.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include "src/utils.h"
#include "src/types.h"
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>




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
// static int callback_duration = 10;  // each 10 MS
static int callback_duration = 50;  // each 10 MS
static block_t table[ROWS][COLS] = { 0 };
static block_t tmp[ROWS][COLS] = { 0 };
static int shape_idx = 0;
static int next_shape_idx = 0;

static int nlines = 0;
static int rr = 0;














#define SAMPLE_RATE 44100
#define AMPLITUDE 32000
#define SDUR 0.5f

typedef struct {
	float frequency;
	float duration; // in seconds
} Note;

// Tetris Theme (Korobeiniki) melody
Note tetrisMelody[] = {
	{ 659.25, SDUR }, { 493.88, SDUR }, { 523.25, SDUR }, { 587.33, SDUR },   // G5, D5, C5, D5
	{ 523.25, SDUR }, { 493.88, SDUR }, { 440.00, SDUR }, { 440.00, SDUR },   // C5, D5, A4, A4
	{ 523.25, SDUR }, { 659.25, SDUR }, { 587.33, SDUR }, { 523.25, SDUR },   // C5, G5, D5, C5
	{ 493.88, SDUR }, { 523.25, SDUR }, { 587.33, SDUR }, { 659.25, SDUR },   // D5, C5, D5, G5
	{ 523.25, SDUR }, { 440.00, SDUR }, { 440.00, SDUR }, {   0.00, SDUR },   // C5, A4, A4, Pause
	{ 587.33, SDUR }, { 698.46, SDUR }, { 880.00, SDUR }, { 783.99, SDUR },   // D5, F5, A5, G5
	{ 698.46, SDUR }, { 659.25, SDUR }, { 523.25, SDUR }, { 659.25, SDUR },   // F5, G5, C5, G5
	{ 587.33, SDUR }, { 523.25, SDUR }, { 493.88, SDUR }, { 523.25, SDUR },   // D5, C5, D5, C5
	{ 587.33, SDUR }, { 659.25, SDUR }, { 523.25, SDUR }, { 440.00, SDUR }	// D5, G5, C5, A4
};

#define MELODY_LENGTH (sizeof(tetrisMelody) / sizeof(Note))

// Generate a sine wave for a given note
Wave GenerateNoteWave(float frequency, float duration) {
	int sampleCount = (int)(SAMPLE_RATE * duration);
	short *samples = (short *)malloc(sampleCount * sizeof(short));

	for (int i = 0; i < sampleCount; i++) {
		float t = (float)i / SAMPLE_RATE;
		samples[i] = (short)(AMPLITUDE * sinf(2.0f * PI * frequency * t));
	}

	Wave wave = { .frameCount = sampleCount, .sampleRate = SAMPLE_RATE, .sampleSize = 16, .channels = 1, .data = samples };
	return wave;
}

void *PlayMelodyThread(void *arg) {
	Note *melody = (Note *)arg;
	while (1) {  // Infinite loop to keep repeating the melody
		for (int i = 0; i < MELODY_LENGTH; i++) {
			if (melody[i].frequency > 0.0) {
				Wave wave = GenerateNoteWave(melody[i].frequency, melody[i].duration);
				Sound sound = LoadSoundFromWave(wave);
				PlaySound(sound);
				usleep((int)(melody[i].duration * 500000)); // Delay for note duration
				UnloadSound(sound);
				free(wave.data);
			} else {
				usleep((int)(melody[i].duration * 1000000)); // Pause for silence
			}
		}
	}
	return NULL;
}




// Play the sound when a block is falling
void block_down_audio(void) {
	// Example melody for block down: quick and short notes
	Note blockDown[] = {
		{ 220.00, 0.2f }, { 220.00, 0.2f }, { 220.00, 0.2f }  // C4 (quick), repetitive
	};
	int length = sizeof(blockDown) / sizeof(Note);

	for (int i = 0; i < length; i++) {
		if (blockDown[i].frequency > 0.0) {
			Wave wave = GenerateNoteWave(blockDown[i].frequency, blockDown[i].duration);
			Sound sound = LoadSoundFromWave(wave);
			PlaySound(sound);
			// usleep((int)(blockDown[i].duration * 500000)); // Delay for note duration
			UnloadSound(sound);
			free(wave.data);
		} else {
			// usleep((int)(blockDown[i].duration * 1000000)); // Pause for silence
		}
	}
}

// Play the sound when a row is full
void row_full_audio(void) {
	// Example melody for row full: slightly longer note sequence
	Note rowFull[] = {
		{ 440.00, 0.4f }, { 523.25, 0.4f }, { 440.00, 0.4f }  // A4, C5, A4
	};
	int length = sizeof(rowFull) / sizeof(Note);

	for (int i = 0; i < length; i++) {
		if (rowFull[i].frequency > 0.0) {
			Wave wave = GenerateNoteWave(rowFull[i].frequency, rowFull[i].duration);
			Sound sound = LoadSoundFromWave(wave);
			PlaySound(sound);
			usleep((int)(rowFull[i].duration * 500000)); // Delay for note duration
			UnloadSound(sound);
			free(wave.data);
		} else {
			usleep((int)(rowFull[i].duration * 1000000)); // Pause for silence
		}
	}
}



















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
	// printf("%d\n", dist);

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



int check_fill_row(int y_set){
	for(int x = 0; x < COLS; x++){
		if(table[y_set][x].set == 0){
			return 0;
		}
	}
	return 1;
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

int check_rows2(void){
	int removed = 0;
	int move_down = 0;
	for(int y = 0; y < ROWS; y++){
		if(check_fill_row(y)){
			for(int i = 0; i < COLS; i++){
				table[y][i] = (block_t){ 0 };
				move_down = 1;
				removed = y;
				goto after_part;
			}
		}
	}

after_part:

	if(move_down){
		printf("%d\n", removed);
		for(int x = 0; x < COLS; x++){
			for(int y = removed; y >= 0; y--){
				table[y + 1][x] = table[y][x];
			}
		}
	}


	// for(int x = removed; x < COLS; x++){
	// }

	return move_down;
}


int callback(){
	block_down_audio();
	int hit = move_down();

	if(hit){
		check_rows();
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


int main(void){
	SetTraceLogLevel(LOG_NONE);
	InitWindow(WIDTH, HEIGHT, "Tetris");
	SetTargetFPS(FPS);

	InitAudioDevice();

	pthread_t melodyThread;

	// Start melody playback in a separate thread as soon as the window is opened
	pthread_create(&melodyThread, NULL, PlayMelodyThread, (void *)tetrisMelody);
	pthread_detach(melodyThread); // Detach the thread so it can run independently



	for(int x = 0; x < COLS; x++){
		for(int y = 0; y < ROWS; y++){
			table[y][x].x = x;
			table[y][x].y = y;
		}
	}

	// shape_idx = randint(0, MAX_SHAPES - 1);
	next_shape_idx = randint(0, MAX_SHAPES - 1);
	import_shape();


	while (!WindowShouldClose()){


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



		// Exit
		if(IsKeyPressed(KEY_Q)){ break; }
		if(IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)){ move_right(); }
		if(IsKeyPressed(KEY_LEFT)  || IsKeyPressedRepeat(KEY_LEFT)){  move_left();  }

		if(IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)){
			rotate();
			if(rr == 2){
				move_left();
				rr = 0;
			}
			fps_cntr = 0;
		}

		if(IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)){
			callback();
			fps_cntr = 0;
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
	CloseAudioDevice();
	return 0;
}

