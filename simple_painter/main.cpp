#include "simple_painter.h"

int WINAPI WinMain(
	HINSTANCE h_instance,
	HINSTANCE h_previnstance,
	LPSTR lp_cmdline,
	int n_cmdshow) {
	
	MSG msg;
	Window window (h_instance, n_cmdshow);

	while (GetMessage (&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}