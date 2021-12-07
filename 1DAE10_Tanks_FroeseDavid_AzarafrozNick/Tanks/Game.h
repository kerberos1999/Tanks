#pragma once
using namespace utils;
#pragma region gameInformation
// Set your name and group in the title here
std::string g_WindowTitle{ "Tanks - Froese_Azarafroz, David_Nick - 1DAE11/10" };

// Change the window dimensions here
float g_WindowWidth{ 960 };
float g_WindowHeight{ 720 };
#pragma endregion gameInformation



#pragma region ownDeclarations
// enums:
enum class ProjectileState
{
	normal,
	bouncing
};
enum class TileState
{
	empty,
	unbreakableWall,
	breakableWall,
	woodenBox
};

// structs:
struct Projectile
{
	Point2f position{};
	ProjectileState state{};
	float speed{ 500.0f };
	float angle{};
	bool active{};
	// TODO:  Warning fix ==> Severity	Code	Description	Project	File	Line	Suppression State  
	// Warning	C26495	Variable 'Projectile::angle' is uninitialized.Always initialize a member variable(type.6).Tanks	C : \Users\nicka\OneDrive\Dokumente\GitHub\Tanks\1DAE10_Tanks_FroeseDavid_AzarafrozNick\Tanks\Game.h	37

};

struct Tank
{
	Point2f position;
	utils::Texture texture;
	float angle;
	float speed;
	float turnSpeed;
	float currentHP;
	float maxHP;

	Projectile* projectiles;
};

struct TankControls
{
	SDL_Scancode leftKey;
	SDL_Scancode rightKey;
	SDL_Scancode upKey;
	SDL_Scancode downKey;

	SDL_Keycode fireKey;
};

// const globals:
const float g_Scaling{ 2.0f };           // scaling of all tiles, sprites, etc.
const float g_CellSize{ 16.0f };         
const int g_PlayerCount{ 2 };            // how many players? (up to 4)
const float g_TankSpeed{ 100.0f };
const float g_TankTurnSpeed{ g_Pi / 2 };
const float g_TankHP{ 10.0f };

const TankControls g_TankControls[]  // define controls here
{
	{SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, SDLK_SPACE},
	{SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDLK_RETURN},
	{}
};

const Point2f g_TankStartPositions[] // define start positions here
{
	Point2f{ 64, 64 },
	Point2f{ g_WindowWidth - 64, g_WindowHeight - 64 },
	Point2f{ 64, g_WindowHeight - 64 },
	Point2f{ g_WindowWidth - 64, 64}
};

const Point2f g_HealthbarPositions[] // define health bar positions here
{
	Point2f{ 20, g_WindowHeight - 40 },
	Point2f{ g_WindowWidth - 174, g_WindowHeight - 40 },
	Point2f{ 20, 20 },
	Point2f{ g_WindowWidth - 174, 20 },
};
//const int g_MaxProjectiles{ 3 }; // max projectile count per player

// globals
Tank g_Tanks[g_PlayerCount]{};
Projectile g_Projectile; // TODO ==> Projectile Array => MuniLager

Texture g_ProjectileStandardTexture{};

Texture g_HealthBarBackgroundTexture{};
Texture g_HealthBarTextures[g_PlayerCount]{};
Texture g_HealthBarFillingTextures[g_PlayerCount]{};

//----------------------------------------------------------------------------------

// Declare your own functions here
void InitTanks();
void DeleteTanks();

void UpdateTanks(float elapsedSec);

void TurnTank(Tank& tank, float angle);

void UpdateProjectiles(float elapsedSec);

void DrawTanks();
void DrawProjectiles();
void DrawHealthBars();

#pragma endregion ownDeclarations

#pragma region gameFunctions											
void Start();
void Draw();
void Update(float elapsedSec);
void End();
#pragma endregion gameFunctions

#pragma region inputHandling											
void OnKeyDownEvent(SDL_Keycode key);
void OnKeyUpEvent(SDL_Keycode key);
void OnMouseMotionEvent(const SDL_MouseMotionEvent& e);
void OnMouseDownEvent(const SDL_MouseButtonEvent& e);
void OnMouseUpEvent(const SDL_MouseButtonEvent& e);
#pragma endregion inputHandling
