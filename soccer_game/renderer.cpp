
#include <cmath>
#include <algorithm>
using namespace std;

 const double PI = 3.14159265358979323846;


internal
void render_background(){
	
	unsigned int* pixel = (unsigned int*)render_state.memory;
		for (int y = 0; y < render_state.height; y++) {
			for (int x = 0; x < render_state.width; x++) {
				*pixel++ = 0x010000 * x + 0x000100 * y + x * y * (0x000001);

			}

		}


}

internal
void clear_screen(unsigned int color) {

	unsigned int* pixel = (unsigned int*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;

		}

	}


}


global_variable float render_scale = 0.01f;

internal
boolean is_point_above_line(float p_x,float p_y, float e_x, float e_y, float e_2x, float e_2y){

	float slope = (e_2y - e_y) / (e_2x - e_x);
	return((p_x-e_x)*slope+e_y<p_y);
	

}

internal
void draw_rect_pixels(unsigned int color, int x0, int y0, int x1, int y1) {

	unsigned int* pixel = (unsigned int*)render_state.memory;
	for (int y = y0; y < y1; y++) {
		unsigned int* pixel = (unsigned int*)render_state.memory + x0 + y * render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;

		}

	}


}




internal
void draw_circle_pixels(unsigned int color, int x0,int y0, int radius) {

	unsigned int* pixel = (unsigned int*)render_state.memory;
	for (int y = y0-radius; y<=y0+radius && y < render_state.height&&y>=0; y++) {
		for (int x = x0-radius;   x>=0&& x < render_state.width&&x<=x0+radius; x++) {
			
			int super = radius * radius;
			if (((x - x0)* (x - x0) + (y - y0)* (y - y0)) < (super)) {
			pixel = (u32*)render_state.memory + x + y * render_state.width;
			*pixel = color;
			}
		}

	}


}

internal void
draw_circle(u32 color, float x, float y, float radius) {

	x *= render_state.height * render_scale;
	y *= render_state.height * render_scale;
	radius *= render_state.height * render_scale;

	x += render_state.width / 2.f;
	y += render_state.height / 2.f;

	draw_circle_pixels(color, x, y, radius);


}




internal void
draw_rect(u32 color, float x, float y, float half_size_x, float half_size_y) {

	x *= render_state.height * render_scale;
	y *= render_state.height * render_scale;
	half_size_x *= render_state.height * render_scale;
	half_size_y *= render_state.height * render_scale;

	x += render_state.width / 2.f;
	y += render_state.height / 2.f;

	int x0 = x - half_size_x;
	int x1 = x + half_size_x;
	int y0 = y - half_size_y;
	int y1 = y + half_size_y;

	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);

	draw_rect_pixels(color, x0, y0, x1, y1);


}


internal void
draw_rotate_rect(u32 color, float x, float y, float half_size_x, float half_size_y, float radians) {

  if(radians == 0.0) {
	  if (radians == 0.0 || radians == PI)
		  draw_rect(color, x, y, half_size_x, half_size_y);
	  else
		  draw_rect(color, x, y, half_size_y, half_size_x);


		  return;

}
	x *= render_state.height * render_scale;
	y *= render_state.height * render_scale;
	half_size_x *= render_state.height * render_scale;
	half_size_y *= render_state.height * render_scale;

	x += render_state.width / 2.f;
	y += render_state.height / 2.f;

	float angle = atan(half_size_y/half_size_x);
	float angle2 = PI - angle;
	float angle3 = PI + angle;
	float angle4 = 2 * PI - angle;

	float hypotenuse = sqrt(half_size_x * half_size_x + half_size_y * half_size_y);


	float x0 = x + hypotenuse * cos(radians + angle);
	float y0 = y + hypotenuse* sin(radians + angle);
	float x1 = x + hypotenuse * cos(radians + angle2);
	float y1 = y + hypotenuse* sin(radians + angle2);
	float x2 = x + hypotenuse * cos(radians + angle3);
	float y2 = y + hypotenuse* sin(radians + angle3);
	float x3 = x + hypotenuse * cos(radians + angle4);
	float y3 = y + hypotenuse* sin(radians + angle4);
	


	unsigned int* pixel = (unsigned int*)render_state.memory;
	

	int x_min = max(0.0f, min(min(min(x0, x1), x2), x3));
	int x_max = min(render_state.width - 1.0f, max(max(max(x0, x1), x2), x3));
	int y_min = max(0.0f, min(min(min(y0, y1), y2), y3));
	int y_max = min(render_state.height - 1.0f, max(max(max(y0, y1), y2), y3));

	for (int x = x_min; x<x_max; x++) {
	
		for (int y = y_min; y < y_max; y++) {
			if ( (is_point_above_line(x, y, x3, y3, x0, y0) ^is_point_above_line(x, y, x1, y1, x2, y2)) && (is_point_above_line(x, y, x2, y2, x3, y3)^ is_point_above_line(x, y, x0, y0, x1, y1))) {
			
				pixel = (u32*)render_state.memory + x + y * render_state.width;
				*pixel = color;
			}

		}
	}


}

internal void
draw_number(u32 color, float x, float y, float size,int number) {
	float half_size = size * .5f;

	bool drew_number = false;
	while (number || !drew_number) {
		drew_number = true;

		int digit = number % 10;
		number = number / 10;

		switch (digit) {
		case 0: {
			draw_rect(color, x - size, y, half_size, 2.5f * size);
			draw_rect(color, x + size, y, half_size, 2.5f * size);
			draw_rect(color, x, y + size * 2.f, half_size, half_size);
			draw_rect(color, x, y - size * 2.f, half_size, half_size);
			x -= size * 4.f;
		} break;

		case 1: {
			draw_rect(color, x + size, y, half_size, 2.5f * size);
			x -= size * 2.f;
		} break;

		case 2: {
			draw_rect(color, x, y + size * 2.f, 1.5f * size, half_size);
			draw_rect(color, x, y, 1.5f * size, half_size);
			draw_rect(color, x, y - size * 2.f, 1.5f * size, half_size);
			draw_rect(color, x + size, y + size, half_size, half_size);
			draw_rect(color, x - size, y - size, half_size, half_size);
			x -= size * 4.f;
		} break;

		case 3: {
			draw_rect(color, x - half_size, y + size * 2.f, size, half_size);
			draw_rect(color, x - half_size, y, size, half_size);
			draw_rect(color, x - half_size, y - size * 2.f, size, half_size);
			draw_rect(color, x + size, y, half_size, 2.5f * size);
			x -= size * 4.f;
		} break;

		case 4: {
			draw_rect(color, x + size, y, half_size, 2.5f * size);
			draw_rect(color, x - size, y + size, half_size, 1.5f * size);
			draw_rect(color, x, y, half_size, half_size);
			x -= size * 4.f;
		} break;

		case 5: {
			draw_rect(color, x, y + size * 2.f, 1.5f * size, half_size);
			draw_rect(color, x, y, 1.5f * size, half_size);
			draw_rect(color, x, y - size * 2.f, 1.5f * size, half_size);
			draw_rect(color, x - size, y + size, half_size, half_size);
			draw_rect(color, x + size, y - size, half_size, half_size);
			x -= size * 4.f;
		} break;

		case 6: {
			draw_rect(color, x + half_size, y + size * 2.f, size, half_size);
			draw_rect(color, x + half_size, y, size, half_size);
			draw_rect(color, x + half_size, y - size * 2.f, size, half_size);
			draw_rect(color, x - size, y, half_size, 2.5f * size);
			draw_rect(color, x + size, y - size, half_size, half_size);
			x -= size * 4.f;
		} break;

		case 7: {
			draw_rect(color, x + size, y, half_size, 2.5f * size);
			draw_rect(color, x - half_size, y + size * 2.f, size, half_size);
			x -= size * 4.f;
		} break;

		case 8: {
			draw_rect(color, x - size, y, half_size, 2.5f * size);
			draw_rect(color, x + size, y, half_size, 2.5f * size);
			draw_rect(color, x, y + size * 2.f, half_size, half_size);
			draw_rect(color, x, y - size * 2.f, half_size, half_size);
			draw_rect(color, x, y, half_size, half_size);
			x -= size * 4.f;
		} break;

		case 9: {
			draw_rect(color, x - half_size, y + size * 2.f, size, half_size);
			draw_rect(color, x - half_size, y, size, half_size);
			draw_rect(color, x - half_size, y - size * 2.f, size, half_size);
			draw_rect(color, x + size, y, half_size, 2.5f * size);
			draw_rect(color, x - size, y + size, half_size, half_size);
			x -= size * 4.f;
		} break;
		}

	}
}