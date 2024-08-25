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
	BUTTON_COUNT,

};

struct Input {
	Button_State buttons[BUTTON_COUNT];

};