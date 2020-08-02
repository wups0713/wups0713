#ifndef _SIMPLE_PAINTER_H_

#include <Windows.h>
#include <tchar.h>
#include <list>

class Window {
public:
	Window(HINSTANCE, int);
	~Window();
	
	// window process
	static LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);

	// init
	void create_buttons();
	void create_coordinate();

	// actions
	void on_hotkey(int);
	void on_mouse_move();
	void on_mouse_ldown();
	void on_mouse_lup();
	void on_resize();
	void on_select(int);

	// draw
	void undo();
	void redo();
	void draw();

	// tool
	void test_action(LPCWSTR);
	void get_mouse_pos();

private:
	// struct
	typedef struct Node {
		bool undo = false;
		int shape = -1;
		POINT start, end;
	};

	// windows
	HWND hwnd, selected_shape, test;
	HWND board;
	HWND x, y;
	HWND button[4];
	WNDCLASSEX wc;
	const char *my_class_name = {"my_class"};

	// window info
	POINT mouse;
	RECT win_size, win_pos;

	// drawing info
	std::list<Node *> draw_list;
	std::list<Node *>::iterator now;
	int shape = -1;
	bool drawing = false;
	HPEN pen[4];
	HBRUSH brush;

	// undo redo
	int max_index = 0, now_index = 0;
};

#endif // !WINDOWS_H