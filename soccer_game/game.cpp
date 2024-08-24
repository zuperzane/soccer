#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down&&input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down&&input->buttons[b].changed)


float player_pos_0 = 0.f;
float player_pos_1 = 0.f;
float player_p_1, player_dp_1, player_p_0, player_dp_0;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;
float ball_x, ball_y, ball_dp_x = 100, ball_dp_y=0;


internal boolean
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {

	return(p1x+hs1x > p2x - hs2x&&
		p1x - hs1x < p2x + hs2x&&
		p1y + hs1y > p2y - hs2y&&
		p1y + hs1y < p2y + hs2y);

} 

internal void
simulate_game(Input* input,float dt) {
	

	
	clear_screen(0x7f0030);
	draw_rect(0x900050,0, 0, 85, 45);
	
	float player_ddp_1 =0.f;
	float player_ddp_0 = 0.f;
	if (is_down(BUTTON_UP))	player_ddp_1 += 2000;
	if (is_down(BUTTON_DOWN))  player_ddp_1-=2000;
	if (is_down(BUTTON_W))	player_ddp_0 += 2000;
	if (is_down(BUTTON_S))  player_ddp_0 -= 2000;

	player_ddp_1 -= player_dp_1 * 10;
	player_pos_1 = player_pos_1 + player_dp_1 * dt + player_ddp_1 * dt * dt * 0.5f;
	player_dp_1 = player_dp_1 + player_ddp_1*dt;

	
	
	if (player_pos_1+player_half_size_y>arena_half_size_y) {
		player_pos_1 = arena_half_size_y-player_half_size_y;
		player_dp_1 = -0.4*player_dp_1;
	}
	if (player_pos_1 - player_half_size_y < -arena_half_size_y) {
		player_pos_1 = -arena_half_size_y +player_half_size_y;
		player_dp_1 = -0.4*player_dp_1;
	}

	if (player_pos_0 + player_half_size_y > arena_half_size_y) {
		player_pos_0 = arena_half_size_y - player_half_size_y;
		player_dp_0 = -player_dp_0;
	}
	if (player_pos_0 - player_half_size_y < -arena_half_size_y) {
		player_pos_0 = -arena_half_size_y + player_half_size_y;
		player_dp_0 = -player_dp_0;
	}


	player_ddp_0 -= player_dp_0 * 10;
	player_pos_0 = player_pos_0 + player_dp_0 * dt + player_ddp_0 * dt * dt * 0.5f;
	player_dp_0 = player_dp_0 + player_ddp_0 * dt;

	if (aabb_vs_aabb(ball_x, ball_y, 1, 1, 80, player_pos_1, player_half_size_x, player_half_size_y)) {
		ball_x = 80 - player_half_size_x-1;
		ball_dp_x *= -1;
		ball_dp_y = (ball_y - player_pos_1) * 2 + player_dp_1 * .74f;
	}else if (aabb_vs_aabb(ball_x, ball_y, 1, 1, -80, player_pos_0, player_half_size_x, player_half_size_y)) {
		ball_x = -80 + player_half_size_x + 1;
		ball_dp_x *= -1;
		ball_dp_x *= (1.0+1/ (0.03*ball_dp_x));
		ball_dp_y = (ball_y - player_pos_0) * 2 + player_dp_0 * .74f;
	
	}
	if (ball_y +1> arena_half_size_y) {
		ball_y = arena_half_size_y - 2;
		ball_dp_y *= -1;
	}
	if (ball_y - 1 < -arena_half_size_y) {
		ball_y = -arena_half_size_y + 2;
		ball_dp_y *= -1;
	}

	if (ball_x > arena_half_size_x| ball_x < -arena_half_size_x) {
		ball_dp_y = 0;
		ball_dp_x = 100;
		ball_x = 0;
		ball_y = 0;

	}
	
	ball_x += ball_dp_x*dt;
	ball_y += ball_dp_y * dt;


	draw_rect(0xffffff,ball_x, ball_y, 1, 1);
	draw_rect(0x00fff0 ,80, player_pos_1, 2.5, 12);
	//draw_circle(0x0018090, 200, 300, 200);
	draw_rect(0x100000, -80,player_pos_0,2.5,12);

}