#include "simple_painter.h"
#include <cstdio>

//#define DEBUG

enum Shape {line, rec, circle, ellipse};

Window *window = NULL;

LRESULT CALLBACK Window::wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
	if (window) {
		switch(msg) {
		case WM_CREATE:
			// create coordinate and buttons;
			window->create_buttons();
			window->create_coordinate();
		case WM_PAINT:
			// draw shapes
			window->draw();
			break;
		case WM_SIZE:
			// resize
			window->on_resize();
			break;
		case WM_MOUSEMOVE:
			// get mouse position
			window->on_mouse_move();
			break;
		case WM_COMMAND:
			// select shape
			window->on_select(LOWORD(w_param));
			break;
		case WM_HOTKEY:
			window->on_hotkey(LOWORD(w_param));
			break;
		case WM_LBUTTONDOWN:
			window->on_mouse_ldown();
			break;
		case WM_LBUTTONUP:
			window->on_mouse_lup();
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
	}

	return DefWindowProc(hwnd, msg, w_param, l_param);
}

Window::Window(HINSTANCE h_instance, int n_cmdshow) {
	window = this;

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = h_instance;
	wc.lpfnWndProc = wnd_proc;
	wc.lpszClassName = (WCHAR *)my_class_name;
	wc.lpszMenuName = NULL;
	wc.style = 0;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Window registration failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	HWND hwnd_tmp = CreateWindowEx(
		0,
		(WCHAR *)my_class_name,
		L"Simple Painter",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
		150, 150, 480, 480,
		NULL, NULL, h_instance, NULL);

	hwnd = hwnd_tmp;

	if (hwnd == NULL) {
		MessageBox(NULL, L"Window creation failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(-1);
	}

	// create drawing board
	board = CreateWindow(
		(WCHAR *)my_class_name,
		L"",
		WS_CHILD | WS_VISIBLE,
		0, 50, 480, 360,
		hwnd, NULL, NULL, NULL);

	// create drawing list
	draw_list.push_back(new Node);
	now = draw_list.begin();

	// set pens and brush
	pen[line] = CreatePen(PS_SOLID, 20, RGB(255, 0, 0));
	pen[rec] = CreatePen(PS_SOLID, 15, RGB(0, 255, 0));
	pen[circle] = CreatePen(PS_SOLID, 10, RGB(0, 0, 255));
	pen[ellipse] = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
	brush = (HBRUSH)GetStockObject(NULL_BRUSH);

	ShowWindow(hwnd, n_cmdshow);
	InvalidateRect(hwnd, NULL, false);

	// set hotkey
	RegisterHotKey(hwnd, 100, MOD_CONTROL, 'Z');
	RegisterHotKey(hwnd, 101, MOD_CONTROL, 'Y');
}

void Window::on_mouse_ldown() {
	if (mouse.y > 50 && mouse.y < 410) {
		(*now)->shape = shape;
		(*now)->undo = false;
		(*now)->start.x = mouse.x;	(*now)->start.y = mouse.y;
		drawing = true;
	}
}

void Window::on_mouse_lup() {
	if (drawing) {
		(*now)->end.x = mouse.x;	(*now)->end.y = mouse.y;

		auto next = std::next(now);
		if (next == draw_list.end()) {
			draw_list.push_back(new Node);
			now++;
			max_index++;
			now_index++;
		} else {
			now++;
			now_index++;
			(*now)->shape = -1;
			if (next != draw_list.end())
				(*next)->shape = -1;
		}

		drawing = false;
	}
}

void Window::on_resize() {
	GetWindowRect(hwnd, &win_pos);
	GetClientRect(hwnd, &win_size);

	SetWindowPos(
		hwnd,
		NULL,
		0, 0,
		win_pos.right - win_pos.left, win_pos.bottom - win_pos.top,
		SWP_NOMOVE | SWP_NOZORDER
	);

	InvalidateRect(hwnd, NULL, true);
}

void Window::on_select(int selected) {
	switch (shape = selected) {
	case line:
		SetWindowTextW(selected_shape, L"直線");
		break;
	case rec:
		SetWindowTextW(selected_shape, L"方形");
		break;
	case circle:
		SetWindowTextW(selected_shape, L"圓形");
		break;
	case ellipse:
		SetWindowTextW(selected_shape, L"橢圓");
		break;
	default:
		break;
	}
}

void Window::draw() {
	PAINTSTRUCT ps;
	BeginPaint(board, &ps);
	SelectObject(ps.hdc, brush);

	for (auto i: draw_list) {
		if (i->undo)
			break;

		switch (i->shape) {
		case line:
			SelectObject(ps.hdc, pen[line]);
			MoveToEx(ps.hdc, i->start.x, i->start.y - 50, NULL);
			LineTo(ps.hdc, i->end.x, i->end.y - 50);
			break;
		case rec:
			SelectObject(ps.hdc, pen[rec]);
			Rectangle(ps.hdc, i->start.x, i->start.y - 50, i->end.x, i->end.y - 50);
			break;
		case circle:
			long r;
			r = ((i->end.x - i->start.x) > (i->end.y - i->start.y)) ? (i->end.y - i->start.y) : (i->end.x - i->start.x);
			SelectObject(ps.hdc, pen[circle]);
			Ellipse(ps.hdc, i->start.x, i->start.y - 50, i->start.x + r, i->start.y - 50 + r);
			break;
		case ellipse:
			SelectObject(ps.hdc, pen[ellipse]);
			Ellipse(ps.hdc, i->start.x, i->start.y - 50, i->end.x, i->end.y - 50);
			break;
		default:
			break;
		}
	}

	EndPaint(board, &ps);
	Sleep(30);
}

void Window::create_buttons() {
	button[line] = CreateWindowEx(WS_EX_CLIENTEDGE,
		L"button",
		L"直線",
		WS_CHILD | WS_VISIBLE,
		10, 10, 40, 30,
		hwnd, (HMENU)0, NULL, NULL);

	button[rec] = CreateWindowEx(WS_EX_CLIENTEDGE,
		L"button",
		L"方形",
		WS_CHILD | WS_VISIBLE,
		60, 10, 40, 30,
		hwnd, (HMENU)1, NULL, NULL);

	button[circle] = CreateWindowEx(WS_EX_CLIENTEDGE,
		L"button",
		L"圓形",
		WS_CHILD | WS_VISIBLE,
		110, 10, 40, 30,
		hwnd, (HMENU)2, NULL, NULL);

	button[ellipse] = CreateWindowEx(WS_EX_CLIENTEDGE,
		L"button",
		L"橢圓",
		WS_CHILD | WS_VISIBLE,
		160, 10, 40, 30,
		hwnd, (HMENU)3, NULL, NULL);

	CreateWindow(
		L"static",
		L"selected:",
		WS_CHILD | WS_VISIBLE,
		210, 15, 60, 20,
		hwnd, NULL, NULL, NULL);

	selected_shape = CreateWindow(
		L"static",
		L"",
		WS_CHILD | WS_VISIBLE,
		270, 15, 30, 20,
		hwnd, NULL, NULL, NULL);

#ifdef DEBUG
	test = CreateWindow(
		L"static",
		L"",
		WS_CHILD | WS_VISIBLE,
		310, 15, 100, 20,
		hwnd, NULL, NULL, NULL);
#endif // DEBUG
}

void Window::create_coordinate() {
	GetClientRect(hwnd, &win_size);

	// show x coordinate
	CreateWindow(
		L"static",
		L"x : ",
		WS_CHILD | WS_VISIBLE,
		0, win_size.bottom - 20, 20, 20,
		hwnd, NULL, NULL, NULL);

	x = CreateWindow(
		L"static",
		L"150",
		WS_CHILD | WS_VISIBLE,
		20, win_size.bottom - 20, 30, 20,
		hwnd, NULL, NULL, NULL);

	// show y coordinate
	CreateWindow(
		L"static",
		L"y : ",
		WS_CHILD | WS_VISIBLE,
		50, win_size.bottom - 20, 20, 20,
		hwnd, NULL, NULL, NULL);

	y = CreateWindow(
		L"static",
		L"150",
		WS_CHILD | WS_VISIBLE,
		70, win_size.bottom - 20, 30, 20,
		hwnd, NULL, NULL, NULL);
}

void Window::test_action(LPCWSTR buf) {
#ifdef DEBUG
	SetWindowTextW(test, buf);
#endif // DEBUG
}

void Window::get_mouse_pos() {
	// get mouse position
	GetCursorPos(&mouse);
	ScreenToClient(hwnd, &mouse);
}

void Window::undo() {
	if (now != draw_list.begin() && now_index > max_index - 10) {
		now = std::prev(now);
		(*now)->undo = true;
		now_index--;
	}
}

void Window::redo() {
	if ((*now)->shape != -1) {
		(*now)->undo = false;
		now = std::next(now);
		now_index++;
	}
}

void Window::on_hotkey(int hotkey) {
	switch (hotkey) {
	case 100:
		undo();
		break;
	case 101:
		redo();
		break;
	}
	InvalidateRect(board, NULL, true);
}

void Window::on_mouse_move() {
	get_mouse_pos();

	(*now)->end.x = mouse.x;	(*now)->end.y = mouse.y;

	WCHAR buf[10];
	swprintf(buf, 10, L"%ld", mouse.x);
	SetWindowTextW(x, buf);

	swprintf(buf, 10, L"%ld", (mouse.y < 51)?0:(mouse.y > 410)?360:(mouse.y - 50));
	SetWindowTextW(y, buf);

	InvalidateRect(board, NULL, true);
}

Window::~Window() {

}
