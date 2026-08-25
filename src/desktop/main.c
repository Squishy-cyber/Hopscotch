#include <loop.h>
#include <stdio.h>
#include <string.h>

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

    // The real launcher (butterscotch-lf2000) invokes this as:
    //   butterscotch --renderer=legacy-gl --window-size=320x240 <data.win path>
    // i.e. flags first, positional data.win path last. This file
    // originally (incorrectly) assumed argv[1] was always the data path --
    // that's exactly what caused "Failed to open file: --renderer=legacy-gl"
    // on real hardware: argv[1] was the flag string, not a path.
    const char* dataWinPath = NULL;
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--renderer=", 11) == 0) {
            const char* val = argv[i] + 11;
            if (strcmp(val, "legacy-gl") == 0) {
                args.renderer = LEGACY_GL;
            } else if (strcmp(val, "modern-gl") == 0) {
                args.renderer = MODERN_GL;
            } else if (strcmp(val, "software") == 0) {
                args.renderer = SOFTWARE;
            } else {
                fprintf(stderr, "Warning: unrecognized --renderer value '%s', using compile-time default\n", val);
            }
        } else if (strncmp(argv[i], "--window-size=", 14) == 0) {
            int w = 0, h = 0;
            if (sscanf(argv[i] + 14, "%dx%d", &w, &h) == 2 && w > 0 && h > 0) {
                args.windowWidth = w;
                args.windowHeight = h;
            } else {
                fprintf(stderr, "Warning: couldn't parse --window-size value '%s' (expected WxH, e.g. 320x240), using auto\n", argv[i] + 14);
            }
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Warning: unrecognized flag '%s', ignoring\n", argv[i]);
        } else {
            // First non-flag argument is the data.win path.
            dataWinPath = argv[i];
        }
    }

    if (!dataWinPath) {
        fprintf(stderr, "Usage: %s [--renderer=legacy-gl|modern-gl|software] [--window-size=WxH] <data.win path>\n", argv[0]);
        return 1;
    }
    args.dataWinPath = dataWinPath;

    int ret = loop(args, argv[0]);
    freeCommandLineArgs(&args);
    return ret;
}
