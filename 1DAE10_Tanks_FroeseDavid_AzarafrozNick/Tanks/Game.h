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
// const globals:
const int g_PlayerCount{ 2 };            // how many players? (up to 4)
const float g_Scaling{ 2.0f };           // scaling of all tiles, sprites, etc.
const int g_Rows{ 15 }, g_Cols{ 20 };    // rows and columns of map
const float g_CellSize{ 24.0f };
const float g_TankSpeed{ 120.0f };
const float g_TankTurnSpeed{ g_Pi / 2 };
const int g_TankHP{ 10 };
const float g_TankSize{ 20.0f };
const float g_ProjectileSpeed{ 600.0f };
const int g_MaxProjectiles{ 3 };
const float g_ProjectileSize{ 8.0f };
const int g_MaxItems{ 2 };
const float g_MaxItemCooldown{ 3.0f };

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
	woodenBox,
	healthBox,
	bounceBox,
	last
};

struct Tile 
{
	int idx{},
		health{};
	TileState state{};
};

// structs:
struct Projectile
{
	Point2f position{};
	ProjectileState state{};
	float speed{ g_ProjectileSpeed },
		  angle{ 0 },
		  size{ 16.0f };
	bool active{};
};

struct Tank
{
	Point2f position{};
	utils::Texture texture{}, destroyedTexture{};
	float size{ g_TankSize },
		angle{},
		speed{ g_TankSpeed },
		turnSpeed{ g_TankTurnSpeed };
    int currentHP{ g_TankHP },
		maxHP{ g_TankHP };
	Projectile projectiles[g_MaxProjectiles]{};
	float itemCooldown{};
};

struct TankControls
{
	SDL_Scancode leftKey;
	SDL_Scancode rightKey;
	SDL_Scancode upKey;
	SDL_Scancode downKey;

	SDL_Keycode fireKey;
};

const TankControls g_TankControls[]  // define controls here
{
	{SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, SDLK_SPACE},
	{SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDLK_RETURN},
	{SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, SDLK_SPACE},
	{SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_W, SDL_SCANCODE_S, SDLK_SPACE}
};

const Point2f g_TankStartPositions[] // define start positions here
{
	Point2f{ 68, 68 },
	Point2f{ g_WindowWidth - 68, g_WindowHeight - 68 },
	Point2f{ 68, g_WindowHeight - 68 },
	Point2f{ g_WindowWidth - 68, 68}
};

const Point2f g_HealthbarPositions[] // define health bar positions here
{
	Point2f{ 20, 16 },
	Point2f{ g_WindowWidth - 256, g_WindowHeight - 40 },
	Point2f{ 20, g_WindowHeight - 40 },
	Point2f{ g_WindowWidth - 256, 16 },
};

// globals
Tank g_Tanks[g_PlayerCount]{};
Projectile g_Projectiles[g_MaxProjectiles]{};

bool g_GameOver{};
int g_TankWinnerIndex{};
float g_GameOverElapsedSec{};

Texture g_ProjectileStandardTexture{};
Texture g_HealthBarBackgroundTexture{};
Texture g_HealthBarTextures[g_PlayerCount]{};
Texture g_HealthBarFillingTextures[g_PlayerCount]{};
Texture g_GameOverTextures[g_PlayerCount]{};

Tile* g_pGridMap;

Texture g_TileTextures[int(TileState::last)]{};

//----------------------------------------------------------------------------------

// Declare your own functions here
void LoadTextures();
void DeleteTextures();

void InitGame();

void TurnTank(Tank& tank, float angle);
void FireProjectile(Tank& tank);
Tile CheckTileCollision(const Rectf& collisionRect);
void CheckTankCollision(Projectile& projectile);
void TakeDamage(Tank& tank, int damage);

void UpdateTanks(float elapsedSec);
void UpdateProjectiles(float elapsedSec);
void UpdateGameOver(float elapsedSec);

void DrawGrid();
void DrawTanks();
void DrawProjectiles();
void DrawHealthBars();
void DrawGameOver();

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
