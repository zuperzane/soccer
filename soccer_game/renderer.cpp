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
void draw_circle(unsigned int color, int x0,int y0, int radius) {

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
