#!/usr/bin/env python3
import sys

if len(sys.argv) < 2:
    print("Usage: patch_sw_renderer.py <path_to_sw_renderer.c>")
    sys.exit(1)

path = sys.argv[1]
with open(path, 'r') as f:
    content = f.read()

# 1. Global Texture downscaler (All 2048x2048 sheets become 1024x1024)
if "// Downscale 2x to save memory" not in content:
    target = '\tswr->textures[pageId] = swrCreateTexture(pixels, w, h);\n\tfree(pixels);'
    downscale = '\n\t// Downscale 2x to save memory (screen is only 320x240)\n\tint newW = w / 2;\n\tint newH = h / 2;\n\tif (newW > 0 && newH > 0) {\n\t\tuint8_t* scaled = safeMalloc(newW * newH * 4);\n\t\tfor (int y = 0; y < newH; y++) {\n\t\t\tfor (int x = 0; x < newW; x++) {\n\t\t\t\tint si = (y * 2 * w + x * 2) * 4;\n\t\t\t\tint di = (y * newW + x) * 4;\n\t\t\t\tfor (int c = 0; c < 4; c++) {\n\t\t\t\t\tscaled[di + c] = (\n\t\t\t\t\t\tpixels[si + c] +\n\t\t\t\t\t\tpixels[si + 4 + c] +\n\t\t\t\t\t\tpixels[si + w * 4 + c] +\n\t\t\t\t\t\tpixels[si + w * 4 + 4 + c]\n\t\t\t\t\t) / 4;\n\t\t\t\t}\n\t\t\t}\n\t\t}\n\t\tfree(pixels);\n\t\tpixels = scaled;\n\t\tw = newW;\n\t\th = newH;\n\t}\n'
    content = content.replace(target, downscale + target)

# 2. Inject Absolute Translation into swrDrawSprite
target_draw = "static void swrDrawSprite(\n\tRenderer* renderer, float dx, float dy, float dw, float dh,\n\tSWTexture* texture, int sx, int sy, int sw, int sh,\n\tuint32_t tintColor, float alpha\n)\n{"
gated_draw = "static void swrDrawSprite(\n\tRenderer* renderer, float dx, float dy, float dw, float dh,\n\tSWTexture* texture, int sx, int sy, int sw, int sh,\n\tuint32_t tintColor, float alpha\n)\n{\n\t// Absolute Downscaled Sheet Translation\n\tsx /= 2; sy /= 2; sw /= 2; sh /= 2;\n\tif (sw <= 0) sw = 1;\n\tif (sh <= 0) sh = 1;"
content = content.replace(target_draw, gated_draw)

# 3. Inject Absolute Translation into swrDrawSpriteRotated
target_draw_rot = "static void swrDrawSpriteRotated(\n\tRenderer* renderer, float dx, float dy, float dw, float dh,\n\tSWTexture* texture, int sx, int sy, int sw, int sh,\n\tuint32_t tintColor, float alpha,\n\tfloat angleDeg,\n\tfloat pivotX,\n\tfloat pivotY\n)\n{"
gated_draw_rot = "static void swrDrawSpriteRotated(\n\tRenderer* renderer, float dx, float dy, float dw, float dh,\n\tSWTexture* texture, int sx, int sy, int sw, int sh,\n\tuint32_t tintColor, float alpha,\n\tfloat angleDeg,\n\tfloat pivotX,\n\tfloat pivotY\n)\n{\n\t// Absolute Downscaled Sheet Translation\n\tsx /= 2; sy /= 2; sw /= 2; sh /= 2;\n\tif (sw <= 0) sw = 1;\n\tif (sh <= 0) sh = 1;"
content = content.replace(target_draw_rot, gated_draw_rot)

with open(path, 'w') as f:
    f.write(content)
print('Absolute coordination translation patch written.')