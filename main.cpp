#include "services/app_manager.h"

#ifdef __WIN32__
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

int main(int argc, char *argv[])
{
#if __WIN32__
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
#endif

    AppManager app;

    return app.start(argc, argv);
}
