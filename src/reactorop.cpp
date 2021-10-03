#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "graphics.hpp"
#include "game.hpp"
#include <vector>
#include <fstream>
#include <iostream>

#define SEED 6262	// Was used for testing stuff

// TODO Load from config
int actual_width = 800;
int actual_height = 600;

const graphics::gtexture floor_texture = graphics::load_gtexture_from_file("data/floor.dat");
const graphics::gtexture border_texture = graphics::load_gtexture_from_file("data/border.dat");
const graphics::gtexture cursor_texture = graphics::load_gtexture_from_file("data/mouse_cursor.dat");
const graphics::gtexture robot_texture = graphics::load_gtexture_from_file("data/robot.dat");
const graphics::gtexture empty_texture = graphics::load_gtexture_from_file("data/check.dat");

const graphics::gtexture movex_texture = graphics::load_gtexture_from_file("data/button_movex.dat");
const graphics::gtexture movey_texture = graphics::load_gtexture_from_file("data/button_movey.dat");
const graphics::gtexture load_texture = graphics::load_gtexture_from_file("data/button_load.dat");
const graphics::gtexture unloadf_texture = graphics::load_gtexture_from_file("data/button_unloadf.dat");
const graphics::gtexture unloadc_texture = graphics::load_gtexture_from_file("data/button_unloadc.dat");
const graphics::gtexture wait_texture = graphics::load_gtexture_from_file("data/button_wait.dat");
const graphics::gtexture x_texture = graphics::load_gtexture_from_file("data/button_x.dat");
const graphics::gtexture y_texture = graphics::load_gtexture_from_file("data/button_y.dat");
const graphics::gtexture exit_texture = graphics::load_gtexture_from_file("data/exit.dat");
const graphics::gtexture enter_texture = graphics::load_gtexture_from_file("data/enter.dat");
const graphics::gtexture backspace_texture = graphics::load_gtexture_from_file("data/backspace.dat");

const graphics::gtexture one_texture = graphics::load_gtexture_from_file("data/1.dat");
const graphics::gtexture two_texture = graphics::load_gtexture_from_file("data/2.dat");
const graphics::gtexture three_texture = graphics::load_gtexture_from_file("data/3.dat");
const graphics::gtexture four_texture = graphics::load_gtexture_from_file("data/4.dat");
const graphics::gtexture five_texture = graphics::load_gtexture_from_file("data/5.dat");
const graphics::gtexture six_texture = graphics::load_gtexture_from_file("data/6.dat");
const graphics::gtexture seven_texture = graphics::load_gtexture_from_file("data/7.dat");
const graphics::gtexture eight_texture = graphics::load_gtexture_from_file("data/8.dat");
const graphics::gtexture nine_texture = graphics::load_gtexture_from_file("data/9.dat");
const graphics::gtexture zero_texture = graphics::load_gtexture_from_file("data/0.dat");

std::vector<graphics::gtexture> reactor_tile_texture;
std::vector<graphics::gtexture> coolant_tile_texture;
std::vector<graphics::gtexture> fuel_tile_texture;

std::vector<reop::robot> robots;
int seed = 0;
unsigned int font[98][256];
//unsigned int temp_memory[128] = {0};
//unsigned int fuel_memory[128] = {0};
//unsigned int coolant_memory[128] = {0};
bool pause = false;
int mouse_x, mouse_y;
int tcursor_x, tcursor_y = 0;
Uint32 mouse_button_mask = 0;
bool collision_map[24][16] = {false};
bool show_robot_program = false;
int m_click_x, m_click_y;
reop::reactor reactor;
int reactorx, reactory;
int coolantx, coolanty;
int fuelx, fuely;
int current_robot_in_focus = -1;
int selected_op = -1;
bool is_keypad_on = false;
bool is_playing = true; 
int unloaded = 0;

bool is_near_reactor(int x, int y){
	if(	y >= reactory-1 ||
		y <= reactory+4
	){
		if(	x >= reactorx-1 ||
			x <= reactorx+4	
		){
			return true;
		}
	}
	return false;
}
bool is_near_coolant(int x, int y){
	if(	x >= coolantx-1 &&
		x <= coolantx+2
	){
		if(	y >= coolanty-1 &&
			y <= coolanty+1
		){
			return true;
		}
	}
	return false;
}
bool is_near_fuel(int x, int y){
	if(	x >= fuelx-1 &&
		x <= fuelx+2
	){
		if(	y >= fuely-1 &&
			y <= fuely+1
		){
			return true;
		}
	}
	return false;
}
int random_range(int min, int max){
	return rand() % (max + 1 - min) + min;
}
void generate_map(){
	srand(time(NULL));
	seed = random_range(0, 10000);
	//srand(SEED);
	srand(seed);
	std::cout << "Seed is: " << seed << "\n";
	int wall_count = random_range(2, 5);
	for(int y = 0; y < 16; y++){
		for(int x = 0; x < 24; x++){
			if(x == 0 || x == 23 || y == 0 || y == 15){
				collision_map[x][y] = true;
			} else {
				collision_map[x][y] = false;
			}
		}
	}
	/*for(int i = 0; i < wall_count; i++){
		int start = 1;
		int end = 0;
		int dir = random_range(0, 1);
		if(dir == 0){
			while(start > end){
				start = random_range(1, 15);
				end = random_range(1, 15);
			}
			int x = random_range(0, 23);
			int hole = random_range(0, 15);
			for(int j = start; j < end; j++){
				if(j != hole){
					collision_map[x][j] = true;
				}
			}
		} else {
			while(start > end){
				start = random_range(1, 23);
				end = random_range(1, 23);
			}
			int y = random_range(0, 15);
			int hole = random_range(0, 23);
			for(int j = start; j < end; j++){
				if(j != hole){
					collision_map[y][j] = true;
				}
			}
		}
	}*/
	// reactor placement
	while(1){
		reactorx = random_range(1, 20);
		reactory = random_range(1, 12);
		if(	collision_map[reactorx][reactory] == false &&
			collision_map[reactorx+1][reactory] == false &&
			collision_map[reactorx+2][reactory] == false &&
			collision_map[reactorx][reactory+1] == false &&
			collision_map[reactorx+1][reactory+1] == false &&
			collision_map[reactorx+2][reactory+1] == false &&
			collision_map[reactorx][reactory+2] == false &&
			collision_map[reactorx+1][reactory+2] == false &&
			collision_map[reactorx+2][reactory+2] == false
		){
			break;
		}
	}
	collision_map[reactorx][reactory] == true;
	collision_map[reactorx+1][reactory] == true;
	collision_map[reactorx+2][reactory] == true;
	collision_map[reactorx][reactory+1] == true;
	collision_map[reactorx+1][reactory+1] == true;
	collision_map[reactorx+2][reactory+1] == false;
	collision_map[reactorx][reactory+2] == true;
	collision_map[reactorx+1][reactory+2] == true;
	collision_map[reactorx+2][reactory+2] == true;
	// coolant placement
	while(1){
		coolantx = random_range(1, 20);
		coolanty = random_range(1, 12);
		if(	collision_map[coolantx][coolanty] == false &&
			collision_map[coolantx+1][coolanty] == false
		){
			break;
		}
	}
	collision_map[coolantx][coolanty] == true;
	collision_map[coolantx+1][coolanty] == true;
	while(1){
		fuelx = random_range(1, 20);
		fuely = random_range(1, 12);
		if(	collision_map[fuelx][fuely] == false &&
			collision_map[fuelx+1][fuely] == false
		){
			break;
		}
	}
	collision_map[fuelx][fuely] == false;
	collision_map[fuelx+1][fuely] == false;
	std::cout<<fuelx<<"\n";
	std::cout<<fuely<<"\n";
}
Uint32 do_tick(Uint32 interval, void *param){
	if(pause){
		return interval;
	}
	for(int i = 0; i < robots.size(); i++){
		// check areas
		bool near_coolant = is_near_coolant(robots.at(i).x, robots.at(i).y);
		bool near_fuel = is_near_fuel(robots.at(i).x, robots.at(i).y);
		bool near_reactor = is_near_reactor(robots.at(i).x, robots.at(i).y);
		if(robots.at(i).pc >= robots.at(i).program.size()){
			robots.at(i).pc = 0;
		} else {
			// get operation
			std::string s = robots.at(i).program.at(robots.at(i).pc);
			// get instruction
			std::string instruction = s.substr(0, s.find("@"));
			// get argument
			int argument = stoi(s.substr(s.find("@")+1, s.size()));
			if(instruction == "MX"){
				if(argument < robots.at(i).x){
					if(collision_map[robots.at(i).x-1][robots.at(i).y] == false){
						robots.at(i).x--;
					}
				}
				if(argument > robots.at(i).x){
					if(collision_map[robots.at(i).x+1][robots.at(i).y] == false){
						robots.at(i).x++;
					}
				}
				if(argument == robots.at(i).x){
					robots.at(i).pc++; // Operation done. Load next instruction
				}
			} else if(instruction == "MY"){
				if(argument < robots.at(i).y){
					if(collision_map[robots.at(i).x][robots.at(i).y-1] == false){
						robots.at(i).y--;
					}
				}
				if(argument > robots.at(i).y){
					if(collision_map[robots.at(i).x][robots.at(i).y+1] == false){
						robots.at(i).y++;
					}
				}
				if(argument == robots.at(i).y){
					robots.at(i).pc++; // Operation done. Load next instruction
				}
			} else if(instruction == "LO"){
				if(near_coolant && robots.at(i).coolant < argument){
					if(robots.at(i).coolant+1 < 20){
						robots.at(i).coolant++;
					}
					if(robots.at(i).coolant == argument){
						robots.at(i).pc++;
					}
				}
				if(near_fuel && robots.at(i).fuel < argument){
					if(robots.at(i).fuel+1 < 20){
						robots.at(i).fuel++;
					}
					if(robots.at(i).fuel == argument){
						robots.at(i).pc++;
					}
				}
			} else if(instruction == "UC"){
				if(robots.at(i).coolant >= argument && is_near_reactor){
					robots.at(i).coolant -= argument;
					reactor.coolant += argument;
					robots.at(i).pc++;
				} else {
					reactor.coolant += robots.at(i).coolant;
					robots.at(i).coolant = 0;
					robots.at(i).pc++;
				}
			} else if(instruction == "UF"){
				if(robots.at(i).fuel >= argument && is_near_reactor){
					robots.at(i).fuel -= argument;
					reactor.fuel += argument;
					robots.at(i).pc++;
				} else {
					reactor.fuel += robots.at(i).fuel;
					robots.at(i).fuel = 0;
					robots.at(i).pc++;
				}
				robots.at(i).pc++;
			} else if(instruction == "WA"){
				// wait...
				robots.at(i).pc++;
			}
		}
	}
	// reactor
	if(reactor.isRunning == true){
		if(reactor.temp > 3000 || reactor.fuel == 0 && reactor.coolant == 0){
			is_playing = false;
		}
		if(reactor.coolant <= 0){
			reactor.temp += 200;
		} else {
			reactor.fuel--;
			reactor.coolant--;
		}
	}
	return interval;
}
void load_font(){
	std::ifstream fstr("data/font.dat", std::ios::binary);
	for(int chr = 0; chr < 98; chr++){
		for(int i = 0; i < 256; i++){
			int value;
			fstr.read(reinterpret_cast<char*>(&value), sizeof(int));
			font[chr][i] = value;
    	}
	}
}
void place_text(graphics::gamewindow& win, int x, int y, std::string str){
	graphics::tile symbol(win, empty_texture);
	for(int i = 0; i < str.size(); i++){
		graphics::gtexture gtextr;
		for(int j = 0 ; j < 256; j++){
			gtextr.data[j] = font[str.at(i)-32][j];
		}
		//std::cout << str.at(i) << +str.at(i) << "\n";
		graphics::texture_from_gtexture(symbol.texture, gtextr);
		symbol.place((x+i)*10, y*10);
		SDL_RenderCopy(win.renderer, symbol.texture, NULL, &symbol.render_rectangle);
	}
}
void load_metatile_texture(std::vector<graphics::gtexture>& texture, std::string name, int tsize_x, int tsize_y){
	for(int i = 0; i < tsize_x*tsize_y; i++){
		texture.push_back(graphics::load_gtexture_from_file("data/"+name+std::to_string(i)+".dat"));
	}
}
void place_mouse_cursor(graphics::gamewindow& win){
	tcursor_x = mouse_x/(actual_width/32);;
	tcursor_y = mouse_y/(actual_height/24);;
}
void robot_program_editor(graphics::gamewindow& win, reop::robot robot){
	if(show_robot_program){
		pause = true;
		for(int i = 0; i < robot.program.size(); i++){
			if(i+1 == 15){
				break;
			}
			place_text(win, 24, 1+i, std::to_string(i)+":" + robot.program.at(i));
		}
	}
}
void focus_cursor_at_robot(graphics::gamewindow& win, std::vector<reop::robot>& robots){
	if(current_robot_in_focus == -1){
		return;
	}
	graphics::tile cursor(win, graphics::mix_gtextures(cursor_texture, robot_texture));
	cursor.place(robots.at(current_robot_in_focus).x*10, robots.at(current_robot_in_focus).y*10);
	cursor.render(win);
}
graphics::gtexture modify_button_texture(graphics::gtexture button, int id){
	if(selected_op == id){
		return graphics::mix_gtextures(cursor_texture, button);
	} else {
		return button;
	}
}

int main(void){
	generate_map();
	graphics::gamewindow window("Reactor Operator", actual_width, actual_height);
	graphics::tile current(window, border_texture);
	load_metatile_texture(reactor_tile_texture, "reactor", 3, 3);
	load_metatile_texture(coolant_tile_texture, "coolant", 2, 1);
	load_metatile_texture(fuel_tile_texture, "fuel", 2, 1);
	graphics::metatile reactor_tile(window, reactor_tile_texture, 3, 3);
	graphics::metatile coolant_tile(window, coolant_tile_texture, 2, 1);
	graphics::metatile fuel_tile(window, fuel_tile_texture, 2, 1);
	SDL_SetRenderDrawColor(window.renderer, 40, 40, 40, 255);
	load_font();
	SDL_TimerID my_timer_id = SDL_AddTimer(100, do_tick, (void*)NULL);
	robots.push_back(reop::robot());
	// main loop
	std::string next_op;
	while (1) {
		// clear the screen
		SDL_RenderClear(window.renderer);
		// event handling
		SDL_Event e;
		if(is_playing == false){
			place_text(window, 0, 23, "Reactor stopped. Game over.");
			SDL_RemoveTimer(my_timer_id);
		}
		if (SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT){
				break;
			} else if(e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE){
				break;
			} else if(e.type == SDL_KEYDOWN) {
				if(e.key.keysym.sym == SDLK_RIGHT){
					if(!show_robot_program){
						if(current_robot_in_focus+1 >= robots.size()){
							current_robot_in_focus = 0;
						} else {
							current_robot_in_focus++;
						}
					} else {
						if(selected_op == 6 && is_keypad_on == false){
							selected_op = 0;
						} else if(selected_op == 18 && is_keypad_on == true){
							selected_op = 0;
						} else {
							selected_op++;
						}
					}
				}
				if(e.key.keysym.sym == SDLK_LEFT){
					if(!show_robot_program){
						if(current_robot_in_focus == 0){
							current_robot_in_focus = robots.size()-1;
						} else {
							current_robot_in_focus--;
						}
					} else {
						if(selected_op == 0 && is_keypad_on == false){
							selected_op = 4;
						} else if(selected_op == 0 && is_keypad_on == true){
							selected_op = 18;
						} else {
							selected_op--;
						}
					}
				}
				if(e.key.keysym.sym == SDLK_DOWN){
					if(show_robot_program && is_keypad_on){
						if(selected_op > -1 && selected_op < 7){
							selected_op = 6;
						} else if(selected_op >= 7){
							selected_op += 3;
						}
					}
				}
				if(e.key.keysym.sym == SDLK_UP){
					if(show_robot_program && is_keypad_on){
						if(selected_op <= 15){
							selected_op -= 3;
						}
					}
				}
				if(e.key.keysym.sym == SDLK_BACKSPACE && robots.at(current_robot_in_focus).program.size() != 0){
					robots.at(current_robot_in_focus).program.pop_back();
				}
				if(e.key.keysym.sym == SDLK_RETURN){
					robots.at(current_robot_in_focus).program.push_back(next_op);
				}
				if(e.key.keysym.sym == SDLK_SPACE){
					if(!show_robot_program){
						pause = !pause;
					}
					if(show_robot_program){
						if(selected_op == 0){
							next_op = "MX@";
							is_keypad_on = true;
						}
						if(selected_op == 1){
							next_op = "MY@";
							is_keypad_on = true;
						}
						if(selected_op == 2){
							next_op = "LO@";
							is_keypad_on = true;
						}
						if(selected_op == 3){
							next_op = "UC@";
							is_keypad_on = true;
						}
						if(selected_op == 4){
							next_op = "UF@";
							is_keypad_on = true;
						}
						if(selected_op == 5){
							next_op = "WA@";
							is_keypad_on = true;
						}
						if(selected_op == 6){
							show_robot_program = false;
							pause = false;
						}
						if(selected_op >= 6 && selected_op <= 15){
							next_op += std::to_string(selected_op - 6);
						}
						if(selected_op == 16){
							next_op.pop_back();
						}
						if(selected_op == 17){
							next_op += "0";
						}
						if(selected_op == 18){
							robots.at(current_robot_in_focus).program.push_back(next_op);
							robots.at(current_robot_in_focus).pc = 0;
						}
					} else if(current_robot_in_focus != -1){
						show_robot_program = !show_robot_program;
					}
				}
				if(e.key.keysym.sym == SDLK_s){
					reactor.isRunning = true;
				}
				if(e.key.keysym.sym == SDLK_a){
					robots.push_back(reop::robot());
					robots.at(robots.size()-1).x = robots.at(robots.size()-2).x+1;
				}
				if(e.key.keysym.sym == SDLK_x){
					next_op = "MX@";
				}
				if(e.key.keysym.sym == SDLK_y){
					next_op = "MY@";
				}
				if(e.key.keysym.sym == SDLK_l){
					next_op = "LO@";
				}
				if(e.key.keysym.sym == SDLK_c){
					next_op = "UC@";
				}
				if(e.key.keysym.sym == SDLK_f){
					next_op = "UF@";
				}
				if(e.key.keysym.sym == SDLK_w){
					next_op = "WA@";
				}
				if(e.key.keysym.sym >= 48 && e.key.keysym.sym <= 57){
					next_op += e.key.keysym.sym;
				}
				
			}
			/* else if(e.type = SDL_MOUSEBUTTONDOWN){
				if(e.button.button == SDL_BUTTON_RIGHT){
					show_robot_program = true;
				} else if(e.button.button == SDL_BUTTON_LEFT){
					m_click_x = tcursor_x;
					m_click_y = tcursor_y;
					if(show_robot_program){
						if(m_click_x == 24 && m_click_y == 15){
							robots.at(0).program.push_back("MX@7");
						}
					}
				}
			}*/
		}
		//place_mouse_cursor(window);
		if(current_robot_in_focus >= 0){
			robot_program_editor(window, robots.at(current_robot_in_focus));
		}
		for(int y = 0 ; y < 16; y++){
			for(int x = 0; x < 24; x++){
				current.place(x*10, y*10);
				if(collision_map[x][y]){
					graphics::texture_from_gtexture(current.texture, border_texture);
				} else {
					if(x == tcursor_x && y == tcursor_y){
						graphics::texture_from_gtexture(current.texture, graphics::mix_gtextures(cursor_texture, floor_texture));
					} else {
						graphics::texture_from_gtexture(current.texture, floor_texture);
					}
				}
				current.render(window);
			}
		}
		for(int i = 0; i < robots.size(); i++){
			if(robots.at(i).x == tcursor_x && robots.at(i).y == tcursor_y){
				graphics::texture_from_gtexture(current.texture, graphics::mix_gtextures(cursor_texture, robot_texture));
			} else {
				graphics::texture_from_gtexture(current.texture, robot_texture);
			}
			current.place(robots.at(i).x*10, robots.at(i).y*10);
			current.render(window);
		}
		place_text(window, 0, 17, "T(C): " + std::to_string(reactor.temp));
		place_text(window, 0, 18, "F(g): " + std::to_string(reactor.fuel));
		place_text(window, 0, 19, "C(k): " + std::to_string(reactor.coolant));
		graphics::texture_from_gtexture(current.texture, robot_texture);
		reactor_tile.place(reactorx*10, reactory*10);
		coolant_tile.place(coolantx*10, coolanty*10);
		fuel_tile.place(fuelx*10, fuely*10);
		reactor_tile.render(window);
		coolant_tile.render(window);
		fuel_tile.render(window);
		// Rendering buttons
		/*graphics::texture_from_gtexture(current.texture, x_texture);
		current.place(25*10, 15*10);
		current.render(window);
		graphics::texture_from_gtexture(current.texture, y_texture);
		current.place(26*10, 15*10);
		current.render(window);*/
		if(show_robot_program){
			place_text(window, 24, 14, next_op);
			graphics::texture_from_gtexture(current.texture, modify_button_texture(movex_texture, 0));
			current.place(24*10, 15*10);
			current.render(window);

			place_text(window, 24, 14, next_op);
			graphics::texture_from_gtexture(current.texture, modify_button_texture(movey_texture, 1));
			current.place(25*10, 15*10);
			current.render(window);
			
			graphics::texture_from_gtexture(current.texture, modify_button_texture(load_texture, 2));
			current.place(26*10, 15*10);
			current.render(window);
			
			graphics::texture_from_gtexture(current.texture, modify_button_texture(unloadc_texture, 3));
			current.place(27*10, 15*10);
			current.render(window);

			graphics::texture_from_gtexture(current.texture, modify_button_texture(unloadf_texture, 4));
			current.place(28*10, 15*10);
			current.render(window);
			
			graphics::texture_from_gtexture(current.texture, modify_button_texture(wait_texture, 5));
			current.place(29*10, 15*10);
			current.render(window);
			
			graphics::texture_from_gtexture(current.texture, modify_button_texture(exit_texture, 6));
			current.place(31*10, 15*10);
			current.render(window);
			if(is_keypad_on){
				graphics::texture_from_gtexture(current.texture, modify_button_texture(one_texture, 7));
				current.place(24*10, 16*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(two_texture, 8));
				current.place(25*10, 16*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(three_texture, 9));
				current.place(26*10, 16*10);
				current.render(window);

				graphics::texture_from_gtexture(current.texture, modify_button_texture(four_texture, 10));
				current.place(24*10, 17*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(five_texture, 11));
				current.place(25*10, 17*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(six_texture, 12));
				current.place(26*10, 17*10);
				current.render(window);

				graphics::texture_from_gtexture(current.texture, modify_button_texture(seven_texture, 13));
				current.place(24*10, 18*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(eight_texture, 14));
				current.place(25*10, 18*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(nine_texture, 15));
				current.place(26*10, 18*10);
				current.render(window);

				graphics::texture_from_gtexture(current.texture, modify_button_texture(backspace_texture, 16));
				current.place(24*10, 19*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(zero_texture, 17));
				current.place(25*10, 19*10);
				current.render(window);
				graphics::texture_from_gtexture(current.texture, modify_button_texture(enter_texture, 18));
				current.place(26*10, 19*10);
				current.render(window);
			}
		}
		if(!show_robot_program && current_robot_in_focus >= 0){
			place_text(window, 24, 0, "PC:"+std::to_string(robots.at(current_robot_in_focus).pc));
			place_text(window, 24, 1, "X:"+std::to_string(robots.at(current_robot_in_focus).x));
			place_text(window, 24, 2, "Y:"+std::to_string(robots.at(current_robot_in_focus).y));
			place_text(window, 24, 3, "C:"+std::to_string(robots.at(current_robot_in_focus).coolant));
			place_text(window, 24, 4, "F:"+std::to_string(robots.at(current_robot_in_focus).fuel));
			//place_text(window, 24, 5, "X:"+robots.at(current_robot_in_focus).program.at(robots.at(current_robot_in_focus).pc));
		}
		focus_cursor_at_robot(window, robots);
        if(reactor.isRunning == false){
			place_text(window, 0, 23, "Press 's' to start reactor");
		}
		// Do magic to display it on the screen
		SDL_RenderPresent(window.renderer);
	}
}