#pragma warning(disable:4996)
#include <iostream>
#include <assert.h>
using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"
#include "SDL2-2.0.8\include\SDL_mixer.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_mixer.lib")

#pragma comment(linker,"/subsystem:console")

SDL_Renderer *renderer = NULL;
int screen_width = 800;
int screen_height = 600;
SDL_Window *window = NULL;

unsigned char prev_key_state[256];
unsigned char *keys = NULL;
const Uint8 *state = SDL_GetKeyboardState(NULL);

struct Player
{
	int w, h;
	float x, y, vel_x, vel_y, accel_x, accel_y;
};

struct Obstacle
{
	int x, y, w, h;
};

struct Camera
{
	float x, y;
};

struct Particle_Vec2D
{
	float x, y;
};

struct Particle_Size
{
	int w, h;
};

struct Particle
{
	SDL_Texture *tex;
	Particle_Vec2D *pos;
	Particle_Size *size;
	int particle_arr;
	int anim_frames;
	unsigned char *active;
	unsigned char *current_frame;
	unsigned int *lifetime;
	unsigned int *creation_time;
	unsigned int *last_frame_update;
	unsigned int anim_speed;
};

void init_P1(Player *p)
{
	//player size, position, and velocity
	p->w = 32;
	p->h = 32;
	p->x = 10.f;
	p->y = 500.f;
	p->vel_x = 0;
	p->vel_y = 0;
	p->accel_x = 0;
	p->accel_y = 0;
}

void camera_Setup(Player *p, Camera *c)
{
	c->x = p->x;
	c->y = 300.f;
}

void particle_Init(SDL_Texture *t, Particle *d, int arr_size, int n_frames, int speed)
{
	d->tex = t;
	d->anim_frames = n_frames;
	d->anim_speed = speed;
	d->particle_arr = arr_size;
	d->active = (unsigned char*)malloc(sizeof(unsigned char)*arr_size);
	d->current_frame = (unsigned char*)malloc(sizeof(unsigned char)*arr_size);
	d->lifetime = (unsigned int*)malloc(sizeof(unsigned int)*arr_size);
	d->creation_time = (unsigned int*)malloc(sizeof(unsigned int)*arr_size);
	d->last_frame_update = (unsigned int*)malloc(sizeof(unsigned int)*arr_size);
	d->pos = (Particle_Vec2D*)malloc(sizeof(Particle_Vec2D)*arr_size);
	d->size = (Particle_Size*)malloc(sizeof(Particle_Size)*arr_size);

	for (int i = 0; i < arr_size; i++) d->active[i] = 0;
}

void create_Particles(Player *p, Particle *d, unsigned int curr_time, int num)
{
	for (int i = 0; i < d->particle_arr; i++)
	{
		if (d->active[i] == 0)
		{
			d->active[i] = 1;
			d->pos[i] = { p->x, p->y };
			d->size[i] = { p->w, p->h };
			d->creation_time[i] = curr_time;
			d->lifetime[i] = 2000 + rand() % 5000;

			num++;
			if (num >= 200) num / 2;
		}
	}
}

void update_Particles(Particle *d, unsigned int curr_time)
{
	for (int i = 0; i < d->particle_arr; i++)
	{
		if (d->active[i] == 1)
		{
			if (curr_time - d->creation_time[i] > d->lifetime[i])
			{
				d->active[i] = 0;
				continue;
			}

			if (curr_time - d->last_frame_update[i] > d->anim_speed)
			{
				d->last_frame_update[i] = curr_time;
				d->current_frame[i]++;
				d->current_frame[i] %= d->anim_frames;
			}
		}
	}
}

void draw_P1(SDL_Renderer *renderer, Player *p)
{
	//set player color to blue
	SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
	SDL_Rect rect;

	rect.w = p->w;
	rect.h = p->h;
	rect.x = p->x;
	rect.y = p->y;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Health_Bar(SDL_Renderer *renderer, int hp)
{
	//set health color to white
	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
	SDL_Rect rect;

	rect.w = hp;
	rect.h = 10;
	rect.x = 150;
	rect.y = 10;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Fuel_Bar(SDL_Renderer *renderer, float gas)
{
	//set fuel color to orange
	SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
	SDL_Rect rect;

	rect.w = gas;
	rect.h = 10;
	rect.x = 575;
	rect.y = 10;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Lives(SDL_Renderer *renderer, int x, int y)
{
	//set lives color to white
	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
	SDL_Rect rect;

	rect.w = 10;
	rect.h = 10;
	rect.x = x;
	rect.y = y;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Cam(SDL_Renderer *renderer, Camera *c)
{
	//set camera color to green
	SDL_SetRenderDrawColor(renderer, 0, 255, 100, 50);
	SDL_Rect rect;

	rect.w = 50;
	rect.h = 50;
	rect.x = c->x;
	rect.y = c->y;

	//SDL_RenderFillRect(renderer, &rect);
}

void draw_Bar(SDL_Renderer *renderer, Obstacle *o, Camera *c, int x, int y, int w, int h)
{
	//bar position and size
	o->x = x - c->x;
	o->y = y;
	o->w = w;
	o->h = h;

	//set bar color to yellow
	SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
	SDL_Rect rect;

	rect.w = o->w;
	rect.h = o->h;
	rect.x = o->x;
	rect.y = o->y;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Fuel_Block(SDL_Renderer *renderer, Obstacle *o, Camera *c, int x, int y, int w, int h)
{
	//fuel position and size
	o->x = x - c->x;
	o->y = y;
	o->w = w;
	o->h = h;

	//set fuel color to orange
	SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
	SDL_Rect rect;

	rect.w = o->w;
	rect.h = o->h;
	rect.x = o->x;
	rect.y = o->y;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Dust(SDL_Renderer *renderer, Player *p)
{
	//set particle color to dirt
	SDL_SetRenderDrawColor(renderer, 125, 100, 0, 255);
	SDL_Rect rect;

	rect.w = 5;
	rect.h = 5;
	rect.x = p->x - rand() % 15;
	rect.y = (p->y + 30) - rand() % 7;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Player_Img(SDL_Renderer *renderer, SDL_Texture *t, Player *p, int src_x, int src_y, int src_w, int src_h, float angle)
{
	//image setup stuff
	SDL_Rect src;
	//define rectangle to be copied from the texture (source)
	src.x = src_x;
	src.y = src_y;
	src.w = src_w;
	src.h = src_h;
	SDL_Rect dest;
	//define rectangle to be copied to the screen (destination)
	dest.x = p->x;
	dest.y = p->y;
	dest.w = p->w;
	dest.h = p->h;

	//draw image
	//copy from source texture to destination screen.
	//SDL_FLIP_XXX enumeration allows you to mirror the image
	SDL_RenderCopyEx(renderer, t, &src, &dest, angle, NULL, SDL_FLIP_NONE);
}

void draw_Obs_Img(SDL_Renderer *renderer, SDL_Texture *t, Obstacle *o, Camera *c, int src_x, int src_y, int src_w, int src_h, 
	int dest_x, int dest_y, int dest_w, int dest_h)
{
	//image setup stuff
	SDL_Rect src;
	//define rectangle to be copied from the texture (source)
	src.x = src_x;
	src.y = src_y;
	src.w = src_w;
	src.h = src_h;
	SDL_Rect dest;
	//define rectangle to be copied to the screen (destination)
	dest.x = dest_x - c->x;
	dest.y = dest_y;
	dest.w = dest_w;
	dest.h = dest_h;
	//get coordinates and size from destination for sprite collision
	o->x = dest.x;
	o->y = dest.y;
	o->w = dest.w;
	o->h = dest.h;

	//draw image
	//copy from source texture to destination screen.
	//SDL_FLIP_XXX enumeration allows you to mirror the image
	SDL_RenderCopyEx(renderer, t, &src, &dest, 0, NULL, SDL_FLIP_NONE);
}

void draw_Particle_Img(SDL_Renderer *renderer, Particle *d, int src_x, int src_y, int src_w, int src_h, int num)
{
	//image setup stuff
	SDL_Rect src;
	//source
	src.x = src_x;
	src.y = src_y;
	src.w = src_w;
	src.h = src_h;
	SDL_Rect dest;
	//destination
	for (int i = 0; i < num; i++)
	{
		dest.x = d->pos[i].x;
		dest.y = (d->pos[i].y - 16) + rand() % 10;
		dest.w = d->size[i].w - rand() % 16;
		dest.h = d->size[i].h - rand() % 16;
	}

	//draw image
	//copy from source texture to destination screen.
	//SDL_FLIP_XXX enumeration allows you to mirror the image
	SDL_RenderCopyEx(renderer, d->tex, &src, &dest, 0, NULL, SDL_FLIP_NONE);
}

void draw_Finish_Area(SDL_Renderer *renderer, Obstacle *o, Camera *c)
{
	//set position and size
	o->x = 1500 - c->x;
	o->y = 0;
	o->w = 100;
	o->h = 600;

	//set finish area color to teal
	SDL_SetRenderDrawColor(renderer, 0, 200, 200, 255);
	SDL_Rect rect;

	rect.w = o->w;
	rect.h = o->h;
	rect.x = o->x;
	rect.y = o->y;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_FA_Img(SDL_Renderer *renderer, SDL_Texture *t, int src_x, int src_y, int src_w, int src_h,
	int dest_x, int dest_y, int dest_w, int dest_h)
{
	//image setup stuff
	SDL_Rect src;
	//define rectangle to be copied from the texture (source)
	src.x = src_x;
	src.y = src_y;
	src.w = src_w;
	src.h = src_h;
	SDL_Rect dest;
	//define rectangle to be copied to the screen (destination)
	dest.x = dest_x;
	dest.y = dest_y;
	dest.w = dest_w;
	dest.h = dest_h;

	//draw image
	//copy from source texture to destination screen.
	//SDL_FLIP_XXX enumeration allows you to mirror the image
	SDL_RenderCopyEx(renderer, t, &src, &dest, 0, NULL, SDL_FLIP_NONE);
}

void draw_Area_Of_Gravity(SDL_Renderer *renderer, Obstacle *o, Obstacle *e, int num)
{
	for (int i = 0; i < num; i++)
	{
		//aog position and size
		e[i].x = o[i].x + (o[i].w + 30);
		e[i].y = o[i].y - 65;
		e[i].w = o[i].w;
		e[i].h = o[i].h * 2;

		//set aog color to red
		SDL_SetRenderDrawColor(renderer, 200, 0, 0, 50);
		SDL_Rect rect;

		rect.x = e[i].x;
		rect.y = e[i].y;
		rect.w = e[i].w;
		rect.h = e[i].h;

		//SDL_RenderFillRect(renderer, &rect);
	}
	
}

void draw_Text(SDL_Renderer *renderer, SDL_Texture *t, char *tex, int font_size, int dest_x, int dest_y)
{
	for (int i = 0; i < 16; i++)
	{
		//source
		SDL_Rect src;
		src.x = 64 * (tex[i] % 16);//column
		src.y = 64 * (tex[i] / 16);//row
		src.w = 64;
		src.h = 64;
		//destination
		SDL_Rect dest;
		dest.x = dest_x;
		dest.y = dest_y;
		dest.w = font_size;
		dest.h = dest.w;

		//draw image
	    //copy from source texture to destination screen.
	    //SDL_FLIP_XXX enumeration allows you to mirror the image
		SDL_RenderCopyEx(renderer, t, &src, &dest, 0, NULL, SDL_FLIP_NONE);

		//increment dest_x!
		dest_x += dest.w;
	}
}

int obs_Collision(Player *p, Obstacle *o, int num)
{
	for (int i = 0; i < num; i++)
	{
		float w = 0.5f * (p->w + o[i].w);
		float h = 0.5f * (p->h + o[i].h);
		float dx = p->x - o[i].x + 0.5f*(p->w - o[i].w);
		float dy = p->y - o[i].y + 0.5f*(p->h - o[i].h);

		if (dx*dx <= w * w && dy*dy <= h * h)
		{
			float wy = w * dy;
			float hx = h * dx;

			if (wy > hx)
			{
				return (wy + hx > 0) ? 3 : 4;
			}
			else
			{
				return (wy + hx > 0) ? 2 : 1;
			}
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	//player sprite
	const char *player_image_filename = "mt_bikeman_riding.png";
	//ramp sprite
	const char *ramp_image_filename = "main_track_ramp.png";
	//fire sprite
	const char *fire_image_filename = "ARW_2D_Flame_Sprite_Sheet.png";
	//smoke sprite
	const char *smoke_image_filename = "Smoke_n_Fire_Expl.png";
	//finish line sprite
	const char *finish_image_filename = "Track_FL.png";
	//text
	const char *font_filename = "font_sheet.png";


	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	
	prev_key_state[256];
	keys = (unsigned char*)SDL_GetKeyboardState(NULL);

	window = SDL_CreateWindow(
		"Track",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_ACCELERATED);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) std::cout << "Error:" << Mix_GetError() << std::endl;

	//load music
	Mix_Music *bgm = Mix_LoadMUS("");

	//load sound effect
	Mix_Chunk *sfx = Mix_LoadWAV("");

	//load player image
	SDL_Surface *player_sprite_surface = IMG_Load(player_image_filename);
	assert(player_sprite_surface);

	//load ramp image
	SDL_Surface *ramp_sprite_surface = IMG_Load(ramp_image_filename);
	assert(ramp_sprite_surface);

	//load fire image
	SDL_Surface *fire_sprite_surface = IMG_Load(fire_image_filename);
	assert(fire_sprite_surface);

	//load smoke image
	SDL_Surface *smoke_sprite_surface = IMG_Load(smoke_image_filename);
	assert(smoke_sprite_surface);

	//load finish line image
	SDL_Surface *finish_sprite_surface = IMG_Load(finish_image_filename);
	assert(finish_sprite_surface);

	//load font sheet image
	SDL_Surface *font_surface = IMG_Load(font_filename);
	assert(font_surface);

	//create player texture from the surface
	SDL_Texture *player_sprite_texture = SDL_CreateTextureFromSurface(renderer, player_sprite_surface);

	//create ramp texture from the surface
	SDL_Texture *ramp_sprite_texture = SDL_CreateTextureFromSurface(renderer, ramp_sprite_surface);

	//create fire texture from the surface
	SDL_Texture *fire_sprite_texture = SDL_CreateTextureFromSurface(renderer, fire_sprite_surface);

	//create smoke texture from the surface
	SDL_Texture *smoke_sprite_texture = SDL_CreateTextureFromSurface(renderer, smoke_sprite_surface);

	//create finish line texture from the surface
	SDL_Texture *finish_sprite_texture = SDL_CreateTextureFromSurface(renderer, finish_sprite_surface);

	//create font texture from the surface
	SDL_Texture *font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);

	//free the player surface
	SDL_FreeSurface(player_sprite_surface);

	//free the ramp surface
	SDL_FreeSurface(ramp_sprite_surface);

	//free the fire surface
	SDL_FreeSurface(fire_sprite_surface);

	//free the smoke surface
	SDL_FreeSurface(smoke_sprite_surface);

	//free the finish line surface
	SDL_FreeSurface(finish_sprite_surface);

	//free the font sheet surface
	SDL_FreeSurface(font_surface);

	SDL_Surface *your_draw_buffer = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	SDL_SetSurfaceBlendMode(your_draw_buffer, SDL_BLENDMODE_NONE);

	//player stuff
	int p1_img_source_x = 0;
	int p1_img_source_w = 576;
	int sprite_sheet_w = p1_img_source_w * 2;
	int n_lives = 3;
	int health = 100;
	float gas = 100.f;
	float degrees = 0.f;
	Player player;
	init_P1(&player);
	Camera camera;
	camera_Setup(&player, &camera);

	//player states
	unsigned int last_frame_update = SDL_GetTicks();
	int previous_player_touch_ramp = 0;
	int player_touch_ramp = 0;
	int prev_in_air = 0;
	int in_air = 0;
	int air_time = 0;
	int on_ground = 1;
	int hurt = 0;
	int time_invun = 0;
	int death_timer = 0;
	int game_over_timer = 0;
	int at_finish = 0;
	int going = 0;

	//frame rate stuff
	const int fps = 60;
	const int frame_delay = 1000 / fps;
	Uint32 frame_start;
	int frame_time;

	//misc
	int delay = 0;
	int level = -1; //positive numbers = in a level, negative numbers = in a menu 
	int paused = 0;
	int game_over = 0;
	int clear_prompt = 0;
	int obs_collision = 0;

	//fire stuff
	int fire_img_source_x = 0;
	int fire_img_source_w = 24;
	int fire_sprite_sheet_w = fire_img_source_w * 7;

	//smoke stuff
	int smoke_img_source_x = 0;
	int smoke_img_source_w = 16;
	int smoke_sprite_sheet_w = fire_img_source_w * 4;

	//text stuff
	//g = game, t = title, m = menu
	char gtext[17];
	char gtext_1[17];
	char gtext_2[17];
	char gtext_3[17];
	char gtext_4[17];
	char gtext_5[17];
	char gtext_6[17];
	char gtext_7[17];
	char ttext[17];
	char ttext_1[17];
	char ttext_2[17];
	char ttext_3[17];
	char ttext_4[17];
	char mtext[17];
	char mtext_1[17];
	char mtext_2[17];
	char mtext_3[17];
	char mtext_4[17];
	char mtext_5[17];
	char mtext_6[17];
	int text_size = 20;
	int title_size = 100;
	int mtitle_size = 50;
	unsigned int last_text_update = SDL_GetTicks();
	sprintf(gtext, "Health");
	sprintf(gtext_1, "Lives");
	sprintf(gtext_2, "Gas");
	sprintf(gtext_3, "Press Space");
	sprintf(gtext_4, "GAME OVER");
	sprintf(gtext_5, "Paused");
	sprintf(gtext_6, "Resume [R]");
	sprintf(gtext_7, "Quit [Q]");
	sprintf(ttext, "Title");
	sprintf(ttext_1, "Main Menu");
	sprintf(ttext_2, "Rules");
	sprintf(ttext_3, "Controls");
	sprintf(ttext_4, "Credits");
	sprintf(mtext, "Press E");
	sprintf(mtext_1, "Start [W]");
	sprintf(mtext_2, "Rules [A]");
	sprintf(mtext_3, "Controls [S]");
	sprintf(mtext_4, "Credits [D]");
	sprintf(mtext_5, "Back [B]");
	sprintf(mtext_6, "Go Back [G]");

	//gas block stuff
	int n_gallons = 7;
	Obstacle *gallon = (Obstacle*)malloc(sizeof(Obstacle)*n_gallons);

	//barrier stuff
	int n_bars = 20;
	Obstacle *bar = (Obstacle*)malloc(sizeof(Obstacle)*n_bars);

	//ramp stuff
	int n_ramps = 12;
	Obstacle *ramp = (Obstacle*)malloc(sizeof(Obstacle)*n_ramps);
	int stunt = 0;

	//gravity stuff
	Obstacle *aog = (Obstacle*)malloc(sizeof(Obstacle)*n_ramps);

	//particle stuff
	Particle particles;
	int n_particles = 100;
	particle_Init(smoke_sprite_texture, &particles, n_particles, 4, 100);
	int particle_frame_counter = 0;
	unsigned int last_particle_frame_time = SDL_GetTicks();
	unsigned int last_particle_spawn = 0;

	//finish line stuff
	Obstacle finish_line;
	int finish_img_source_x = 0;
	int finish_img_source_w = 32;
	int finish_sprite_sheet_w = fire_img_source_w * 8;
	
	//set transparency of the smoke texture.
	SDL_SetTextureAlphaMod(smoke_sprite_texture, rand() % 255);

	for (;;)
	{
		memcpy(prev_key_state, keys, 256);

		frame_start = SDL_GetTicks();

		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		//sets screen
		{
			if (level >= 0)
			{
				//set color to dirt if playing a level
				SDL_SetRenderDrawColor(renderer, 100, 75, 0, 100);
			}
			else if (level < 0)
			{
				//set color to black if still in menus
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			}
			//clear screen
			SDL_RenderClear(renderer);
		}

		//menu stuff
		{
			//title screen
			if (level == -1)
			{
				draw_Text(renderer, font_texture, ttext, title_size, 150, 75);
				draw_Text(renderer, font_texture, mtext, text_size, 325, 400);
				if (state[SDL_SCANCODE_E]) level = -2;
			}

			//main menu screen
			if (level == -2)
			{
				draw_Text(renderer, font_texture, ttext_1, mtitle_size, 150, 75);
				draw_Text(renderer, font_texture, mtext_1, text_size, 300, 200);
				draw_Text(renderer, font_texture, mtext_2, text_size, 300, 250);
				draw_Text(renderer, font_texture, mtext_3, text_size, 300, 300);
				draw_Text(renderer, font_texture, mtext_4, text_size, 300, 350);
				draw_Text(renderer, font_texture, mtext_5, text_size, 300, 450);
				if (state[SDL_SCANCODE_W]) level = 0;
				if (state[SDL_SCANCODE_A]) level = -3;
				if (state[SDL_SCANCODE_S]) level = -4;
				if (state[SDL_SCANCODE_D]) level = -5;
				if (state[SDL_SCANCODE_B]) level = -1;
			}

			//rules screen
			if (level == -3)
			{
				draw_Text(renderer, font_texture, ttext_2, mtitle_size, 275, 75);
				draw_Text(renderer, font_texture, mtext_6, text_size, 50, 550);
				if (state[SDL_SCANCODE_G]) level = -2;
			}

			//controls screen
			if (level == -4)
			{
				draw_Text(renderer, font_texture, ttext_3, mtitle_size, 200, 75);
				draw_Text(renderer, font_texture, mtext_6, text_size, 50, 550);
				if (state[SDL_SCANCODE_G]) level = -2;
			}

			//credits screen
			if (level == -5)
			{
				draw_Text(renderer, font_texture, ttext_4, mtitle_size, 200, 75);
				draw_Text(renderer, font_texture, mtext_6, text_size, 50, 550);
				if (state[SDL_SCANCODE_G]) level = -2;
			}
		}

		//if out of lives
		{
			if (n_lives == 0 && level >= 0)
			{
				clear_prompt = 1;
				game_over = 1;
				health = 0;
				going = 0;
				gas = 1;
				draw_Player_Img(renderer, fire_sprite_texture, &player, fire_img_source_x, 0, fire_img_source_w, 24, degrees);
				//display game over text
				draw_Text(renderer, font_texture, gtext_4, text_size, 300, 300);
			}
			if (game_over == 1) game_over_timer++;
			//reset player and go back to main menu after a bit
			if (game_over_timer == 500)
			{
				level = -2;
				init_P1(&player);
				health = 100;
				gas = 100;
				n_lives = 3;
				game_over = 0;
				clear_prompt = 0;
				obs_collision = 0;
				game_over_timer = 0;
			}
		}

		//if out of gas
		{
			if (gas <= 0) gas = 0;
			if (gas == 0) health = 0;
		}

		unsigned int current_time = SDL_GetTicks();
		
		//player status
		previous_player_touch_ramp = player_touch_ramp;
		prev_in_air = in_air;

		//accel init
		player.accel_x = 0;
		if (n_lives != 0 && level >= 0)
		{
			if (state[SDL_SCANCODE_SPACE])
			{
				going = 1;
				clear_prompt = 1;
			}
		}
		if (going == 1 && paused != 1 && level >= 0)
		{
			player.accel_x = 0.1f;
			obs_collision = 1;
		}
		if (player.accel_x == 0.1f) gas -= 0.1f;
		player.accel_y = 0;

		//player movement
		{
			if (at_finish == 0 && going == 1 && paused != 1 && level >= 0)
			{
				if (player_touch_ramp == 0 && previous_player_touch_ramp == 0)
				{
					if (gas != 0)
					{
						//slowing down
						if (state[SDL_SCANCODE_A])
						{
							player.accel_x = 0.05f;
							gas -= 0.05f;
						}
						//speeding up
						if (state[SDL_SCANCODE_D])
						{
							player.accel_x = 0.2f;
							gas -= 0.2f;
						}
						//moving up
						if (state[SDL_SCANCODE_W])
						{
							player.accel_y = -0.1f;
							gas -= 0.1f;
						}
						//moving down
						if (state[SDL_SCANCODE_S])
						{
							player.accel_y = 0.1f;
							gas -= 0.1f;
						}

						if (health != 0) draw_Dust(renderer, &player);
					}
				}
			}
		}
		
		//sprite animation
		{
			//update
			{
				if (current_time - last_frame_update > 100 && paused != 1)
				{
					last_frame_update = current_time;

					//player
					p1_img_source_x += p1_img_source_w;
					//fire
					fire_img_source_x += fire_img_source_w;
					//finish line
					finish_img_source_x += finish_img_source_w;
				}
				if (p1_img_source_x >= sprite_sheet_w) p1_img_source_x = 0;
				if (fire_img_source_x >= fire_sprite_sheet_w) fire_img_source_x = 0;
				if (finish_img_source_x >= finish_sprite_sheet_w) finish_img_source_x = 0;
			}
		}

		//camera movement
		{
			//moving left or right
			if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_D] || player.accel_x > 0) camera.x = player.x;
		}

		//bounds collision
		{
			//left wall
			if (player.x <= 0) player.x = 0.0f;
			//right wall
			if (player.x >= screen_width - 10) player.x = screen_width - 10.0f;
			//ceiling
			if (player.y <= 0) player.y = 0.0f;
			//floor
			if (player.y >= screen_height - 10) player.y = screen_height - 10.0f;
		}
		
		//obstacle collision
		{
			if (obs_collision == 1 && paused != 1)
			{
				//gas refill
				{
					int gallon_status = obs_Collision(&player, gallon, n_gallons);
					if (gallon_status != 0) gas += 0.5f;
				}

				//bar collision
				{
					if (on_ground == 1)
					{
						int bar_status = obs_Collision(&player, bar, n_bars);
						//right side
                        if (bar_status == 2) player.vel_x = 0.2f;
                        //left side
                        if (bar_status == 4) player.vel_x = -0.2f;
                        //below
                        if (bar_status == 3) player.vel_y = 0.2f;
                        //above
                        if (bar_status == 1) player.vel_y = -0.2f;
                        //apply damage
                        if (bar_status != 0 && hurt == 0 && health != 0)
                        {
	                          health -= 10;
	                          hurt = 1;
                        }
					}
					//invincibility timer
					{
					    if (hurt == 1) time_invun++;
					    if (time_invun == 20)
					    {
						      hurt = 0;
						      time_invun = 0;
					    }
					}
				}

				//ramp collision
				{
					int ramp_status = obs_Collision(&player, ramp, n_ramps);

					if (player_touch_ramp == 0 && previous_player_touch_ramp == 0 && in_air == 0 && prev_in_air == 0 && on_ground == 1)
					{
						//left side (going up ramp)
						if (ramp_status == 4) stunt = 1;
						//right side (ramp wall)
						else if (ramp_status == 2)
						{
							player.vel_x += 0.2f;
							player_touch_ramp = 0;
						}
						//below
						else if (ramp_status == 3)
						{
							player.vel_y += 0.2f;
							player_touch_ramp = 0;
						}
						//above
						else if (ramp_status == 1)
						{
							player.vel_y += -0.2f;
							player_touch_ramp = 0;
						}
					}

					//player movement up ramp
					{
						if (stunt == 1)
						{
							player_touch_ramp = 1;
							draw_Area_Of_Gravity(renderer, ramp, aog, n_ramps);
						}
						else player_touch_ramp = 0;
						if (player_touch_ramp == 1)
						{
							degrees = -45.f;
							player.accel_x += 0.2f;
							player.accel_y += -0.2f;
							camera.x = player.x;
						}
					}

					//area of gravity
					{
						int in_area = obs_Collision(&player, aog, n_ramps);
						if (in_area == 4)
						{
							player_touch_ramp = 0;
							in_air = 1;
							on_ground = 0;
						}
						//the descent
						if (in_air == 1)
						{
							player.accel_x += -0.05f;
							player.accel_y += 0.35f;
							camera.x = player.x;
							air_time++;
						}
						//setting air timer amount to turn off gravity
						if (air_time == 40)
						{
							air_time = 0;
							in_air = 0;
							stunt = 0;
							degrees = 0.f;
							on_ground = 1;
						}
					}
				}
			}
		}

		//stuff
		{}
		
		//player reached finish area
		{
			//slow down
			if (player.x >= screen_width - 50)
			{
				gas += 0.1f;
				at_finish = 1;
				player.accel_x = 0.001f;
			}
			//respawn at the start
			{
				if (at_finish == 1) delay++;
				if (delay == 100)
				{
					init_P1(&player);
					health = 100;
					gas = 100;
					at_finish = 0;
					level += 1;
					delay = 0;
					going = 0;
					clear_prompt = 0;
					obs_collision = 0;
				}
			}
		}

		//update player
		{
			if (n_lives != 0)
			{
				if (health != 0)
				{
					if (gas != 0)
					{
						//friction
						player.vel_x *= 0.9f;
						player.vel_y *= 0.9f;
						//speed
						player.vel_x += player.accel_x;
						player.vel_y += player.accel_y;
						player.x += player.vel_x;
						player.y += player.vel_y;
					}
				}
			}
		}

		draw_Cam(renderer, &camera);

		draw_Finish_Area(renderer, &finish_line, &camera);

		//draw lanes
		{
			if (level >= 0)
			{
				//set line color to white
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				//lane 1
				SDL_RenderDrawLine(renderer, 1, 140, 799, 140);
				//lane 2
				SDL_RenderDrawLine(renderer, 1, 290, 799, 290);
				//lane 3
				SDL_RenderDrawLine(renderer, 1, 440, 799, 440);
				//lane 4
				SDL_RenderDrawLine(renderer, 1, 590, 799, 590);
			}
		}

		if (level >= 0) draw_FA_Img(renderer, finish_sprite_texture, finish_img_source_x, 0, finish_img_source_w, 32,
			finish_line.x - 100, finish_line.y, 600, 600);

		//draw levels
		{
			if (level == 1)
			{
				//draw obstacles
				{
					draw_Bar(renderer, &bar[0], &camera, 400, 450, 5, 150);
					draw_Bar(renderer, &bar[1], &camera, 500, 325, 100, 5);
					draw_Bar(renderer, &bar[2], &camera, 250, 250, 5, 100);
					draw_Bar(renderer, &bar[3], &camera, 600, 10, 5, 150);
					draw_Bar(renderer, &bar[4], &camera, 825, 100, 5, 125);
					draw_Bar(renderer, &bar[5], &camera, 1000, 300, 5, 300);
					draw_Bar(renderer, &bar[6], &camera, 1400, 50, 5, 250);
					draw_Bar(renderer, &bar[7], &camera, 1200, 400, 200, 5);
				}

				//draw ramps
				{
					draw_Obs_Img(renderer, ramp_sprite_texture, &ramp[0], &camera, 0, 0, 370, 232, 400, 300, 64, 64);
					draw_Obs_Img(renderer, ramp_sprite_texture, &ramp[1], &camera, 0, 0, 370, 232, 700, 150, 64, 64);
					draw_Obs_Img(renderer, ramp_sprite_texture, &ramp[2], &camera, 0, 0, 370, 232, 1050, 200, 64, 64);
				}

				//draw gallons
				{
					draw_Fuel_Block(renderer, &gallon[0], &camera, 750, 350, 20, 20);
					draw_Fuel_Block(renderer, &gallon[1], &camera, 600, 200, 20, 20);
					draw_Fuel_Block(renderer, &gallon[2], &camera, 1300, 300, 20, 20);
				}
			}
		}

		//draw player 
		{
			//if not out of lives and not in menus
			if (n_lives != 0 && level >= 0)
			{
				//if not dead
				if (health != 0)
				{
					draw_Player_Img(renderer, player_sprite_texture, &player, p1_img_source_x, 0, p1_img_source_w, 444, degrees);
				}
				//respawn time if dead and not out of lives
				else if (health == 0 && n_lives != 0)
				{
					going = 0;
					death_timer++;
					draw_Player_Img(renderer, fire_sprite_texture, &player, fire_img_source_x, 0, fire_img_source_w, 24, degrees);
				}
				//respawn at start
				if (death_timer == 100)
				{
					clear_prompt = 0;
					death_timer = 0;
					n_lives -= 1;
					init_P1(&player);
					health = 100;
					gas = 100;
				}
			}
		}
		
		if (level >= 0)
		{
			draw_Health_Bar(renderer, health);

			draw_Fuel_Bar(renderer, gas);
		}
		
		//draw player lives
		{
			if (n_lives != 0 && level >= 0)
			{
				if (n_lives == 3)
				{
					draw_Lives(renderer, 420, 10);
					draw_Lives(renderer, 440, 10);
					draw_Lives(renderer, 460, 10);
				}
				else if (n_lives == 2)
				{
					draw_Lives(renderer, 420, 10);
					draw_Lives(renderer, 440, 10);
				}
				else if (n_lives == 1) draw_Lives(renderer, 420, 10);
			}
		}

		//particle system
		{
			if (particle_frame_counter++ > 100)
			{
				particle_frame_counter = 0;
				float elapsed = current_time - last_particle_frame_time;
				last_particle_frame_time = current_time;
			}

			if (current_time - last_particle_spawn > 100)
			{
				last_particle_spawn = current_time;
				if (health == 0)
				{
					create_Particles(&player, &particles, current_time, 50);
					draw_Particle_Img(renderer, &particles, smoke_img_source_x, 0, smoke_img_source_w, 16, n_particles);
				}
			}

			update_Particles(&particles, current_time);
		}

		//draw labels
		{
			if (level >= 0)
			{
				draw_Text(renderer, font_texture, gtext, text_size, 10, 10);
				draw_Text(renderer, font_texture, gtext_1, text_size, 300, 10);
				draw_Text(renderer, font_texture, gtext_2, text_size, 500, 10);
			}
		}

		//game prompt and pause menu
		{
			if (n_lives != 0 && level >= 0)
			{
				if (clear_prompt == 0) draw_Text(renderer, font_texture, gtext_3, text_size, 300, 300);
				if (state[SDL_SCANCODE_P]) paused = 1;
				if (paused == 1)
				{
					draw_Text(renderer, font_texture, gtext_5, mtitle_size, 250, 200);
					draw_Text(renderer, font_texture, gtext_6, text_size, 300, 300);
					draw_Text(renderer, font_texture, gtext_7, text_size, 325, 350);

					//resume
					if (state[SDL_SCANCODE_R]) paused = 0;
					//quit
					if (state[SDL_SCANCODE_Q])
					{
						paused = 0;
						level = -2;
						init_P1(&player);
						health = 100;
						gas = 100;
						going = 0;
						n_lives = 3;
						clear_prompt = 0;
						obs_collision = 0;
					}
				}
			}
		}

		SDL_RenderPresent(renderer);

		frame_time = SDL_GetTicks() - frame_start;
		if (frame_delay > frame_time) SDL_Delay(frame_delay - frame_time);
	}

	return 0;
}