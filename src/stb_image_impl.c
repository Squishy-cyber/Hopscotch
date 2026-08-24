// Provides the actual implementations for stb_image / stb_image_write.
//
// vendor/stb/image is only ever added as an include directory in
// CMakeLists.txt (see the repeated `target_include_directories(butterscotch
// PUBLIC vendor/stb/image)` lines) -- nothing ever defines
// STB_IMAGE_IMPLEMENTATION / STB_IMAGE_WRITE_IMPLEMENTATION anywhere, so
// stbi_load_from_memory() (called from src/image/image_decoder.c) and
// stbi_write_png() (called from src/loop.c's writeFramebufferAsPng) both
// resolve to nothing but declarations at link time. This mirrors the
// existing src/stb_ds.c pattern for stb_ds.h.
//
// This file must be placed directly under src/ so the generic
// `file(GLOB SOURCES src/*.c)` in CMakeLists.txt picks it up automatically,
// same as stb_ds.c.

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
