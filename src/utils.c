// #include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <stdarg.h>
#include <stdio.h>




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


