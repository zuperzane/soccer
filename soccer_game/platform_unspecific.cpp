struct Button_State {

	bool is_down;
	bool changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_S,
	BUTTON_W,
	BUTTON_A,
	BUTTON_D,
	BUTTON_I,
	BUTTON_J,
	BUTTON_K,
	BUTTON_L,
	BUTTON_T,
	BUTTON_F,
	BUTTON_G,
	BUTTON_H,
	BUTTON_COUNT,

};

struct Input {
	Button_State buttons[BUTTON_COUNT];

};