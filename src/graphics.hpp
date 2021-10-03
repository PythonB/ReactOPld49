#include <string>
#include <vector>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace graphics {
    struct gtexture {
        unsigned int data[256];
    };
    gtexture load_gtexture_from_file(std::string path);
    gtexture mix_gtextures(gtexture fg, gtexture bg);
    void texture_from_gtexture(SDL_Texture* stextr, gtexture gtextr);
    class gamewindow {
        public:
            SDL_Window *sdlwindow;
            SDL_Renderer *renderer;
            int get_width();
            int get_height();
            gamewindow();
            gamewindow(std::string title, unsigned int width, unsigned int height);
            ~gamewindow();
    };
    class tile {
        public:
            SDL_Texture *texture = NULL;
            SDL_Rect render_rectangle;
            unsigned char get_size_x();
            unsigned char get_size_y();
            unsigned char get_render_size_x();
            unsigned char get_render_size_y();
            void place(int newx, int newy);
            void render(gamewindow& win);
            tile();
            tile(gamewindow& win, gtexture gtextr);
            ~tile();
        private:
            int size_x;
            int size_y;
    };
    class metatile {
        public:
            SDL_Texture* texture;
            SDL_Rect render_rectangle;
            unsigned char get_size_x();
            unsigned char get_size_y();
            void place(int newx, int newy);
            void render(gamewindow& win);
            metatile();
            metatile(gamewindow& win, const std::vector<gtexture>& tilemap, int tsize_x, int tsize_y);
            ~metatile();
        private:
            int size_x;
            int size_y;
    };
}