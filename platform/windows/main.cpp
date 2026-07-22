#include <iostream>
#include <engine/application/application.h>

int main()
{
    std::cout << "Hello, World\n";

    ApplicationCI ci;
    ci.title = "NEW GAME";
    ci.w = 1280;
    ci.h = 720;

    Application app;
    app.create(ci);
    return app.run();

    return 0;
}