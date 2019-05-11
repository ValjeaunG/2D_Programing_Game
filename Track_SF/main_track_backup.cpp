#include <iostream>
#include <assert.h>
using namespace std;

//include SDL header
#include "SDL2-2.0.8\include\SDL.h"
#include "SDL2-2.0.8\include\SDL_image.h"

//load libraries
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")

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
	float x, y, vel_x, vel_y;
};

struct Obstacle
{
	int x, y, w, h;
};

void init_P1(SDL_Renderer *renderer, Player *p)
{
	//player position, velocity, and size
	p->x = 100.f;
	p->y = 500.f;
	p->vel_x = 1.0f;
	p->vel_y = 1.0f;
	p->w = 10;
	p->h = 10;
}

void spawn_P1(SDL_Renderer *renderer, Player *p)
{
	//set player color to blue
	SDL_SetRenderDrawColor(renderer, 0, 0, 100, 255);
	SDL_Rect rect;

	rect.w = p->w;
	rect.h = p->h;
	rect.x = p->x;
	rect.y = p->y;

	SDL_RenderFillRect(renderer, &rect);
}

void init_Obs_Size(Obstacle *o, int w, int h) 
{
	o->w = w;
	o->h = h;
}

void draw_Bar(SDL_Renderer *renderer, Obstacle *o, int x, int y)
{
	init_Obs_Size(o, 5, 25);
	o->x = x;
	o->y = y;

	//set bar color to yellow
	SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255);
	SDL_Rect rect;

	rect.w = o->w;
	rect.h = o->h;
	rect.x = o->x;
	rect.y = o->y;

	SDL_RenderFillRect(renderer, &rect);
}

void draw_Img(SDL_Renderer *renderer, SDL_Texture *t, int src_x, int src_y, int src_w, int src_h, int dest_x, int dest_y, int dest_w, int dest_h)
{
	//image setup stuff
	SDL_Rect src;
	SDL_Rect dest;
	//define rectangle to be copied from the texture (source)
	src.x = src_x;
	src.y = src_y;
	src.w = src_w;
	src.h = src_h;
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
	//ramp sprite
	const char *image_filename = "main_track_ramp.png";
	SDL_Init(SDL_INIT_VIDEO);

	prev_key_state[256];
	keys = (unsigned char*)SDL_GetKeyboardState(NULL);

	window = SDL_CreateWindow(
		"Track",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_width, screen_height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_ACCELERATED);

	//load image
	SDL_Surface *sprite_surface = IMG_Load(image_filename);
	assert(sprite_surface);
	//create a texture from the surface
	SDL_Texture *sprite_texture = SDL_CreateTextureFromSurface(renderer, sprite_surface);
	//free the surface
	SDL_FreeSurface(sprite_surface);

	SDL_Surface *your_draw_buffer = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 32, SDL_PIXELFORMAT_RGBA32);
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	SDL_SetSurfaceBlendMode(your_draw_buffer, SDL_BLENDMODE_NONE);

	//player stuff
	Player player;
	init_P1(renderer, &player);

	//barrier stuff
	int n_bars = 7;
	Obstacle *bar = (Obstacle*)malloc(sizeof(Obstacle)*n_bars);

	//set transparency of the texture.
	SDL_SetTextureAlphaMod(sprite_texture, 255);

	for (;;)
	{
		memcpy(prev_key_state, keys, 256);

		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
		}

		//sets screen to black
		{
			//set color to black
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			//clear screen with black
			SDL_RenderClear(renderer);
		}

		spawn_P1(renderer, &player);

		//draw lanes
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

		//draw obstacles
		draw_Bar(renderer, &bar[0], 300, 450);
		draw_Bar(renderer, &bar[1], 500, 200);
		draw_Bar(renderer, &bar[2], 200, 300);
		draw_Bar(renderer, &bar[3], 600, 100);

		//draw ramps
		draw_Img(renderer, sprite_texture, 0, 0, 370, 232, 400, 300, 64, 64);
		
		//player movement/bounds and obstacle collision
		int hit = obs_Collision(&player, bar, n_bars);
		//moving left
		if (state[SDL_SCANCODE_A] && player.x != 0 && hit != 2)
		{
			player.vel_x = -0.2f;
			player.x += player.vel_x;
		}
		//moving right
		if (state[SDL_SCANCODE_D] && player.x != 790 && hit != 4)
		{
			player.vel_x = 0.2f;
			player.x += player.vel_x;
		}
		//moving up
		if (state[SDL_SCANCODE_W] && player.y != 0 && hit != 3)
		{
			player.vel_y = -0.2f;
			player.y += player.vel_y;
		}
		//moving down
		if (state[SDL_SCANCODE_S] && player.y != 590 && hit != 1)
		{
			player.vel_y = 0.2f;
			player.y += player.vel_y;
		}
		

		
		SDL_RenderPresent(renderer);
	}

	return 0;
}