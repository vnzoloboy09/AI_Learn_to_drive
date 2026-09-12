#include <iostream>

#include "app/Application.h"

int main()
{
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 960;
	const char* TITLE = "AI learn to drive";

	Application app(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
	app.Run();

    return 0;
}