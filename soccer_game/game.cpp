#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down&&input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down&&input->buttons[b].changed)


float player_pos_0 = 0.f;
float player_pos_1 = -12.f;
float player_p_1, player_dp_1, player_p_0, player_dp_0;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_x, ball_y, ball_dp_x = 12, ball_dp_y=21;

float ball_2_dp_x, ball_2_dp_y;



int player_score_0 = 0;
int player_score_1 = 0;
int predictedball_y = 0;
float ball_1_x = 0;
float ball_1_y = 0;
float ball_2_x = 3.5;
float ball_2_y = 1.5;



internal void 
simulate_player(float* p, float* dp, float ddp, float dt) {
	ddp -= *dp * 10;
	*p = *p + *dp * dt + ddp * dt * dt * 0.5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y;
		*dp = -0.1 * *dp;
	}

	if (*p - player_half_size_y < -arena_half_size_y) {
		*p = -arena_half_size_y + player_half_size_y;
		*dp = -0.1 * *dp;
	}
}


internal boolean
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {

	return(p1x+hs1x > p2x - hs2x&&
		p1x - hs1x < p2x + hs2x&&
		p1y + hs1y > p2y - hs2y&&
		p1y + hs1y < p2y + hs2y);

} 

internal boolean
circle_vs_circle(float o1x,float o1y, float o2x, float o2y, float o1r, float o2r){
	return((o1x - o2x) * (o1x - o2x) + (o1y - o2y) * (o1y - o2y) < (o1r + o2r) * (o1r + o2r));


}

internal void
final_speed(float* final_px, float* final_py, float s_x, float s_y, float b_x, float b_y,float s_dx,float s_dy,float b_dx,float b_dy) {

	float relative_dx = s_dx - b_dx;
	float relative_dy = s_dy - b_dy;
	float relative_x = s_x - b_x;
	float relative_y = s_y - b_y;

	

	if (relative_dx * relative_x+relative_y*relative_dy > 0) {
		return;
	}

	float length = sqrt(relative_x * relative_x + relative_y * relative_y);
	relative_x /= length;
	relative_y /= length;

	float m = (relative_y / relative_x);
	float constant = 1.0 / (1.0 + m*m);

	float relative_dx_back = (relative_dy * 2 * m + relative_dx * (1 - m * m)) * constant;
	float relative_dy_back = (relative_dx * 2 * m + relative_dy * (m * m - 1)) * constant;

	*final_px = -1.0*relative_dx_back + b_dx;
	*final_py = -1.0*relative_dy_back + b_dy;
		
}








internal void
simulate_game(Input* input, float dt) {



	clear_screen(0x7f0030);
	draw_rect(0x900050, 0, 0, 85, 45);

	float player_ddp_1 = 0.f;
	float player_ddp_0 = 0.f;

	float ball_2_ddp_y = 0;
	float ball_2_ddp_x = 0;


	
	if (is_down(BUTTON_UP))	ball_2_ddp_y += 500.4;
	if (is_down(BUTTON_DOWN))  ball_2_ddp_y -= 500.4;
	if (is_down(BUTTON_RIGHT))	ball_2_ddp_x += 500.4;
	if (is_down(BUTTON_LEFT))  ball_2_ddp_x -= 500.4;

	simulate_player(&ball_2_x, &ball_2_dp_x, ball_2_ddp_x, dt);
	simulate_player(&ball_2_y, &ball_2_dp_y, ball_2_ddp_y, dt);

	if (circle_vs_circle(ball_2_x, ball_2_y, ball_x, ball_y, 6, 1)) {
		final_speed(&ball_dp_x, &ball_dp_y, ball_x, ball_y, ball_2_x, ball_2_y, ball_dp_x, ball_dp_y, ball_2_dp_x, ball_2_dp_y);

	};



	u32 color = 0x00ffff;
	draw_circle(color, ball_1_x, ball_1_y, 3);
	draw_circle(color, ball_2_x, ball_2_y, 6);
	
	
	float rise_1 = 45 + ball_y + (80-ball_x)  * ball_dp_y / ball_dp_x;

	while (rise_1 < 0 || rise_1>180) {
		if (rise_1 < 0) {
			rise_1 += 180;
		}
		else {
			rise_1 -= 180;
		}
	}



	if (rise_1 > 90) {
		predictedball_y = 45 - (rise_1 - 90);
	}
	else {
		predictedball_y = -45 + rise_1;
	}



	if (predictedball_y > player_pos_1 + 9) {
		player_ddp_1 = 4000;
	}
	if (predictedball_y < player_pos_1 - 9) {
		player_ddp_1 = -4000;
	}
	
	

	
	
	if (is_down(BUTTON_W))	player_ddp_0 += 2000;
	
	if (is_down(BUTTON_S))  player_ddp_0 -= 2000;
	
	float rise=45+ball_y+(ball_x+80)*-1.0*ball_dp_y / ball_dp_x;

	while (rise < 0 || rise>180) {
		if (rise < 0) {
			rise += 180;
		}
		else {
			rise -= 180;
		}
	}



	if (rise > 90) {
		predictedball_y = 45 - (rise-90);
	}
	else {
		predictedball_y = -45 + rise;
	}


	/*
	if (predictedball_y > player_pos_0+9) {
		player_ddp_0 = 6000;
	}
	if (predictedball_y < player_pos_0-9) {
		player_ddp_0 = -6000;
	}
	*/



	simulate_player(&player_pos_1, &player_dp_1, player_ddp_1, dt);
	simulate_player(&player_pos_0, &player_dp_0, player_ddp_0, dt);

	//ball sim
	{

		if (aabb_vs_aabb(ball_x, ball_y, 1, 1, 80, player_pos_1, player_half_size_x, player_half_size_y)) {
			ball_x = 80 - player_half_size_x - 1;
			ball_dp_x *= -1;
			ball_dp_y = (ball_y - player_pos_1) * ball_dp_x / -12.0 + player_dp_1 * .74f;
		}
		else if (aabb_vs_aabb(ball_x, ball_y, 1, 1, -80, player_pos_0, player_half_size_x, player_half_size_y)) {
			ball_x = -80 + player_half_size_x + 1;
			ball_dp_x *= -1;
			ball_dp_x *= (1.0 + 0 / (0.03 * ball_dp_x));
			ball_dp_y = (ball_y - player_pos_0) *ball_dp_x/12.0 + player_dp_0 * .74f;

		}
		if (ball_y + 1 > arena_half_size_y) {
			ball_y = arena_half_size_y - 2;
			ball_dp_y *= -1;
		}
		if (ball_y - 1 < -arena_half_size_y) {
			ball_y = -arena_half_size_y + 2;
			ball_dp_y *= -1;
		}

		if (ball_x > arena_half_size_x | ball_x < -arena_half_size_x) {
			if (ball_x > 0) {
				player_score_0++;
			}
			else {
				player_score_1++;
			}
			ball_dp_y = 0;
			ball_dp_x = 100;
			ball_x = 0;
			ball_y = 0;


		}

		ball_x += ball_dp_x * dt;
		ball_y += ball_dp_y * dt;
		
		draw_number(0x00ffff, -10, 20, 2, player_score_0);
		draw_number(0x00ffff, 10, 20, 2, player_score_1);

		draw_circle(0xffffff, ball_x, ball_y, 1);
		draw_rect(0x00fff0, 80, player_pos_1, 2.5, 12);
		//draw_circle(0x0018090, 200, 300, 200);
		draw_rect(0x100000, -80, player_pos_0, 2.5, 12);

	}
}