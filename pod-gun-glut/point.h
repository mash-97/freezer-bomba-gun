#include <stdio.h>
#include <stdlib.h>

typedef struct {
	float x;
	float y;
} Point;

Point newPoint(float x, float y) {
	Point new_point = {x,y};
	return new_point;
}

