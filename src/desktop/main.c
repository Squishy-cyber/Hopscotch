#include <loop.h>
#include <stdio.h>

/* For SDL_main */
#if defined(USE_SDL1)
#include <SDL/SDL_main.h>
#elif defined(USE_SDL2)
#include <SDL2/SDL_main.h>
#elif defined(USE_SDL3)
#include <SDL3/SDL_main.h>
#endif

int main(int argc, char* argv[]) {
    setbuf(stderr, NULL);

    CommandLineArgs args = {0};
    args.exitAtFrame = -1;
#ifdef ENABLE_VM_TRACING
    args.traceBytecodeAfterFrame = 0;
#endif
    args.speedMultiplier = 1.0;
    args.fastForwardSpeed = 0.0;
    args.osType = OS_WINDOWS;
    args.profilerFramesBetween = 0;
    args.loadType = DATAWINLOADTYPE_LOAD_PER_CHUNK;
    args.lazyRooms = true;
    args.lazyTextures = true;
    args.lazyAudio = true;
#if defined(ENABLE_MODERN_GL)
    args.renderer = MODERN_GL;
#elif defined(ENABLE_LEGACY_GL)
    args.renderer = LEGACY_GL;
#else
    args.renderer = SOFTWARE;
#endif

    // Unlike src/embedded/main.c, "desktop" has no DATA_WIN_PATH compile
    // definition anywhere in CMakeLists.txt, so take the data.win path as
    // the first command-line argument instead, matching how a normal
    // desktop game engine takes a game file path.
    //
    // If the Leapster launches this binary with a fixed invocation and no
    // arguments (e.g. from an init script), swap this out for either a
    // hardcoded absolute path to wherever data.win lives on the device's
    // filesystem, or add a DATA_WIN_PATH compile definition to
    // CMakeLists.txt for this platform and use that instead, mirroring the
    // embedded target's pattern.
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <data.win path>\n", argv[0]);
        return 1;
    }
    args.dataWinPath = argv[1];

    int ret = loop(args, argv[0]);
    freeCommandLineArgs(&args);
    return ret;
}
