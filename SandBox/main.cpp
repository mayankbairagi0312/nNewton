#include <iostream>
#include "Core/Application.h"
int main()
{
	//Application DemoTest;
	//if (DemoTest.Initialize()) {
	//	DemoTest.Run();
	//}
	//DemoTest.Shutdown();
	//return 0;



    std::unique_ptr<Application> app;

    try {
        app = std::make_unique<Application>();
    }
    catch (const std::exception& e) {
        std::cerr << "[FATAL] Failed to initialize application: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "[FATAL] Unknown initialization exception!" << std::endl;
        return EXIT_FAILURE;
    }

    int exitCode = EXIT_SUCCESS;
    try {
        exitCode = app->Run();
    }
    catch (const std::exception& e) {
        std::cerr << "[FATAL] Runtime error: " << e.what() << std::endl;
        exitCode = EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "[FATAL] Unknown runtime exception!" << std::endl;
        exitCode = EXIT_FAILURE;
    }

    return exitCode;
}