#include <raylib.h>

#define V2(x, y) (Vector2){x, y}
#define R(x, y, w, h) (Rectangle){x, y, w, h}


#define MAX_SHAPES 7
#define SHAPE_W 4
#define SHAPE_H 3



typedef struct {
	int a[4];
	int b[4];
	int c[4];
} shape;



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




typedef struct {
	int set;
	int movement;
	int reflect;
	Color color;
	int x;
	int y;
} block_t;


static Color shape_clr[MAX_SHAPES] = {
	(Color){ 0,   240, 240, 255 },  // I
	(Color){ 0,   0,   240, 255 },  // J
	(Color){ 240, 160, 0,   255 },  // L
	(Color){ 240, 240, 0,   255 },  // O
	(Color){ 240, 0,   0,   255 },  // Z
	(Color){ 160, 0,   240, 255 },  // T
	(Color){ 0,   240, 0,   255 },  // S
};

static shape shapes[MAX_SHAPES] = {
	{ .a = { 0, 0, 0, 0 }, .b = { 0, 0, 0, 0 }, .c = { 1, 1, 1, 1 } },  // I
	{ .a = { 0, 0, 1, 0 }, .b = { 0, 0, 1, 0 }, .c = { 0, 1, 1, 0 } },  // J
	{ .a = { 0, 1, 0, 0 }, .b = { 0, 1, 0, 0 }, .c = { 0, 1, 1, 0 } },  // L
	{ .a = { 0, 0, 0, 0 }, .b = { 0, 1, 1, 0 }, .c = { 0, 1, 1, 0 } },  // O
	{ .a = { 0, 0, 0, 0 }, .b = { 1, 1, 0, 0 }, .c = { 0, 1, 1, 0 } },  // Z
	{ .a = { 0, 0, 0, 0 }, .b = { 0, 1, 0, 0 }, .c = { 1, 1, 1, 0 } },  // T
	{ .a = { 0, 0, 0, 0 }, .b = { 0, 1, 1, 0 }, .c = { 1, 1, 0, 0 } },  // S
};



/*

static shape shapes[MAX_SHAPES] = {
	// I
	{
		.a = { 0, 0, 0, 0 },
		.b = { 0, 0, 0, 0 },
		.c = { 1, 1, 1, 1 }
	},
	// J
	{
		.a = { 0, 0, 1, 0 },
		.b = { 0, 0, 1, 0 },
		.c = { 0, 1, 1, 0 }
	},
	// L
	{
		.a = { 0, 1, 0, 0 },
		.b = { 0, 1, 0, 0 },
		.c = { 0, 1, 1, 0 }
	},
	// O
	{
		.a = { 0, 0, 0, 0 },
		.b = { 0, 1, 1, 0 },
		.c = { 0, 1, 1, 0 }
	},
	// Z
	{
		.a = { 0, 0, 0, 0 },
		.b = { 1, 1, 0, 0 },
		.c = { 0, 1, 1, 0 }
	},
	// T
	{
		.a = { 0, 0, 0, 0 },
		.b = { 0, 1, 0, 0 },
		.c = { 1, 1, 1, 0 }
	},
	// S
	{
		.a = { 0, 0, 0, 0 },
		.b = { 0, 1, 1, 0 },
		.c = { 1, 1, 0, 0 }
	},
};

*/
