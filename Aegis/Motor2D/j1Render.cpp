#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "player.h"
#include "j1Input.h"

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	//laod borders
	pugi::xml_node borders= config.child("borders");
	top_border = borders.attribute("top").as_int();
	bot_border = borders.attribute("bot").as_int();
	left_border = borders.attribute("left").as_int();
	right_border = borders.attribute("right").as_int();

	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_F8)==KEY_DOWN)
		debug=!debug;

	if (find_player)
		FindPlayer();

	if (debug == true) {
		FreeMovement();
	}
	else {
		FollowPlayer();
	}

	return true;
}

bool j1Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle,SDL_RendererFlip flip, int pivot_x, int pivot_y,float extrascale) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * (scale*extrascale);
	rect.y = (int)(camera.y * speed) + y * (scale*extrascale);

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle,0, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

void j1Render::CenterCamera(){
	camera.x = left_border - App->player->data.xpos;
	camera.y = top_border - App->player->data.ypos;
		
}

void j1Render::FreeMovement() {
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		camera.y += 30;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		camera.y -= 30;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		camera.x += 30;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		camera.x -= 30;
}

void j1Render::FollowPlayer() {
	if ((App->player->data.xpos) > (-camera.x + camera.w - right_border)) {	//Move the camera to the right if the player is advancing and ahead of the border

		if (App->player->automatic_right == true)							//If the player jumps while going to a side, his velocity.x increases, so we increase
			camera.x -= App->player->data.xvel + 3;							//the velocity of the camera aswell
		else
			camera.x -= App->player->data.xvel;
	}
	if ((App->player->data.xpos) < (-camera.x + left_border)) {				//Move the camera to the left if the player is going back and behnid the left border

		if (App->player->automatic_left == true)								//If the player jumps while going to a side, his velocity.x increases, so we increase
			camera.x += App->player->data.xvel + 3;							//the velocity of the camera aswell
		else
			camera.x += App->player->data.xvel;
	}

	if ((App->player->data.ypos) < (-camera.y + top_border))					//Move the camera upwards if the player is going up and above the top border
		camera.y += App->player->data.xvel;

	if ((App->player->data.ypos) > (-camera.y + camera.h - bot_border))		//Move the camera upwards if the player is going up and above the top border
		camera.y -= App->player->data.xvel;
}

void j1Render::FindPlayer() {
	int vel = 20;

	if ((App->player->data.xpos) > (-camera.x + camera.w - right_border)) {	

		if (App->player->automatic_right == true)							
			camera.x -=vel;													
		else
			camera.x -= vel;
	}
	if ((App->player->data.xpos) < (-camera.x + left_border)) {				

		if (App->player->automatic_left == true)							
			camera.x += vel;												
		else
			camera.x += vel;
	}

	if ((App->player->data.ypos) < (-camera.y + top_border))				
		camera.y += vel;

	if ((App->player->data.ypos) > (-camera.y + camera.h - bot_border))		
		camera.y -= vel;

	if (((App->player->data.xpos) < (-camera.x + camera.w - right_border))		//If we found the player, stop looking for it
		&& ((App->player->data.xpos) > (-camera.x + left_border))
		&& ((App->player->data.ypos) > (-camera.y + top_border))
		&& ((App->player->data.ypos) < (-camera.y + camera.h - bot_border)))
		find_player = false;
}