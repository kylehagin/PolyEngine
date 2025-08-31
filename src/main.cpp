#include "app.h"
#include <iostream>

int main(int argc, char** argv)
{
    bool validation=false;
    for(int i=1;i<argc;++i)
        if (std::string(argv[i])=="--validation") validation=true;

    App app;
    if(!app.Init(validation)) {
        std::cerr << "Failed to initialise\n";
        return 1;
    }
    app.Run();
    app.Shutdown();
    return 0;
}
