#pragma once
#include "../core/macros.h"
#include "../vendor/stb/stb_truetype.h"
#include "../vulkan/vk_image.h"
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>
namespace cui::renderer {

#define MAX_TEXTURE_SIZE 2048
#define MAX_GLYPH_COUNT 220
struct character {
  uint32_t x_0;
  uint32_t x_1;
  uint32_t y_0;
  uint32_t y_1;
  float advance;
  int char_index;
};

class font {
public:
  vulkan::vk_image atlas;

  std::unordered_map<char, character> font_data;

  void load(const std::string path) {

    FILE *font_file = fopen(path.c_str(), "rb");

    fseek(font_file, 0, SEEK_END);
    uint32_t size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    std::vector<unsigned char> buffer;
    buffer.resize(size);

    fread(buffer.data(), size, 1, font_file);
    fclose(font_file);

    stbtt_fontinfo info;
    ASSERT(!stbtt_InitFont(&info, buffer.data(),
                           stbtt_GetFontOffsetForIndex(buffer.data(), 0)),
           "Error, font reading error.", TEXT_COLOR_ERROR);

    // setup glyph info stuff, check stb_truetype.h for definition of structs
    std::vector<stbtt_packedchar> glyph_metrics;
    glyph_metrics.resize(MAX_GLYPH_COUNT);

    std::vector<stbtt_pack_range> ranges = {
        {72, 32, NULL, MAX_GLYPH_COUNT, glyph_metrics.data(), 0, 0}};

    int width = MAX_TEXTURE_SIZE;
    int max_height = MAX_TEXTURE_SIZE;
    std::vector<unsigned char> bitmap_buffer(max_height * width);

    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, bitmap_buffer.data(), width, max_height, 0, 1, NULL);
    stbtt_PackSetOversampling(
        &pc, 1, 1); // say, choose 3x1 oversampling for subpixel positioning
    stbtt_PackFontRanges(&pc, buffer.data(), 0, ranges.data(), 1);
    stbtt_PackEnd(&pc);

    int filled = 0;
    int height = 0;
    for (int i = 0; i < MAX_GLYPH_COUNT; i++) {
      
      font_data[i + 32] = character{
          glyph_metrics[i].x0, glyph_metrics[i].x1,       glyph_metrics[i].y0,
          glyph_metrics[i].y1, glyph_metrics[i].xadvance, i + 32};
      if (glyph_metrics[i].y1 > height)
        height = glyph_metrics[i].y1;
      filled += (glyph_metrics[i].x1 - glyph_metrics[i].x0) *
                (glyph_metrics[i].y1 - glyph_metrics[i].y0);
    }

    // Texture
    std::vector<char> image_buffer(width * max_height * 4);
    for (int i = 0; i < max_height * width; i++) {
      image_buffer[i * 4 + 0] |= bitmap_buffer[i];
      image_buffer[i * 4 + 1] |= bitmap_buffer[i];
      image_buffer[i * 4 + 2] |= bitmap_buffer[i];
      image_buffer[i * 4 + 3] = 0xff;
    }
    atlas = vulkan::vk_image((void *)image_buffer.data(), width, max_height,
                             "DEVICE_KHR", VK_IMAGE_TILING_LINEAR);
  }
};

/*


  void load(const std::string path) {

    FILE *font_file = fopen(path.c_str(), "rb");

    fseek(font_file, 0, SEEK_END);
    uint32_t size = ftell(font_file);
    fseek(font_file, 0, SEEK_SET);

    std::vector<unsigned char> buffer;
    buffer.resize(size);

    fread(buffer.data(), size, 1, font_file);
    fclose(font_file);

    stbtt_fontinfo info;
    ASSERT(!stbtt_InitFont(&info, buffer.data(), 0),
           "Error, font reading error.", TEXT_COLOR_ERROR);

    const uint32_t glyph_size = 220;
    const uint32_t glyph_per_column = MAX_TEXTURE_SIZE / glyph_size;

    const uint32_t column_count = (info.numGlyphs / glyph_per_column) + 1;

    const uint32_t size_x = MAX_TEXTURE_SIZE;
    const uint32_t size_y = column_count * glyph_size;

    std::vector<unsigned char> bit_map_buffer;
    bit_map_buffer.resize(size_x * size_y);

    const float scale = stbtt_ScaleForPixelHeight(&info, glyph_size);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    ascent *= scale;
    descent *= scale;

    int pen_x = 0;
    int pen_y = 0;

    unsigned int codepoint = 0;
    int glyphIndex;

    while (codepoint <= 0x10FFFF) {
      glyphIndex = stbtt_FindGlyphIndex(&info, codepoint);
      if (!stbtt_IsGlyphEmpty(&info,glyphIndex)) {
        int ax;
        int lsb;
        stbtt_GetCodepointHMetrics(&info, glyphIndex, &ax, &lsb);

        if (pen_x + ax >= MAX_TEXTURE_SIZE) {
          pen_x = 0;
          pen_y += glyph_size;
        }

        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&info, glyphIndex, scale, scale, &c_x1,
  &c_y1, &c_x2, &c_y2);

        int byteOffset = pen_x + roundf(lsb * scale) + (pen_y * size_x);
        stbtt_MakeCodepointBitmap(
            &info, bit_map_buffer.data() + byteOffset, c_x2 - c_x1,
            c_y2 - c_y1, size_x, scale, scale, glyphIndex);

        pen_x += ax * scale;
      }
      // Increment to the next codepoint
      codepoint++;
    }

    std::vector<char> image_buffer(size_x * size_y * 4);
    for (int i = 0; i < size_x * size_y; i++) {
      image_buffer[i * 4 + 0] |= bit_map_buffer[i];
      image_buffer[i * 4 + 1] |= bit_map_buffer[i];
      image_buffer[i * 4 + 2] |= bit_map_buffer[i];
      image_buffer[i * 4 + 3] = 0xff;
    }
    atlas = vulkan::vk_image((void *)image_buffer.data(), size_x, size_y,
                             "DEVICE_KHR", VK_IMAGE_TILING_LINEAR);
  }
*/

} // namespace cui::renderer
