#include "graphics.hpp"

graphics::gtexture graphics::load_gtexture_from_file(std::string path){
    gtexture gtextr;
    std::ifstream fstr(path, std::ios::binary);
    for(int i = 0; i < 256; i++){
        int value;
        fstr.read(reinterpret_cast<char*>(&value), sizeof(int));
        gtextr.data[i] = value;
    }
    return gtextr;
}
graphics::gtexture graphics::mix_gtextures(graphics::gtexture fg, graphics::gtexture bg){
	graphics::gtexture result;
	for(int i = 0; i < 256; i++){
		if((fg.data[i]>>24) == 0x00){
			result.data[i] = bg.data[i];
		} else {
			result.data[i] = fg.data[i];
		}
	}
	return result;
}
void graphics::texture_from_gtexture(SDL_Texture* stextr, gtexture gtextr){
    SDL_UpdateTexture(stextr, NULL, gtextr.data, 64);
}
/*graphics::gtexture mix_gtextures(graphics::gtexture gtextr_bg, graphics::gtexture gtextr_fg){
    graphics::gtexture g;
    for(int i = 0; i < 256; i++){
        if((gtextr_fg.data[i]<<24)>>24 == 0){
            g.data[i] = gtextr_bg.data[i];
        } else {
            g.data[i] = gtextr_fg.data[i];
        }
    }
    return g;
}*/

int graphics::gamewindow::get_width(){
    int w, h;
    SDL_GetWindowSize(sdlwindow, &w, &h);
    return w;
}
int graphics::gamewindow::get_height(){
    int w, h;
    SDL_GetWindowSize(sdlwindow, &w, &h);
    return h;
}
graphics::gamewindow::gamewindow(){
    //placeholder
}
graphics::gamewindow::gamewindow(std::string title, unsigned int width, unsigned int height){
    sdlwindow = SDL_CreateWindow(title.data(), 100, 100, width, height, 0);
    renderer = SDL_CreateRenderer(sdlwindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 320, 240);
}
graphics::gamewindow::~gamewindow(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(sdlwindow);
}
void graphics::tile::place(int newx, int newy){
    render_rectangle.x = newx;
    render_rectangle.y = newy;
}
void graphics::tile::render(gamewindow &win){
    SDL_RenderCopy(win.renderer, texture, NULL, &render_rectangle);
}
graphics::tile::tile(){
    // placeholder
}
graphics::tile::tile(gamewindow& win, gtexture gtextr){
    texture = SDL_CreateTexture(win.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 16, 16);
    SDL_UpdateTexture(texture, NULL, gtextr.data, 64);
    render_rectangle.x = 0; 
    render_rectangle.y = 0; 
    render_rectangle.w = (win.get_width()/320)*5; 
    render_rectangle.h = (win.get_height()/240)*5;
    SDL_QueryTexture(texture, NULL, NULL, &size_x, &size_y);
}
graphics::tile::~tile(){
    SDL_DestroyTexture(texture);
}
void graphics::metatile::place(int newx, int newy){
    render_rectangle.x = newx;
    render_rectangle.y = newy;
}
void graphics::metatile::render(gamewindow &win){
    SDL_RenderCopy(win.renderer, texture, NULL, &render_rectangle);
}
graphics::metatile::metatile(){
    // placeholder
}
graphics::metatile::metatile(gamewindow& win, const std::vector<gtexture>& tilemap, int tsize_x, int tsize_y){
    texture = SDL_CreateTexture(win.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, 16*tsize_x, 16*tsize_y);
    //SDL_UpdateTexture(texture, NULL, gtextr.data, 64);
    for(int y = 0; y < tsize_y; y++){
        for(int x = 0; x < tsize_x; x++){
            SDL_Rect current_working_area;
            current_working_area.h = 16;
            current_working_area.w = 16;
            current_working_area.x = 16*x;
            current_working_area.y = 16*y;
            SDL_UpdateTexture(texture, &current_working_area, tilemap.at(y*tsize_y+x).data, 64);
        }
    }
    render_rectangle.x = 0; 
    render_rectangle.y = 0; 
    render_rectangle.w = (win.get_width()/320)*5*tsize_x; 
    render_rectangle.h = (win.get_height()/240)*5*tsize_y;
    SDL_QueryTexture(texture, NULL, NULL, &size_x, &size_y);
}
graphics::metatile::~metatile(){
    SDL_DestroyTexture(texture);
}