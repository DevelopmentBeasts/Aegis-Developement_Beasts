#include "EnemyWorm.h"
#include "j1Scene.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "Player.h"
#include "p2Defs.h"
#include "EntityManager.h"
#include "j1Map.h"
#include "j1PathFinding.h"

EnemyWorm::EnemyWorm(iPoint pos): j1Enemy(pos, ENEMY_TYPE::WORM) {
	
	properties_node = App->j1entity_manager->properties_doc.child("properties").child("worm");

	idle.LoadPushbacks(properties_node.child("idle"));
	move.LoadPushbacks(properties_node.child("move"));
}

EnemyWorm::~EnemyWorm() {}

bool EnemyWorm::Start() {

	texture= App->j1entity_manager->worm_texture;
	debug_texture = App->j1entity_manager->debug_texture;
	current_animation = &idle;
	return true;
}

bool EnemyWorm::Update(float dt) {
	
	Draw();
	
	iPoint to = App->scene->PlayerPt->position;
	App->pathfinding->CreatePath(position, to);
	
	path = App->pathfinding->GetLastPath();

	DrawPath();
	return true;
}

void EnemyWorm::DrawPath() {

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
	}
}