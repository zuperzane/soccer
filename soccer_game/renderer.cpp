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