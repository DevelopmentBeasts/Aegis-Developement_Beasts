#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "Player.h"
#include <math.h>
#include "j1Input.h"

//Destructor

PlayerClass::~PlayerClass() {

}
PlayerClass::PlayerClass() {   //DO PUSHBACKS WITH XML
	name.create("player");

	pugi::xml_parse_result result = AnimsDoc.load_file("PlayerAnims.xml");

	if (result == NULL) {
		LOG("The xml file that contains the pushbacks for the animations is not working.PlayerAnims.xml. error: %s",result.description());
	}
	
	AnimsNode = AnimsDoc.child("config").child("AnimsCoords").child("idle_left");
	idle_left.LoadPushbacks(AnimsNode);
	AnimsNode = AnimsDoc.child("config").child("AnimsCoords").child("run_left");
	run_left.LoadPushbacks(AnimsNode);
}

bool PlayerClass::Start() {
	
	bool ret = true;
	pugi::xml_parse_result result = PlayerStartFile.load_file("StartPlayerConfig.xml");
	if (result == NULL) {
		LOG("Could not load StartPlayerConfig.xml. pugi error: %s", result.description());
		ret = false;
	}
	if (ret == true) {
		//Load all Player starter info
		LOG("Loading player info at the start of the game");
	}
	PlayerXmlNode = PlayerStartFile.child("config").child("player");
	if (PlayerXmlNode == NULL) {
		LOG("PlayerXmlNode is not reading correctly");
	}
	//Asigment of the values
	const char* type1 = PlayerXmlNode.attribute("type").as_string();
	
	if (type1 == "FIRE_WISP") {
		data.type = FIRE_WISP;
	}
	if (type1 == "WATER_WISP") {
		data.type = WATER_WISP;
	}
	if (type1 == "ROCK_WISP") {
		data.type = ROCK_WISP;
	}
	if (type1 == "WISP_WISP") {
		data.type = WIND_WISP;
	}
	data.xpos = PlayerXmlNode.attribute("xpos").as_uint();
	data.ypos = PlayerXmlNode.attribute("ypos").as_uint(); 
	data.yvel = PlayerXmlNode.attribute("yvel").as_uint(); //player stats
	data.xvel = PlayerXmlNode.attribute("xvel").as_uint();
	data.PlayerOnTop = PlayerXmlNode.attribute("PlayerOnTop").as_bool();
	data.PlayerColliding = PlayerXmlNode.attribute("PlayerColliding").as_bool();

	

	playerrect.w = PlayerXmlNode.child("playerrect").attribute("w").as_int(); //the rect that contains the player in wich we blit()
	playerrect.h = PlayerXmlNode.child("playerrect").attribute("h").as_int();

	StaminaRect.w = PlayerXmlNode.child("StaminaRect").attribute("w").as_int();
	StaminaRect.h = PlayerXmlNode.child("StaminaRect").attribute("h").as_int();
	StaminaRect.x = PlayerXmlNode.child("StaminaRect").attribute("x").as_int(); //info of the stamina rect
	StaminaRect.y = PlayerXmlNode.child("StaminaRect").attribute("y").as_int();
	LOG("Resseting anims");

	
	idle_left.Reset();
	run_left.Reset();

	jump_right.Reset();
	jump_left.Reset();

	fall_left.Reset();
	attack_right.Reset();
	attack_left.Reset();

	LOG("LOADING PLAYER TEXTURES");

	Textures = App->tex->Load("textures/Fire_Wisp/fireSheet.png");

	current_animation = &idle_left;

	LOG("CREATING PLAYER COLLIDER");
	PlayerCollider = App->collision->AddCollider({ data.xpos, data.ypos, playerrect.w, playerrect.h }, COLLIDER_PLAYER, this);



	//a collider that needs to be initialized and its values are redefinited in ONCOLLISION FUNCTION
	TheWallCollider = App->collision->AddCollider({ NULL, NULL, NULL, NULL }, COLLIDER_NONE, this);
	
	return ret;
}


bool PlayerClass::Update(float dt) {


	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		godmode_activated = !godmode_activated;

	if (godmode_activated == false) {
		MovePlayer();
    MovePlayerCollider();
		PlayerAnims();
	}
	else
		GodMode();
	
	return true;

}

void PlayerClass::MovePlayer() {
	
	//IMPORTANT!!!
	//MAXIMUN SPEED IN Y AXIS FOR THE PLAYER MUST BE UNDER 26.5. IF NOT, THE PROGRAM CAN NOT DETECT IT WHEN COLLIDING WITH A COLLIDER IN ITS TOP SIDE
	//WITH A VALUE OF -0.3 OF ACCELERATION, THE MAXIMUN HEIGHT TO JUMP AND DON'T PASS THE MAXIMUN SPEED IS: 870.
	
	
	movingleft = false;
	movingright = false; //DEFAULT FOR BEING IN IDL ANIMATION IF NOT MOVING

	//important to know that playerrect is not the player itself, it is just a rectangle with the same position w and h as the player.
	int Px = playerrect.x;
	int Py = playerrect.y;
	int Pw = playerrect.w;
	int Ph = playerrect.h;
	int Cx = TheWallCollider->rect.x;
	int Cy = TheWallCollider->rect.y;
	int Cw = TheWallCollider->rect.w;
	int Ch = TheWallCollider->rect.h;

	
	if (data.PlayerColliding) {
		if (Px < (Cx + 8)/*if player is in the left of the C*/) {
			if (((Py < Cy) && (Py > (Cy + Ch))/*Py is inside Ch*/ || (((Py + Ph) < (Cy + Ch)) && ((Py + Ph) > Cy)))/**/) {
				data.PlayerOnLeft = true;
				LOG("PLAYER ON LEFT");
				data.PlayerColliding = true;
			}
			if (!(((Py < Cy) && (Py > (Cy + Ch))/*Py is inside Ch*/ || (((Py + Ph) < (Cy + Ch)) && ((Py + Ph) > Cy)))/**/)) {
				data.PlayerOnLeft = false;
				LOG("PLAYER NOT ON LEFT ANYMORE");
				data.PlayerColliding = false;
			}
		}

		if (Px > (Cx + Cw - 8)/*if player is in the left of the C*/) {
			if (((Py < Cy) && (Py > (Cy + Ch))/*Py is inside Ch*/ || (((Py + Ph) < (Cy + Ch)) && ((Py + Ph) > Cy)))/**/) {
				data.PlayerOnRight = true;
				LOG("PLAYER ON RIGHT");
				data.PlayerColliding = true;
			}
			if (!(((Py < Cy) && (Py > (Cy + Ch))/*Py is inside Ch*/ || (((Py + Ph) < (Cy + Ch)) && ((Py + Ph) > Cy)))/**/)) {
				data.PlayerOnRight = false;
				LOG("PLAYER NOT ON RIGHT ANYMORE");
				data.PlayerColliding = false;
			}
		}
		
			if ((Px > Cx) && (Px < (Cx + Cw))/*Px inside Cw*/ || (((Px + Pw) > Cx) && ((Px + Pw) < (Cx + Cw)))/*(Px+Pw) inside Cw*/) {
				if ((Py + Ph) < (Cy + 20)/*if (Py+Ph) is inside the C but not more than 20p*/ && !((Py< (Cy + 20))&&(Py>(Cy+Ch)))) {
					if (!data.PlayerOnLeft && !data.PlayerOnRight) {
						data.PlayerOnTop = true;
						LOG("PLAYER ON TOP");
						data.PlayerColliding = false;
					}			
				}
			}

			if ((Px > Cx) && (Px < (Cx + Cw))/*Px inside Cw*/ || (((Px + Pw) > Cx) && ((Px + Pw) < (Cx + Cw)))/*(Px+Pw) inside Cw*/) {
				if ((Py > (Cy + (Ch - 8)))) {
					data.PlayerOnBot = true;
					LOG("PLAYER ON BOT");
					data.PlayerColliding = false;
				}

			}
		/*if (playerrect.x || (playerrect.x + playerrect.w) > (TheWallCollider->rect.x + 10) && (playerrect.x || (playerrect.x + playerrect.w) < (TheWallCollider->rect.x + TheWallCollider->rect.w))) {
			if (playerrect.y > (TheWallCollider->rect.y - 10)) {
				data.PlayerOnBot = true;
			}
		}*/

	}
	
	if (data.PlayerOnTop) {
		jumping = false;
		data.Falling = false;
		data.ypos = Cy - Ph-1;
		data.PlayerOnBot = false;
	}
	if (data.PlayerOnLeft) {
		automatic_right = false;
		jumping = false;
		data.Falling = true;
		if (!data.PlayerColliding) {
		data.PlayerOnLeft = false;
		}
		data.PlayerOnBot = false;
	}
	if (data.PlayerOnRight) {
		automatic_left = false;
		jumping = false;
		data.Falling = true;
		if (!data.PlayerColliding) {
			data.PlayerOnRight = false;
		}
		data.PlayerOnBot = false;
	}
	if (data.PlayerOnBot) {	
		data.Falling = true;
		jumping = false;
	}
	if (Cx != NULL && Cy != NULL && Cw != NULL && Ch != NULL) {
		if (((data.xpos < (Cx - Pw))/*if player wants to fall in the left side*/ || (Px > (Cx + Cw)) )&&!jumping) {
			data.PlayerOnTop = false;
			data.Falling = true;
		}
	}
	if (data.Falling) {
		data.ypos += data.yvel;
		data.yvel += 0.6;
	}

//___________________________________________________________________________________________________________________________________________________
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !data.PlayerOnLeft) {
		movingleft = false;
		movingright = true;
		if (!automatic_right) {
			data.xpos += data.xvel;
		}
		if (jumping) {
			automatic_left = false;
			automatic_right = true;
		}
		if (data.Falling) {
			automatic_left = false;
			automatic_right = true;
		}
	}
	if (automatic_right) {
		data.xpos += (data.xvel + 3);
	}
//__________________________________________________________
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !data.PlayerOnRight) {
		movingleft = true;
		movingright = false;
		if (!automatic_left) {
			data.xpos -= data.xvel;
		}
		if (jumping) {
			automatic_right = false;
			automatic_left = true;
		}
		if (data.Falling) {
			automatic_right = false;
			automatic_left = true;
		}
	}
	if (automatic_left) {
		data.xpos -= (data.xvel + 3);
	}
//___________________________________________________________
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && !data.PlayerOnBot && !data.PlayerOnLeft && !data.PlayerOnRight && !automatic_right && !automatic_right) {
		if (!jumping && !data.Falling) {
			jumping = true;
			data.yvel = 9;	
		}
	}
	if (jumping) {
		if (!data.PlayerOnLeft && !data.PlayerOnRight) {
			data.ypos -= data.yvel;
			data.yvel -= 0.3;
			data.PlayerOnTop = false;
		}
		if (data.PlayerOnLeft || data.PlayerOnRight) {
			data.ypos -= data.yvel;
			data.yvel -= 0.7;
			data.PlayerOnTop = false;
		}
		
	}
	if (!jumping && !data.Falling) {
		data.yvel = 0;
		automatic_left = false;
		automatic_right = false;
	}
//___________________________________________________________
	
	

	//PLAYER RECT POSITION USED FOR USEFULL THINGS IS BEIG ACTUALIZED
	playerrect.x = data.xpos;
	playerrect.y = data.ypos;

	



	
















	//ASASAS
	//ASASASA
	//if (!data.PlayerColliding && !fall_atack/*||data.PlayerOnLeft||data.PlayerOnRight*/) { //descomentar esto cuando est�n hechas las colisiones laterales
	//	jumping = true;
	//	data.PlayerOnTop = false;
	//}

	////lets make the player fall down by default
	//if (!data.PlayerOnTop && !jumping) {
	//	data.yvel += 0.3;
	//	data.ypos += data.yvel;
	//}
	////conditions depending on the collision pos
	//if (data.PlayerOnTop) {  //HERE NEEDS TO STOP THE Y MOVEMENT (NOT DONE YET)
	//	data.PlayerOnLeft = false;
	//	data.PlayerOnRight = false;
	//	data.yvel = 0;
	//	data.ypos = TheWallCollider->rect.y - (playerrect.h - 1);
	//	jumping = false;
	//	fall_atack = false;
	//	data.PlayerColliding = false;
	//}
	//if (data.PlayerOnLeft) {
	//	automatic_right = false;
	//}
	//if (data.PlayerOnRight) {
	//	automatic_left = false;
	//}


	////usefull for idl anim
	//movingleft = false;
	//movingright = false;

	////_______________________________________________________________

	//if ((App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) && !data.PlayerOnLeft) {
	//	movingleft = false;
	//	movingright = true;
	//	automatic_left = false;
	//	if (!automatic_right) {
	//		data.xpos += data.xvel;
	//	}
	//	if (jumping) {
	//		automatic_right = true;
	//	}
	//}
	//if (automatic_right) {
	//	data.xpos += (data.xvel + 3);
	//}

	////________________________________________________________________

	//if ((App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) && !data.PlayerOnRight) {
	//	movingleft = true;
	//	movingright = false;
	//	automatic_right = false;
	//	if (!automatic_left) {
	//		data.xpos -= data.xvel;
	//	}
	//	if (jumping) {
	//		automatic_left = true;
	//	}
	//}
	//if (automatic_left) {
	//	data.xpos -= (data.xvel + 3);
	//}

	////_________________________________________________________________


	//if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) { //ONLY USED FOR THE FALL ATACK
	//	if (data.yvel > -6 && jumping) {
	//		if (StaminaRect.w >= 121) {
	//			StaminaRect.w -= 120;
	//			fall_atack = true;
	//			jumping = false;
	//			automatic_left = false;
	//			automatic_right = false;
	//		}

	//	}
	//}

	//if (fall_atack && !data.PlayerOnTop) {
	//	data.yvel = 15;
	//	data.ypos += data.yvel;
	//	data.yvel += 2;
	//}

	////_________________________________________________________________

	//if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
	//	if (!jumping) {
	//		data.PlayerOnTop = false;
	//		data.PlayerColliding = false;
	//		jumping = true;
	//		data.yvel = 9;
	//		//yposaux = data.ypos; never gonna use this again erase when stop editing player movement
	//	}
	//}

	//if (jumping) {
	//	data.ypos -= data.yvel;
	//	if (!data.PlayerOnLeft && !data.PlayerOnRight) {
	//		data.yvel -= 0.3;
	//	}
	//	if (data.PlayerOnLeft || data.PlayerOnRight) {
	//		data.yvel -= 0.6;
	//	}

	//	if (data.PlayerOnTop && data.PlayerColliding) {
	//		data.ypos = TheWallCollider->rect.y - playerrect.h;
	//		jumping = false;
	//	}
	//	//data.PlayerOnTop = false; //hhhhhhhhhhh
	//}

	//if (!jumping) { //STAMINA AMOUNT CONTROL
	//	automatic_left = false;
	//	automatic_right = false;
	//	if (StaminaRect.w <= 300) {
	//		StaminaRect.w += 1;
	//	}
	//}

	////PLAYER RECT POSITION USED FOR USEFULL THINGS IS BEIG ACTUALIZED
	//playerrect.x = data.xpos;
	//playerrect.y = data.ypos;

}

bool PlayerClass::Save(pugi::xml_node& node)const{
	
	pugi::xml_node pos = node.append_child("position");

	pos.append_attribute("x") = data.xpos;
	pos.append_attribute("y") = data.ypos;

	return true;
}

bool PlayerClass::Load(pugi::xml_node& node) {

	data.xpos = node.child("position").attribute("x").as_int();
	data.ypos = node.child("position").attribute("y").as_int();
	return true;
}

void PlayerClass::PlayerAnims() {

	

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		SCANCODE_D = true;
		SCANCODE_A = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		SCANCODE_A = true;
		SCANCODE_D = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		SCANCODE_W = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		SCANCODE_S = true;
	}
	

	//LAST DIRECTION RECORD (usefull to continue looking the same side when we are idl and we run or jump
	
	if (SCANCODE_D == true ) {  //RIGHT 
		LastDirectionLeft = false;
		LastDirectionRight = true;	
	}
	if (SCANCODE_A == true) {
		LastDirectionLeft = true;
		LastDirectionRight = false;
	}

	//ANIMS BASED ON LAST DIRECTION RECORD


	// JUMP UP STRAIGHT (going up)

	if (jumping  && (data.yvel > 0) && !movingleft && !movingright && !automatic_left && !automatic_right) {  //looking left
		current_animation = &run_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, 90.0, SDL_FLIP_NONE, 1, 1, 1.0);
	}

	// JUMP UP STRAIGHT (going down)

	if (jumping && (data.yvel < 0) && !movingleft && !movingright && !automatic_left && !automatic_right) {  //looking left
		current_animation = &run_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, 90.0, SDL_FLIP_HORIZONTAL, 1, 1, 1.0);
	}

	//GOING DOWN AS ATACK

	if (!jumping && (data.yvel < 0) && !movingleft && !movingright && !automatic_left && !automatic_right) { //looking the left
		current_animation = &run_left;
    
		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, 90, SDL_FLIP_HORIZONTAL, 1, 1, 1.0);
	}


	//JUMP LEFT
	if (jumping && (data.yvel > 0) && LastDirectionLeft  && automatic_left) {  //JUMPING UP + LEFT
		current_animation = &run_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, data.yvel * 4, SDL_FLIP_NONE, 1, 1, 1.0);
	}
	if (jumping && (data.yvel < 0) && LastDirectionLeft && automatic_left) {  //JUMPING DOWN + LEFT
		current_animation = &run_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, data.yvel * 4, SDL_FLIP_NONE, 1, 1, 1.0);
	}

	//MOVE LEFT 
	if (!jumping && movingleft) {  //NOT  JUMPING + MOVING LEFT
		current_animation = &run_left;
		
		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos,&CurrentAnimationRect, 1, 0.0, SDL_FLIP_NONE, 1, 1, 1.0);
	}


	//MOVE RIGHT 
	if (!jumping && movingright) {  //NOT  JUMPING + MOVING LEFT
		current_animation = &run_left;
		
		CurrentAnimationRect = current_animation->GetCurrentFrame();

	App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, 0.0, SDL_FLIP_HORIZONTAL, 1, 1, 1.0);
	}
	// JUMP RIGHT

	if (jumping && (data.yvel > 0) && LastDirectionRight && automatic_right) {  //JUMPING UP + LEFT
		current_animation = &run_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect,1, data.yvel * (-4), SDL_FLIP_HORIZONTAL, 1, 1, 1.0);
	}
	if (jumping && (data.yvel < 0) && LastDirectionRight && automatic_right) {  //JUMPING DOWN + LEFT
		current_animation = &run_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, data.yvel * (-4), SDL_FLIP_HORIZONTAL, 1, 1, 1.0);
	}

	// IDEL LEFT
	if (LastDirectionLeft && !jumping && !movingleft) { 
		current_animation = &idle_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, data.yvel * (-4), SDL_FLIP_NONE, 1, 1, 1.0);
	}

	//IDEL RIGHT
	if (LastDirectionRight && !jumping && !movingright) {   
		current_animation = &idle_left;

		CurrentAnimationRect = current_animation->GetCurrentFrame();

		App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &CurrentAnimationRect, 1, data.yvel * (-4), SDL_FLIP_HORIZONTAL, 1, 1, 1.0);
	}

	//App->render->DrawQuad(playerrect, 34, 255, 90, 100); //used for debugging player positions, DO NOT ERASE PLEASE!!!!!!!!!

	//App->render->DrawQuad(StaminaRect, 0, 0, 255, 100);
}

void PlayerClass::MovePlayerCollider() {
	PlayerCollider->rect.x = data.xpos;
	PlayerCollider->rect.y = data.ypos;
	PlayerCollider->rect.w = playerrect.w;
	PlayerCollider->rect.h = playerrect.h;
}

void PlayerClass::OnCollision(Collider *c1, Collider *c2) {

	if ((c1->type == COLLIDER_TYPE::COLLIDER_WALL  &&  c2->type == COLLIDER_TYPE::COLLIDER_WALL ) || (c1->type == COLLIDER_PLAYER && c2->type==COLLIDER_TYPE::COLLIDER_WALL)) {
		data.PlayerColliding = true;
	}
	else if (!((c1->type == COLLIDER_TYPE::COLLIDER_WALL  &&  c2->type == COLLIDER_TYPE::COLLIDER_PLAYER) || (c1->type == COLLIDER_PLAYER  && c2->type == COLLIDER_TYPE::COLLIDER_WALL))) {
		data.PlayerColliding = false;
	}
		
	if (c1->type == COLLIDER_TYPE::COLLIDER_WALL) { //HERE WE PASS THE COLLIDER INFO TO THE COLLIDER DECLARED IN PLAYER.H AS "TheWallCollider
		TheWallCollider->rect.x = c1->rect.x;
		TheWallCollider->rect.y = c1->rect.y;
		TheWallCollider->rect.w = c1->rect.w;
		TheWallCollider->rect.h = c1->rect.h;
	}
	if (c2->type == COLLIDER_TYPE::COLLIDER_WALL) {
		TheWallCollider->rect.x = c2->rect.x;
		TheWallCollider->rect.y = c2->rect.y;
		TheWallCollider->rect.w = c2->rect.w;
		TheWallCollider->rect.h = c2->rect.h;
	}
//	App->render->DrawQuad(playerrect, 0, 255, 0, 100); //used for debugging player positions, DO NOT ERASE PLEASE!!!!!!!!!

	App->render->DrawQuad(StaminaRect, 0, 0, 255, 100);
}

void PlayerClass::GodMode() {									//The player flies and ignores collisions
	
	current_animation = &idle_left;

	if (App->input->GetKey(SDL_SCANCODE_A)==KEY_REPEAT)
		data.xpos -= data.xvel;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		data.xpos += data.xvel;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		data.ypos -= data.xvel;

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		data.ypos += data.xvel;

	App->render->Blit(Textures, (int)data.xpos, (int)data.ypos, &current_animation->GetCurrentFrame(), 1, 0, SDL_FLIP_NONE, 1, 1, 1.0);

}