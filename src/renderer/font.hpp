#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "texture.hpp"
#include <glm/glm.hpp>
#include <map>
#include <memory>

namespace Engine::Renderer {

    struct Character {
        glm::vec2 size;
        glm::vec2 bearing;
        uint32_t advance;
        glm::vec2 texOffset;
        glm::vec2 texSize;
    };

    struct FTLibraryDeleter {
        void operator()(FT_Library lib) {
            FT_Done_FreeType(lib);
        }
    };
    struct FTFaceDeleter {
        void operator()(FT_Face face) {
            FT_Done_Face(face);
        }
    };

    using FTLibraryPtr = std::unique_ptr<FT_LibraryRec_, FTLibraryDeleter>;
    using FTFacePtr = std::unique_ptr<FT_FaceRec_, FTFaceDeleter>;

    class Font {
      private:
        FTLibraryPtr _library;
        FTFacePtr _face;

        std::map<char, Character> _characters;
        std::unique_ptr<VulkanTexture> _atlas;

      public:
        Font(VkDevice device, VkPhysicalDevice physicalDevice,
             VkCommandPool pool, VkQueue queue, const std::string &path);
        ~Font() = default;

        const Character &getCharacter(char c) const {
            return _characters.at(c);
        }
        const VulkanTexture &getAtlas() const {
            return *_atlas;
        }
    };
} // namespace Engine::Renderer
