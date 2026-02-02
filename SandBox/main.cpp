#include <iostream>
#include "Core/Application.h"
int main()
{
	Application DemoTest;
	if (DemoTest.Initialize()) {
		DemoTest.Run();
	}
	DemoTest.Shutdown();
	return 0;
}