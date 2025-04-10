// #include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <raylib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>


#define MAX_KEYS 512



// ------------ AUDIO ------------
//
#define SAMPLE_RATE 44100
#define AMPLITUDE 32000
#define SDUR 0.5f


typedef struct {
	float frequency;
	float duration; // in seconds
} Note;

// Tetris Theme (Korobeiniki) melody
Note tetrisMelody[] = {
	{ 659.25, SDUR }, { 493.88, SDUR }, { 523.25, SDUR }, { 587.33, SDUR },  // G5, D5, C5, D5
	{ 523.25, SDUR }, { 493.88, SDUR }, { 440.00, SDUR }, { 440.00, SDUR },  // C5, D5, A4, A4
	{ 523.25, SDUR }, { 659.25, SDUR }, { 587.33, SDUR }, { 523.25, SDUR },  // C5, G5, D5, C5
	{ 493.88, SDUR }, { 523.25, SDUR }, { 587.33, SDUR }, { 659.25, SDUR },  // D5, C5, D5, G5
	{ 523.25, SDUR }, { 440.00, SDUR }, { 440.00, SDUR }, {   0.00, SDUR },  // C5, A4, A4, Pause
	{ 587.33, SDUR }, { 698.46, SDUR }, { 880.00, SDUR }, { 783.99, SDUR },  // D5, F5, A5, G5
	{ 698.46, SDUR }, { 659.25, SDUR }, { 523.25, SDUR }, { 659.25, SDUR },  // F5, G5, C5, G5
	{ 587.33, SDUR }, { 523.25, SDUR }, { 493.88, SDUR }, { 523.25, SDUR },  // D5, C5, D5, C5
	{ 587.33, SDUR }, { 659.25, SDUR }, { 523.25, SDUR }, { 440.00, SDUR }   // D5, G5, C5, A4
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




// int randint(int min, int max) {
// 	return min + (rand() % (max - min + 1));
// }

int randint(int min, int max) {
	unsigned int timeSeed = (unsigned int)clock();             // CPU time in clock ticks
	unsigned int randomValue = timeSeed * 1103515245 + 12345;  // LCG formula
	return min + (randomValue % (max - min + 1));
}


int contains(int arr[], int size, int item){
	for(int i = 0; i < size; i++){
		if(arr[i] == item){
			return 1;
		}
	}
	return 0;
}


void drawText(Vector2 pos, int font_size, Color color, char *fmt, ...){
	static char vout_buffer[256] = { 0 };
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(vout_buffer, fmt, arg_ptr);
	DrawText(vout_buffer, pos.x, pos.y, font_size, color);
	va_end(arg_ptr);
}




typedef void (*buttonCallback_t)(void);

void drawButton(const char *title, Vector2 position, Vector2 size, int font_size, buttonCallback_t callback){
	Rectangle rbtn = { position.x, position.y, size.x, size.y };


	int btnState = 0;
	bool btnAction = false;
	Vector2 mousePoint = GetMousePosition();

	// int font_size = (float)GetFontDefault().baseSize;


	if (CheckCollisionPointRec(mousePoint, rbtn)){
		DrawRectangleRounded(rbtn, 0.4, 10, DARKBLUE);
	} else {
		DrawRectangleRounded(rbtn, 0.4, 10, BLUE);
	}

	float textSize = strlen(title) * font_size;

	// Make the text in the center of the button
	Vector2 textPos = {
		.x = position.x + ((size.x / 2) - textSize / 4),
		.y = position.y + ((size.y / 2) - (int)(font_size / 2))
	};

	DrawTextEx(GetFontDefault(), title,
		textPos,
		font_size, 1, WHITE);

	if (CheckCollisionPointRec(mousePoint, rbtn)){
		btnState = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 2 : 1;
		if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && callback != NULL){ callback(); }
	}
}





static char currentKeyState[MAX_KEYS] = {0};
static char previousKeyState[MAX_KEYS] = {0};

void UpdateKeyStates(void) {
	for (int i = 0; i < MAX_KEYS; i++) {
		previousKeyState[i] = currentKeyState[i];
		currentKeyState[i] = IsKeyDown(i);
	}
}

int IsKeyPressedEmu(int key) {
	return currentKeyState[key] && !previousKeyState[key];
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
