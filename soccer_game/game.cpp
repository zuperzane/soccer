#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

typedef struct {
    float x, y;
    float radius;
    float dp_x, dp_y;
    float ddp_x, ddp_y;
} Ball;

float player_pos_0 = 0.f;
float player_pos_1 = -12.f;
float player_p_1, player_dp_1, player_p_0, player_dp_0;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;

int player_score_0 = 0;
int player_score_1 = 0;
int predictedball_y = 0;

float bot_radius = 6.0f;

Ball ball_1 = { 0, 0, 1, 12, 21, 0, 0 };
Ball ball_2 = { 3.5, 1.5, bot_radius, 0, 0, 0, 0 };
Ball ball_3 = { -13.5, -21.5, bot_radius, 0, 0, 0, 0 };
Ball ball_4 = { 23.5, 21.5, bot_radius, 0, 0, 0, 0 };
Ball ball_5 = { 23.5, 42.5, bot_radius+5.0, 0, 0, 0, 0 };
Ball ball_6 = { -40.5, 21.5, bot_radius, 0, 0, 0, 0 };


//functions hitting stuff like that


internal void simulate_player(float* p, float* dp, float ddp, float dt, float arena_half_size, float radius) {
    ddp -= *dp * 10;
    *p = *p + *dp * dt + ddp * dt * dt * 0.5f;
    *dp = *dp + ddp * dt;

    if (*p + radius > arena_half_size) {
        *p = arena_half_size - radius;
        *dp = -0.1 * *dp;
    }

    if (*p - radius < -arena_half_size) {
        *p = -arena_half_size + radius;
        *dp = -0.1 * *dp;
    }
}

internal void simulate_ball(Ball* ball, float dt, float arena_half_size_x, float arena_half_size_y) {
    simulate_player(&ball->x, &ball->dp_x, ball->ddp_x, dt, arena_half_size_x, ball->radius);
    simulate_player(&ball->y, &ball->dp_y, ball->ddp_y, dt, arena_half_size_y, ball->radius);
}

    internal boolean aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y, float p2x, float p2y, float hs2x, float hs2y) {
        return (p1x + hs1x > p2x - hs2x &&
            p1x - hs1x < p2x + hs2x &&
            p1y + hs1y > p2y - hs2y &&
            p1y - hs1y < p2y + hs2y);
    }

    internal boolean circle_vs_circle(Ball * ball1, Ball * ball2) {
        return ((ball1->x - ball2->x) * (ball1->x - ball2->x) +
            (ball1->y - ball2->y) * (ball1->y - ball2->y) <
            (ball1->radius + ball2->radius) * (ball1->radius + ball2->radius));
    }

    internal void final_speed(Ball * ball1, Ball * ball2, float* final_px, float* final_py) {
        float relative_dx = ball1->dp_x - ball2->dp_x;
        float relative_dy = ball1->dp_y - ball2->dp_y;
        float relative_x = ball1->x - ball2->x;
        float relative_y = ball1->y - ball2->y;

        if (relative_dx * relative_x + relative_y * relative_dy > 0) {
            return;
        }

        float length = sqrt(relative_x * relative_x + relative_y * relative_y);
        relative_x /= length;
        relative_y /= length;

        float m = (relative_y / relative_x);
        float constant = 1.0 / (1.0 + m * m);

        float relative_dx_back = (relative_dy * 2 * m + relative_dx * (1 - m * m)) * constant;
        float relative_dy_back = (relative_dx * 2 * m + relative_dy * (m * m - 1)) * constant;

        *final_px = -0.8 * relative_dx_back + ball2->dp_x;
        *final_py = -0.8 * relative_dy_back + ball2->dp_y;
    }

    void ball_hit_ball_check(Ball * big_ball, Ball * small_ball) {
        if (circle_vs_circle(big_ball, small_ball)) {
            final_speed(small_ball, big_ball, &small_ball->dp_x, &small_ball->dp_y);
        }
    }




internal void simulate_game(Input* input, float dt) {
    clear_screen(0x000000);
    draw_rect(0x009000, 0, 0, 85, 45);
    draw_rect(0x909000, -90, 0, 5, 17);
    draw_rect(0x909000, 90, 0, 5, 17);

    float player_ddp_1 = 0.f;
    float player_ddp_0 = 0.f;
	ball_2.ddp_x = 0;
    ball_2.ddp_y = 0;
    if (is_down(BUTTON_UP)) ball_2.ddp_y += 500.4;
    if (is_down(BUTTON_DOWN)) ball_2.ddp_y -= 500.4;
    if (is_down(BUTTON_RIGHT)) ball_2.ddp_x += 500.4;
    if (is_down(BUTTON_LEFT)) ball_2.ddp_x -= 500.4;

	simulate_ball(&ball_2, dt, arena_half_size_x, arena_half_size_y);
    ball_hit_ball_check(&ball_2, &ball_1);
    ball_hit_ball_check(&ball_3, &ball_1);
    ball_hit_ball_check(&ball_4, &ball_1);
	ball_hit_ball_check(&ball_5, &ball_1);
	ball_hit_ball_check(&ball_6, &ball_1);

    

    u32 color = 0x00ffff;
    draw_circle(color, ball_2.x, ball_2.y, ball_2.radius);
	draw_circle(color, ball_3.x, ball_3.y, ball_3.radius);
	draw_circle(color, ball_4.x, ball_4.y, ball_4.radius);
	draw_circle(color, ball_5.x, ball_5.y, ball_5.radius);
	draw_circle(color, ball_6.x, ball_6.y, ball_6.radius);

    
    //ball prediction
    {
        float rise_1 = 45 + ball_1.y + (80 - ball_1.x) * ball_1.dp_y / ball_1.dp_x;

        while (rise_1 < 0 || rise_1 > 180) {
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

        if (is_down(BUTTON_W)) player_ddp_0 += 2000;
        if (is_down(BUTTON_S)) player_ddp_0 -= 2000;

        float rise = 45 + ball_1.y + (ball_1.x + 80) * -1.0 * ball_1.dp_y / ball_1.dp_x;

        while (rise < 0 || rise > 180) {
            if (rise < 0) {
                rise += 180;
            }
            else {
                rise -= 180;
            }
        }

        if (rise > 90) {
            predictedball_y = 45 - (rise - 90);
        }
        else {
            predictedball_y = -45 + rise;
        }
    }

    

   

    if (ball_1.y + ball_1.radius > arena_half_size_y) {
        ball_1.y = arena_half_size_y - ball_1.radius;
        ball_1.dp_y *= -0.6;
    }
    if (ball_1.y - ball_1.radius < -arena_half_size_y) {
        ball_1.y = -arena_half_size_y + ball_1.radius;
        ball_1.dp_y *= -0.6;
    }

    if (ball_1.x > arena_half_size_x || ball_1.x < -arena_half_size_x) {

        if (ball_1.y<17 && ball_1.y>-17) {
            if (ball_1.x > 0) {
                player_score_0++;
            }
            else {
                player_score_1++;
            }
            ball_1.dp_y = 0;
            ball_1.dp_x = 0;
            ball_1.x = 0;
            ball_1.y = 0;
        }
        else {

            if (ball_1.x > 0) {
                ball_1.x = arena_half_size_x - ball_1.radius;
                ball_1.dp_x *= -0.6;
            }
            else {
                ball_1.x = -arena_half_size_x + ball_1.radius;
                ball_1.dp_x *= -0.6;
            }

        }
    }

		ball_1.x += ball_1.dp_x * dt;
		ball_1.y += ball_1.dp_y * dt;
		
		draw_number(0x006000, -10, 20, 2, player_score_0);
		draw_number(0x006000, 10, 20, 2, player_score_1);

		draw_circle(0xffffff, ball_1.x, ball_1.y, 1);
		//draw_circle(0x0018090, 200, 300, 200);
		

	}
