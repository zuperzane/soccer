#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#include <cmath>
using namespace std;

typedef struct {
    float x, y;
    float radius;
    float dp_x, dp_y;
    float ddp_x, ddp_y;
    int team;
} Ball;

typedef struct {
    Ball *balls[6]; // Adjust the size as needed
    int friends;
} Team;

float player_pos_0 = 0.f;
float player_pos_1 = -12.f;
float player_p_1, player_dp_1, player_p_0, player_dp_0;
float arena_half_size_x = 85, arena_half_size_y = 45;
float player_half_size_x = 2.5, player_half_size_y = 12;

int player_score_0 = 0;
int player_score_1 = 0;
int predictedball_y = 0;

float bot_radius = 3.0f;

Ball ball_1 = { 0, 0, 1, 12, 21, 0, 0 };
Ball ball_2 = { 20, 20, bot_radius, 0, 0, 0, 0,1 };
Ball ball_3 = { -21, 28, bot_radius, 0, 0, 0, 0,1 };
Ball ball_4 = { -20, -30, bot_radius, 0, 0, 0, 0,1 };
Ball ball_5 = { -61, 3, bot_radius, 0, 0, 0, 0 ,1};
Ball ball_6 = { -20, 1, bot_radius, 0, 0, 0, 0,1 };
Ball ball_7 = { 21, 31, bot_radius, 0, 0, 0, 0,1 };
Ball ball_8 = { 20, -31, bot_radius, 0, 0, 0, 0,1 };
Ball ball_9 = { 61, -2, bot_radius, 0, 0, 0, 0 ,1 };
Ball post_1 = { -86, 16, bot_radius, 0, 0, 0, 0 ,1 };
Ball post_2 = { -86, -16, bot_radius, 0, 0, 0, 0 ,1 };
Ball post_3 = { 86, 16, bot_radius, 0, 0, 0, 0 ,1 };
Ball post_4 = { 86, -16, bot_radius, 0, 0, 0, 0 ,1 };



Team team1 = { {&ball_2, &ball_3, &ball_4, &ball_5,&post_3,&post_4}, 0 };
Team team2 = { {&ball_6, &ball_7, &ball_8, &ball_9,&post_1,&post_2}, 1 };

Ball* all_balls[] = { &ball_2, &ball_3, &ball_4, &ball_5, &ball_6, &ball_7, &ball_8, &ball_9 };
bool catch_ball_1 = false;
//functions hitting stuff like that
int size_of_team=4;

internal void simulate_player(float* p, float* dp, float ddp, float dt, float arena_half_size, float radius) {
    ddp -= *dp * 7;
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



internal void simulate_player_s(float* p, float* dp, float ddp, float dt, float arena_half_size, float radius) {
    ddp -= *dp * 0.9;
    *p = *p + *dp * dt + ddp * dt * dt * 0.5f;
    *dp = *dp + ddp * dt;

    
}

internal void simulate_ball_s(Ball* ball, float dt, float arena_half_size_x, float arena_half_size_y) {
    simulate_player_s(&ball->x, &ball->dp_x, ball->ddp_x, dt, arena_half_size_x, ball->radius);
    simulate_player_s(&ball->y, &ball->dp_y, ball->ddp_y, dt, arena_half_size_y, ball->radius);
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

    internal boolean circle_vs_circle_hold(Ball* ball1, Ball* ball2) {
        return ((ball1->x - ball2->x) * (ball1->x - ball2->x) +
            (ball1->y - ball2->y) * (ball1->y - ball2->y) <
            (ball1->radius + ball2->radius+10.0) * (ball1->radius + ball2->radius+10.0));
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

    internal void big_final_speed(Ball* ball1, Ball* ball2) {
        float relative_dx = ball1->dp_x - ball2->dp_x;
        float relative_dy = ball1->dp_y - ball2->dp_y;
        float relative_x = ball1->x - ball2->x;
        float relative_y = ball1->y - ball2->y;
        
        if (relative_dx * relative_x + relative_y * relative_dy > 0) {
            return;
        }

        float vs_ss = (relative_dx * relative_x+relative_dy*relative_y)/((relative_x * relative_x + relative_y * relative_y));

        float final_add_x=vs_ss*relative_x;
        float final_add_y=vs_ss*relative_y;

        ball1->dp_x = -1.1 * final_add_x + ball1->dp_x;
        ball1->dp_y = -1.1 * final_add_y + ball1->dp_y;

        ball2->dp_x = 1.1 * final_add_x + ball2->dp_x;
        ball2->dp_y = 1.1 * final_add_y + ball2->dp_y;
    }



   internal void ball_hit_ball_check(Ball * big_ball, Ball * small_ball) {
        if (circle_vs_circle(big_ball, small_ball)) {
            final_speed(small_ball, big_ball, &small_ball->dp_x, &small_ball->dp_y);
        }
    }

  internal void big_ball_hit_ball_check(Ball* big_ball, Ball* small_ball) {
        if (circle_vs_circle(big_ball, small_ball)) {
            big_final_speed(small_ball, big_ball);
        }
    }

  internal void ball_connect(Ball* ball_1, Ball* ball_2, u32 color) {
      draw_rotate_rect(color, (ball_1->x + ball_2->x) / 2.0, (ball_1->y + ball_2->y) / 2.0, 0.5,
          sqrt((ball_1->x - ball_2->x) * (ball_1->x - ball_2->x) + (ball_1->y - ball_2->y) * (ball_1->y - ball_2->y)) / 2.0,
          -atan((ball_1->x - ball_2->x) / (ball_1->y - ball_2->y)));
  }

  internal void catch_ball(Ball* ball_1, Ball* ball_2, float radians) {
	  ball_1->x = ball_2->x+4.0*sin(radians);
	  ball_1->y = ball_2->y+4.0*cos(radians);
	  ball_1->dp_x = ball_2->dp_x;
	  ball_1->dp_y = ball_2->dp_y;
  }

  internal boolean is_too_close(Ball* ball_1, Ball* ball_2, Ball* opp) {
      float slope = (ball_1->y - ball_2->y) / (ball_1->x - ball_2->x);
      if (slope == 0.0) {
          slope = 0.01;
      }
      float inversion = -1.0 / slope;

      float x_intercept = (slope * ball_1->x - ball_1->y - inversion * opp->x + opp->y) / (slope - inversion);
      float y_intercept = slope * (x_intercept - ball_1->x) + ball_1->y;
      float distance_pass = sqrt((ball_1->x - x_intercept) * (ball_1->x - x_intercept) + (ball_1->y - y_intercept) * (ball_1->y - y_intercept));
      float distance_intercept = sqrt((opp->x - x_intercept) * (opp->x - x_intercept) + (opp->y - y_intercept) * (opp->y - y_intercept))-3.0;
      if (distance_intercept < 0.2 * distance_pass) {
          if ((x_intercept < ball_1->x && x_intercept < ball_2->x) || (x_intercept > ball_1->x && x_intercept > ball_2->x)) {
              return false;
          }
          return true;
      }
      return false;
  }




  internal float how_too_close(Ball* ball_1, Ball* ball_2, Ball* opp) {
       

	  float distance_ball_1_ball_2 = sqrt((ball_1->x - ball_2->x) * (ball_1->x - ball_2->x) + (ball_1->y - ball_2->y) * (ball_1->y - ball_2->y));
	  if (distance_ball_1_ball_2 < 35.0) {
		  return -800.0/ distance_ball_1_ball_2;
	  }


      float slope = (ball_1->y - ball_2->y) / (ball_1->x - ball_2->x);
      if (slope == 0.0) {
          slope = 0.01;
      }
      float inversion = -1.0 / slope;

      float x_intercept = (slope * ball_1->x - ball_1->y - inversion * opp->x + opp->y) / (slope - inversion);
      float y_intercept = slope * (x_intercept - ball_1->x) + ball_1->y;
      float distance_pass = sqrt((ball_1->x - x_intercept) * (ball_1->x - x_intercept) + (ball_1->y - y_intercept) * (ball_1->y - y_intercept));
      float distance_intercept = sqrt((opp->x - x_intercept) * (opp->x - x_intercept) + (opp->y - y_intercept) * (opp->y - y_intercept)) - 3.0;

      


      if ((x_intercept < ball_1->x && x_intercept < ball_2->x) || (x_intercept > ball_1->x && x_intercept > ball_2->x)|| (y_intercept < ball_1->y && y_intercept < ball_2->y) || (y_intercept > ball_1->y && y_intercept > ball_2->y)) {
          return PI;
      }

      float ABx = ball_1->x - ball_2->x;
      float ABy = ball_1->y - ball_2->y;
      float ACx = opp->x - ball_2->x;
      float ACy = opp->y - ball_2->y;

      // Dot product and magnitudes
      float dotProduct = ABx * ACx + ABy * ACy;
      float magnitudeAB = std::sqrt(ABx * ABx + ABy * ABy);
      float magnitudeAC = std::sqrt(ACx * ACx + ACy * ACy);

      // Cosine of the angle
      float cosTheta = dotProduct / (magnitudeAB * magnitudeAC);

      // Angle in radians
      return std::acos(cosTheta);


  }

  internal void add_orbit_force(Ball* big_ball, Ball* small_ball, float orbit_radius, float radians) {
      // Calculate the desired position on the orbit
      float target_x = big_ball->x + orbit_radius * cos(radians);
      float target_y = big_ball->y + orbit_radius * sin(radians);

      // Calculate the difference between the current position and the target position
      float diff_x = target_x - small_ball->x;
      float diff_y = target_y - small_ball->y;

      // Calculate the force to apply (simple proportional controller)
      float force_x = diff_x * 5000.0; // Adjust the constant as needed
      float force_y = diff_y * 5000.0; // Adjust the constant as needed

      if (force_x < 0) {
          force_x = -1.0 * sqrt(-1.0*force_x);
      }
      else {
          force_x = sqrt(force_x);
      }

      if (force_y < 0) {
          force_y = -1.0 * sqrt(-1.0 * force_y);
      }
      else {
          force_y = sqrt(force_y);
      }

      


      

      // Apply the force to the small ball's acceleration
      small_ball->ddp_x += force_x;
      small_ball->ddp_y += force_y;
  }



  internal 
  void check_connections(Team* team1, Team* team2) {
    for (int i = 0; i < 6; ++i) {
        for (int j = i+1; j < 6; ++j) {
            bool can_pass_1_to_2 = true;
            bool can_pass_2_to_1 = true;

            for (int k = 0; k < 4; ++k) {
                if (is_too_close(team1->balls[i], team1->balls[j], team2->balls[k])) {
                    can_pass_1_to_2 = false;
                }
                if (is_too_close(team1->balls[j], team1->balls[i], team2->balls[k])) {
                    can_pass_2_to_1 = false;
                }
            }

            unsigned int color;
            if (can_pass_1_to_2 && can_pass_2_to_1) {
                color = 0x00ff00; // Green

                ball_connect(team1->balls[i], team1->balls[j], color);
            } else if (!can_pass_1_to_2 && !can_pass_2_to_1) {
                color = 0x008d00; // Dark Green
            } else if (can_pass_1_to_2) {
                color = (team1->balls[i]->x < team1->balls[j]->x) ? 0x0000ff : 0xffa500; // Blue or Orange

                ball_connect(team1->balls[i], team1->balls[j], color);
            } else {
                color = (team1->balls[j]->x < team1->balls[i]->x) ? 0x0000ff : 0xffa500; // Blue or Orange

                ball_connect(team1->balls[i], team1->balls[j], color);
            }

        }
    }
}





internal void simulate_game(Input* input, float dt) {
    clear_screen(0x000000);
    draw_rect(0x009000, 0, 0, 85, 45);


    float player_ddp_1 = 0.f;
    float player_ddp_0 = 0.f;
	ball_1.ddp_x = 0;
    ball_1.ddp_y = 0;
    ball_2.ddp_y = 0;
	ball_2.ddp_x = 0;
    ball_2.ddp_y = 0;
	ball_3.ddp_x = 0;
	ball_3.ddp_y = 0;
	ball_4.ddp_x = 0;
	ball_4.ddp_y = 0;
	ball_5.ddp_x = 0;
	ball_5.ddp_y = 0;
	ball_6.ddp_x = 0;
	ball_6.ddp_y = 0;
	ball_7.ddp_x = 0;
	ball_7.ddp_y = 0;
	ball_8.ddp_x = 0;
	ball_8.ddp_y = 0;
	ball_9.ddp_x = 0;
	ball_9.ddp_y = 0;


    //controls
    {
        if (is_down(BUTTON_UP)) ball_2.ddp_y += 500.4;
        if (is_down(BUTTON_DOWN)) ball_2.ddp_y -= 500.4;
        if (is_down(BUTTON_RIGHT)) ball_2.ddp_x += 500.4;
        if (is_down(BUTTON_LEFT)) ball_2.ddp_x -= 500.4;
        if (is_down(BUTTON_W)) ball_3.ddp_y += 500.4;
        if (is_down(BUTTON_S)) ball_3.ddp_y -= 500.4;
        if (is_down(BUTTON_D)) ball_3.ddp_x += 500.4;
        if (is_down(BUTTON_A)) ball_3.ddp_x -= 500.4;
        if (is_down(BUTTON_I)) ball_4.ddp_y += 500.4;
        if (is_down(BUTTON_K)) ball_4.ddp_y -= 500.4;
        if (is_down(BUTTON_L)) ball_4.ddp_x += 500.4;
        if (is_down(BUTTON_J)) ball_4.ddp_x -= 500.4;
        if (is_down(BUTTON_T)) ball_5.ddp_y += 500.4;
        if (is_down(BUTTON_G)) ball_5.ddp_y -= 500.4;
        if (is_down(BUTTON_H)) ball_5.ddp_x += 500.4;
        if (is_down(BUTTON_F)) ball_5.ddp_x -= 500.4;
    }
	

    
   



    draw_number(0x006000, -10, 38, 2, player_score_0);
    draw_number(0x006000, 10, 38, 2, player_score_1);

    u32 color = 0x00ffff;
    u32 color_2 = 0xff0000;
    
    
 
    
    for (int i = 0; i < size_of_team; i++) {
 
        float curr_array[6] = { PI/2.0,PI/2.0,PI/2.0,PI/2.0,PI/2.0,PI / 2.0 };
        float up_array[6] = { PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0 ,PI / 2.0 };
        float right_array[6] = { PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0 };
        float down_array[6] = { PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0 ,PI / 2.0 };
        float left_array[6] = { PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0,PI / 2.0 ,PI / 2.0 };

        curr_array[i] = 0.0;
        up_array[i] = 0.0;
        right_array[i] = 0.0;
        left_array[i] = 0.0;
        down_array[i] = 0.0;

        for (int j = 0; j < size_of_team + 2; j++) {
            
            for (int k = 0; k < size_of_team; k++) {
                if (i == j) {
    

                }
                else {
                    
                    
                    curr_array[j] = min(how_too_close(team2.balls[i], team2.balls[j], team1.balls[k]), curr_array[j]);
                    team2.balls[i]->x += 1.0;
                    right_array[j] =  min(how_too_close(team2.balls[i], team2.balls[j], team1.balls[k]), right_array[j]);
                    team2.balls[i]->x -= 1.0;
                    team2.balls[i]->y += 1.0;
                    up_array[j] = min(how_too_close(team2.balls[i], team2.balls[j], team1.balls[k]), up_array[j]);
                    team2.balls[i]->y -= 1.0;
					team2.balls[i]->x -= 1.0;
					left_array[j] = min(how_too_close(team2.balls[i], team2.balls[j], team1.balls[k]), left_array[j]);
					team2.balls[i]->x += 1.0;
					team2.balls[i]->y -= 1.0;
					down_array[j] = min(how_too_close(team2.balls[i], team2.balls[j], team1.balls[k]), down_array[j]);
					team2.balls[i]->y += 1.0;


                }
            }

                        

        
        }


        float curr_sum = 0.0;
        for(int z = 0; z < size_of_team + 2; z++) {
            curr_sum += curr_array[z];
        
        }

        float right_sum = 0.0;
        for(int z = 0; z < size_of_team + 2; z++) {
            right_sum += right_array[z];

        }


        float up_sum = 0.0;
        for(int z = 0; z < size_of_team + 2; z++) {
            up_sum += up_array[z];

        }
		float left_sum = 0.0;
        for (int z = 0; z < size_of_team + 2; z++) {
            left_sum += left_array[z];
        }

		float down_sum = 0.0;
		for (int z = 0; z < size_of_team + 2; z++) {
			down_sum += down_array[z];
		}

        if (up_sum < 0 || down_sum < 0 || curr_sum < 0 || right_sum < 0 || left_sum < 0) {
        
            int q = 0;
        }

        float right = right_sum - left_sum;
        float up = up_sum - down_sum;
        float hypotenuse=sqrt((right)*(right)+(up)*(up));
        if (hypotenuse<0.001) {
        
        }
        else {

            if (curr_sum > right_sum&&curr_sum>left_sum) {
            }
            else {
                team2.balls[i]->ddp_x += right * 300.0 / hypotenuse;
            }
			if (curr_sum > up_sum && curr_sum > down_sum) {
			}
			else {
				team2.balls[i]->ddp_y += up * 300.0 / hypotenuse;
			}
        }
    }


    {

        for (int i = 0; i < 8; ++i) {
            simulate_ball(all_balls[i], dt, arena_half_size_x, arena_half_size_y);
            ball_hit_ball_check(all_balls[i], &ball_1);

        }

        


        for (int i = 0; i < 8; ++i) {
            for (int j = i + 1; j < 8; ++j) {
                big_ball_hit_ball_check(all_balls[i], all_balls[j]);
            }
        }


        if (circle_vs_circle_hold(&ball_1, &ball_2)) {
           
            if (catch_ball_1==false) {
				catch_ball(&ball_1, &ball_2, atan2((ball_1.x - ball_2.x) , (ball_1.y - ball_2.y)));
				catch_ball_1 = true;
            
            }
            else {
                add_orbit_force(&ball_2, &ball_1, 4.5, 0.2);
            }
        }

        simulate_ball_s(&ball_1, dt, arena_half_size_x, arena_half_size_y);
    }

    for (int i = 0; i < sizeof(all_balls) / sizeof(all_balls[0]); ++i) {
        u32 current_color = (i < sizeof(all_balls) / sizeof(all_balls[0]) / 2) ? color : color_2;
        draw_circle(current_color, all_balls[i]->x, all_balls[i]->y, all_balls[i]->radius);
    }
    
    /*
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

    */

   

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

   


   // check_connections(&team1, &team2);
    check_connections(&team2, &team1);

    draw_rect(0x909000, -90, 0, 5, 17);
    draw_rect(0x909000, 90, 0, 5, 17);
    draw_circle(0xffffff, ball_1.x, ball_1.y, 1);

		//draw_circle(0x0018090, 200, 300, 200);
		

	}
