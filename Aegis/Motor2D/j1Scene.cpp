#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "Player.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Enemy.h"
#include "EnemyWorm.h"
#include "j1Pathfinding.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");

	level1 = "MAGIC_CAVES.tmx";
	level2 = "AEGIS_RUN.tmx";
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{	
	current_map = App->map;

	PlayerPt = App->j1entity_manager->CreateEntity(App->map->data.start_position.x, App->map->data.start_position.y, ENTITY_TYPE::PLAYER);

	if (PlayerPt != nullptr) {
		PlayerExists = true;
	}	

	LoadLevel(level1);

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	static const p2DynArray<iPoint>* path=nullptr;

	//if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_DOWN)
	//	App->render->CenterCamera(); 

	if (SceneLoaded) {
		PlayerPt->position.x = App->map->data.start_position.x;
		PlayerPt->position.y = App->map->data.start_position.y;
		SceneLoaded = false;
		PlayerExists = true;//no hace falta pero por si acaso
	}
	/*if (App->render->find_player) {
		App->render->FindPlayer(dt);
	}*/

	/*if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		App->pathfinding->CreatePath({ 0,0 }, {12,5 });
	}

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();*/

	/*for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint lemao;
		lemao = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		
		SDL_Rect rect = {lemao.x , lemao.y, 32, 32 };
		App->render->DrawQuad(rect,150,150,150);
	}*/
	
	if(App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)			//Save game
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)			//Save game
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {		//Load game
	/*	LoadLevel(level1);*/
		LoadLevel1NOW = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)		//Load game
		LoadLevel2NOW = true;
		
	if (LoadLevel1NOW && (PlayerPt->velocity.y * -1 > 0)) {
		LoadLevel(level1);
		LoadLevel1NOW = false;
	}
	if (LoadLevel2NOW && (PlayerPt->velocity.y * -1 > 0)) {
		LoadLevel(level2);
		LoadLevel2NOW = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)			//Load game
	{
		PlayerPt->position.x = App->map->data.start_position.x;
		PlayerPt->position.y = App->map->data.start_position.y;

		App->render->CenterCamera();
	}

	if (PlayerPt->position.x >= App->map->data.wincondition) {

		LoadLevel(level2);
		//App->render->FindPlayer(dt);
	}

	//Draw the map
	current_map->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

// Load Game State
bool j1Scene::Load(pugi::xml_node& data)
{
	
	LoadLevel(p2SString(data.attribute("level").as_string()));

	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{

	data.append_attribute("level") = current_level->GetString();

	return true;
}

void j1Scene::LoadLevel(p2SString &level_to_load) {
	
	if (&level_to_load != current_level)
	{
		App->map->CleanUp();
		App->map->Load(level_to_load.GetString());
		current_level = &level_to_load;

		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);
		RELEASE_ARRAY(data);

	}

		PlayerPt->position.x = App->map->data.start_position.x;
		PlayerPt->position.y = App->map->data.start_position.y;
		App->render->CenterCamera();
		SceneLoaded = true;
		App->render->camera.x = -100;
		App->render->find_player = true;

}
