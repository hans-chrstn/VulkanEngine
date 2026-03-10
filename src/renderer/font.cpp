#include "font.hpp"
#include "core/logger.hpp"

namespace Engine::Renderer {
    Font::Font(VkDevice device, VkPhysicalDevice physicalDevice,
               VkCommandPool pool, VkQueue queue, const std::string &path) {
        FT_Library rawLib;
        if (FT_Init_FreeType(&rawLib)) {
            ENGINE_FATAL("Failed to initialize FreeType library");
        }
        _library.reset(rawLib);

        FT_Face rawFace;
        if (FT_New_Face(_library.get(), path.c_str(), 0, &rawFace)) {
            ENGINE_FATAL("Failed to load font: " + path);
        }
        _face.reset(rawFace);

        FT_Set_Pixel_Sizes(_face.get(), 0, 48);
        uint32_t atlasWidth = 1024;
        uint32_t atlasHeight = 1024;
        std::vector<uint8_t> atlasData(atlasWidth * atlasHeight, 0);

        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t maxHeightInRow = 0;

        for (unsigned char c = 32; c < 128; c++) {
            if (FT_Load_Char(_face.get(), c, FT_LOAD_RENDER)) {
                continue;
            }

            auto &glyph = _face->glyph;

            if (x + glyph->bitmap.width >= atlasWidth) {
                x = 0;
                y += maxHeightInRow + 2;
                maxHeightInRow = 0;
            }

            for (uint32_t row = 0;
                 y + row < atlasHeight && row < glyph->bitmap.rows; row++) {
                for (uint32_t col = 0;
                     x + col < atlasWidth && col < glyph->bitmap.width; col++) {
                    atlasData[(y + row) * atlasWidth + (x + col)] =
                        glyph->bitmap.buffer[row * glyph->bitmap.pitch + col];
                }
            }

            Character character = {
                glm::vec2(glyph->bitmap.width, glyph->bitmap.rows),
                glm::vec2(glyph->bitmap_left, glyph->bitmap_top),
                static_cast<uint32_t>(glyph->advance.x),
                glm::vec2((float)x / atlasWidth, (float)y / atlasHeight),
                glm::vec2((float)glyph->bitmap.width / atlasWidth,
                          (float)glyph->bitmap.rows / atlasHeight),
            };
            _characters[c] = character;

            x += glyph->bitmap.width + 2;
            maxHeightInRow = std::max(maxHeightInRow, glyph->bitmap.rows);
        }

        _atlas = std::make_unique<VulkanTexture>(
            device, physicalDevice, pool, queue, atlasData.data(), atlasWidth,
            atlasHeight, VK_FORMAT_R8_UNORM);
    }
} // namespace Engine::Renderer
