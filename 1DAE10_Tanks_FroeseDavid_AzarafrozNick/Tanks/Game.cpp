#include "pch.h"
#include "Game.h"
#include <iostream>

//Basic game functions
#pragma region gameFunctions											
void Start()
{
	LoadTextures();
	InitGame();
}

void Draw()
{
	ClearBackground();

	DrawGrid();
	DrawTanks();
	DrawProjectiles();

	//-------------------- UI -----------------------------

	DrawHealthBars();
	DrawGameOver();
}

void Update(float elapsedSec)
{
	// physics
	if (!g_GameOver)
	{
		UpdateProjectiles(elapsedSec);
		UpdateTanks(elapsedSec);
	}
	else 
	{
		UpdateGameOver(elapsedSec);
	}
	
}

void End()
{
	// delete map
	delete[] g_pGridMap;
	g_pGridMap = nullptr;

	DeleteTextures();
}
#pragma endregion gameFunctions

//Keyboard and mouse input handling
#pragma region inputHandling											
void OnKeyDownEvent(SDL_Keycode key)
{

}

void OnKeyUpEvent(SDL_Keycode key)
{
	if (!g_GameOver)
	{
		for (int i{}; i < g_PlayerCount; ++i)
		{
			// shoot projectile if key pressed
			if (g_Tanks[i].currentHP > 0 && g_TankControls[i].fireKey == key)
			{
				FireProjectile(g_Tanks[i]);
			}
		}
	}
}

void OnMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "  [" << e.x << ", " << e.y << "]\n";
	//Point2f mousePos{ float( e.x ), float( g_WindowHeight - e.y ) };
}

void OnMouseDownEvent(const SDL_MouseButtonEvent& e)
{

}

void OnMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	////std::cout << "  [" << e.x << ", " << e.y << "]\n";
	//switch (e.button)
	//{
	//case SDL_BUTTON_LEFT:
	//{
	//	//std::cout << "Left mouse button released\n";
	//	//Point2f mousePos{ float( e.x ), float( g_WindowHeight - e.y ) };
	//	break;
	//}
	//case SDL_BUTTON_RIGHT:
	//	//std::cout << "Right mouse button released\n";
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	//std::cout << "Middle mouse button released\n";
	//	break;
	//}
}
#pragma endregion inputHandling

#pragma region ownDefinitions
// Define your own functions here

void LoadTextures()
{
	for (int i{}; i < g_PlayerCount; ++i)
	{
		// textures
		if (!TextureFromFile("Resources/healthbar_" + std::to_string(i) + ".png", g_HealthBarTextures[i]))
			std::cout << "Resources/healthbar_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromFile("Resources/healthbar_filling_" + std::to_string(i) + ".png", g_HealthBarFillingTextures[i]))
			std::cout << "Resources/healthbar_filling_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromFile("Resources/tank_" + std::to_string(i) + ".png", g_Tanks[i].texture))
			std::cout << "Resources/tank_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromFile("Resources/tank_destroyed_" + std::to_string(i) + ".png", g_Tanks[i].destroyedTexture))
			std::cout << "Resources/tank_destroyed_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromString("Player " + std::to_string(i + 1) + " has won!", "Resources/arialbd.ttf", 30, Color4f(1.0f, 1.0f, 1.0f, 1.0f), g_GameOverTextures[i]))
			std::cout << "Resources/tank_destroyed_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';
	}

	// tiles
	for (int i{}; i < int(TileState::last); i++)
	{
		if (!TextureFromFile("Resources/tile_" + std::to_string(i) + ".png", g_TileTextures[i]))
			std::cout << "Resources/tile_" + std::to_string(i) + ".png" << " could not be loaded!" << '\n';
	}

	// single textures
	if (!TextureFromFile("Resources/healthbar_background.png", g_HealthBarBackgroundTexture))
		std::cout << "Resources/healthbar_background.png " << "could not be loaded!" << '\n';

	if (!TextureFromFile("Resources/projectile_0.png", g_ProjectileStandardTexture))
		std::cout << "Resources/projectile_0.png " << "could not be loaded!" << '\n';
}

void DeleteTextures()
{
	// tank stuff
	for (int i{}; i < g_PlayerCount; ++i)
	{
		DeleteTexture(g_HealthBarTextures[i]);
		DeleteTexture(g_HealthBarFillingTextures[i]);
		DeleteTexture(g_GameOverTextures[i]);
		DeleteTexture(g_Tanks[i].texture);
		DeleteTexture(g_Tanks[i].destroyedTexture);
	}

	// tiles
	for (int i{}; i < int(TileState::last); i++)
	{
		DeleteTexture(g_TileTextures[i]);
	}

	// single textures
	DeleteTexture(g_HealthBarBackgroundTexture);
	DeleteTexture(g_ProjectileStandardTexture);
}

void InitGame()
{
	// delete old map if set
	delete[] g_pGridMap;
	g_pGridMap = nullptr;

	// init map
	g_pGridMap = new Tile[g_Rows * g_Cols];
	for (int i{}; i < g_Rows * g_Cols; ++i)
	{
		int row{ i / g_Cols }, col{ i % g_Cols };
		g_pGridMap[i].idx = i;
		if (row == 0 || row == g_Rows - 1 || col == 0 || col == g_Cols - 1)
		{
			g_pGridMap[i].state = TileState::unbreakableWall;
		}
		else if(row == 1 || col == 1 || row == g_Rows - 2 || col == g_Cols - 2)
		{
			g_pGridMap[i].state = TileState::empty;
		}
		else
		{
			// REMOVE THIS AND GENERATE DECENT MAPS PLS
			int randomNumber{ rand() % 101 };
			if (randomNumber <= 85)
			{
				g_pGridMap[i].state = TileState::empty;
			}
			else if (randomNumber <= 98)
			{
				g_pGridMap[i].state = TileState::unbreakableWall;
			}
			else
			{
				g_pGridMap[i].state = TileState::woodenBox;
			}
		}
	}

	for (int i{}; i < g_PlayerCount; ++i)
	{
		// init position and angle of tanks (facing to the center)
		g_Tanks[i].position = g_TankStartPositions[i];
		g_Tanks[i].angle = atan2f(g_WindowHeight / 2 - g_Tanks[i].position.y, g_WindowWidth / 2 - g_Tanks[i].position.x);
		g_Tanks[i].currentHP = g_TankHP;

		// set all projectiles to inactive!
		for (int j{}; j < g_MaxProjectiles; ++j)
		{
			g_Tanks[i].projectiles[j].active = false;
		}
	}
}

void TurnTank(Tank& tank, float angle)
{
	tank.angle += angle;

	// angle should not be greater than 2 * g_Pi
	while (tank.angle > 2 * g_Pi)
	{
		tank.angle -= 2 * g_Pi;
	}
	while (tank.angle < 0)
	{
		tank.angle += 2 * g_Pi;
	}
}

void FireProjectile(Tank& tank)
{
	for (int j{}; j < g_MaxProjectiles; ++j)
	{
		if (tank.projectiles[j].active == false)
		{
			tank.projectiles[j].active = true;
			const float offset{ 16.0f };
			tank.projectiles[j].position.x = tank.position.x + cosf(tank.angle) * offset * g_Scaling;
			tank.projectiles[j].position.y = tank.position.y + sinf(tank.angle) * offset * g_Scaling;
			tank.projectiles[j].angle = tank.angle;
			break;
		}
	}
}

Tile CheckTileCollision(const Rectf& collisionRect)
{
	// check if rectangle is colliding with wall in grid
	for (int i{}; i < g_Rows * g_Cols; i++)
	{
		int row{ i / g_Cols }, col{ i % g_Cols };
		Rectf tileRect{ col * g_CellSize * g_Scaling, row * g_CellSize * g_Scaling, g_CellSize * g_Scaling, g_CellSize * g_Scaling};
		if (g_pGridMap[i].state != TileState::empty && IsOverlapping(collisionRect, tileRect))
			return g_pGridMap[i];
	}
	return Tile{};
}

void CheckTankCollision(Projectile& projectile)
{
	for (int i{}; i < g_PlayerCount; ++i) 
	{
		Rectf projectileRect{};
		projectileRect.left = projectile.position.x - projectile.size / 2;
		projectileRect.bottom = projectile.position.y - projectile.size / 2;
		projectileRect.width = projectile.size;
		projectileRect.height = projectile.size;

		Rectf tankRect{};
		tankRect.left = g_Tanks[i].position.x - g_Tanks[i].size / 2;
		tankRect.bottom = g_Tanks[i].position.y - g_Tanks[i].size / 2;
		tankRect.width = g_Tanks[i].size;
		tankRect.height = g_Tanks[i].size;

		if (IsOverlapping(projectileRect, tankRect)) 
		{
			projectile.active = false;
			TakeDamage(g_Tanks[i], 1); // TODO: For funture: change to projectile power
		}
	}
}

void TakeDamage(Tank& tank, int damage) 
{
	tank.currentHP -= damage;
	if (tank.currentHP <= 0)
	{
		tank.currentHP = 0;
	}
	else if(tank.currentHP > tank.maxHP)
	{
		tank.currentHP = tank.maxHP;
	}
}

void UpdateTanks(float elapsedSec)
{
	// check if player has won
	int count{}, winnerIndex{};
	for (int i{}; i < g_PlayerCount; i++)
	{
		if (g_Tanks[i].currentHP > 0)
		{
			count++;
			winnerIndex = i;
		}
	}
	if (count == 1)
	{
		g_TankWinnerIndex = winnerIndex;
		g_GameOver = true;
	}

	// input for all tanks
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	for (int i{}; i < g_PlayerCount; i++)
	{
		if (g_Tanks[i].currentHP > 0)
		{
			// turning
			if (pStates[g_TankControls[i].rightKey])
			{
				TurnTank(g_Tanks[i], -g_Tanks[i].turnSpeed * elapsedSec);
			}
			else if (pStates[g_TankControls[i].leftKey])
			{
				TurnTank(g_Tanks[i], g_Tanks[i].turnSpeed * elapsedSec);
			}

			// determine move direction
			float hspd{}, vspd{};
			Rectf collisionRect
			{
				g_Tanks[i].position.x - g_Tanks[i].size * g_Scaling / 2,
				g_Tanks[i].position.y - g_Tanks[i].size * g_Scaling / 2,
				g_Tanks[i].size * g_Scaling,
				g_Tanks[i].size * g_Scaling
			};

			if (pStates[g_TankControls[i].upKey])
			{
				hspd = cosf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
				vspd = sinf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
			}
			else if (pStates[g_TankControls[i].downKey])
			{
				hspd = -cosf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
				vspd = -sinf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
			}

			// horizontal collisions
			if (hspd != 0)
			{
				collisionRect.left += hspd;
				if (CheckTileCollision(collisionRect).state != TileState::empty)
					hspd = 0;
			}

			// reset left position for vertical check
			collisionRect.left = g_Tanks[i].position.x - g_Tanks[i].size * g_Scaling / 2;

			// vertical collisions
			if (vspd != 0)
			{
				collisionRect.bottom += vspd;
				if (CheckTileCollision(collisionRect).state != TileState::empty)
					vspd = 0;
			}

			// move tank
			g_Tanks[i].position.x += hspd;
			g_Tanks[i].position.y += vspd;
		}
	}

}

void UpdateProjectiles(float elapsedSec)
{
	for (int i{}; i < g_PlayerCount; ++i) 
	{
		for (int j{}; j < g_MaxProjectiles; ++j) 
		{
			if (g_Tanks[i].projectiles[j].active)
			{
				Rectf destinationProjectile{};
				destinationProjectile.left = g_Tanks[i].projectiles[j].position.x - g_Tanks[i].projectiles[j].size / 2;
				destinationProjectile.bottom = g_Tanks[i].projectiles[j].position.y - g_Tanks[i].projectiles[j].size / 2;
				destinationProjectile.width = g_Tanks[i].projectiles[j].size;
				destinationProjectile.height = g_Tanks[i].projectiles[j].size;

				Tile tile{ CheckTileCollision(destinationProjectile) };

				if (tile.state != TileState::empty && g_Tanks[i].projectiles[j].state != ProjectileState::bouncing)
				{
					g_Tanks[i].projectiles[j].active = false;
				}
				else if(tile.state != TileState::empty)
				{
					// TODO: bouncing 
				}

				switch (tile.state)
				{
				case TileState::woodenBox: 
					g_pGridMap[tile.idx].state = TileState::empty;
					break;
				case TileState::unbreakableWall:
					break;
				case TileState::breakableWall:
					--tile.health;
					if (tile.health <= 0) 
					{
						g_pGridMap[tile.idx].state = TileState::empty;
					}
					break;
				}

				CheckTankCollision(g_Tanks[i].projectiles[j]);

				// movement
				g_Tanks[i].projectiles[j].position.x += cosf(g_Tanks[i].projectiles[j].angle) * g_Tanks[i].projectiles[j].speed * elapsedSec;
				g_Tanks[i].projectiles[j].position.y += sinf(g_Tanks[i].projectiles[j].angle) * g_Tanks[i].projectiles[j].speed * elapsedSec;

				// collision checks
				if (g_Tanks[i].projectiles[j].position.x >= g_WindowWidth || g_Tanks[i].projectiles[j].position.y >= g_WindowHeight || g_Tanks[i].projectiles[j].position.x <= 0 || g_Tanks[i].projectiles[j].position.y <= 0)
				{
					g_Tanks[i].projectiles[j].active = false;
				}
			}
		}
	}
}

void UpdateGameOver(float elapsedSec)
{
	const float secondsToRestart{ 3 };
	g_GameOverElapsedSec += elapsedSec;
	if (g_GameOverElapsedSec >= secondsToRestart)
	{
		g_GameOverElapsedSec = 0;
		g_GameOver = false;
		InitGame();
	}
}

void DrawGrid()
{
	// fill background with brown color
	Color4f backgroundColor{ 93 / 255.0f, 54 / 255.0f, 45 / 255.0f, 1.0f };
	SetColor(backgroundColor);
	FillRect(0, 0, g_WindowWidth, g_WindowHeight);

	// draw tiles
	for (int i{}; i < g_Rows * g_Cols; ++i)
	{
		Rectf destinationRect{};

		int row{ i / g_Cols }, column{ i % g_Cols }, tileState{ int(g_pGridMap[i].state) };

		destinationRect.left = column * g_CellSize * g_Scaling;
		destinationRect.bottom = row * g_CellSize * g_Scaling;
		destinationRect.width = g_CellSize * g_Scaling;
		destinationRect.height = g_CellSize * g_Scaling;
		DrawTexture(g_TileTextures[tileState], destinationRect);
	}
}

void DrawTanks()
{
	// draw all tanks
	for (int i{}; i < g_PlayerCount; i++)
	{
		// determine texture
		Rectf destinationRect{};
		Texture texture{};
		if (g_Tanks[i].currentHP > 0)
		{
			texture = g_Tanks[i].texture;
		}
		else 
		{
			texture = g_Tanks[i].destroyedTexture;
		}

		// draw texture
		destinationRect.left = g_Tanks[i].position.x - (texture.width / 2) * g_Scaling;
		destinationRect.bottom = g_Tanks[i].position.y - (texture.height / 2) * g_Scaling;
		destinationRect.width = texture.width * g_Scaling;
		destinationRect.height = texture.height * g_Scaling;
		DrawTexture(texture, destinationRect, g_Tanks[i].angle);
	}
}

void DrawProjectiles() 
{
	for (int i{}; i < g_PlayerCount; ++i) 
	{
		for (int j{}; j < g_MaxProjectiles; ++j) 
		{
			// only draw if active
			if (g_Tanks[i].projectiles[j].active) 
			{
				Rectf destinationProjectile{};
				destinationProjectile.left = g_Tanks[i].projectiles[j].position.x - g_ProjectileStandardTexture.width / 2;
				destinationProjectile.bottom = g_Tanks[i].projectiles[j].position.y - g_ProjectileStandardTexture.height / 2;
				destinationProjectile.width = g_ProjectileStandardTexture.width * g_Scaling;
				destinationProjectile.height = g_ProjectileStandardTexture.height * g_Scaling;
				DrawTexture(g_ProjectileStandardTexture, destinationProjectile, g_Tanks[i].projectiles[j].angle);

				SetColor(1.0f, 1.0f, 1.0f);
			}
		}
	}
}

void DrawHealthBars() 
{
	for (int i{}; i < g_PlayerCount; ++i)
	{
		// border
		Rectf destinationHealth{};

		destinationHealth.left = g_HealthbarPositions[i].x;
		destinationHealth.bottom = g_HealthbarPositions[i].y;
		destinationHealth.width = g_HealthBarTextures[i].width * g_Scaling;
		destinationHealth.height = g_HealthBarTextures[i].height * g_Scaling;
		DrawTexture(g_HealthBarTextures[i], destinationHealth);

		// background
		destinationHealth.width = g_HealthBarBackgroundTexture.width * g_Scaling;
		destinationHealth.height = g_HealthBarBackgroundTexture.height * g_Scaling;
		DrawTexture(g_HealthBarBackgroundTexture, destinationHealth);
		if (g_Tanks[i].currentHP > 0)
		{
			// filling
			float hpScale = g_Tanks[i].currentHP / float(g_Tanks[i].maxHP),
				  xOffset = 16.0f * g_Scaling, yOffset = 3.0f * g_Scaling;
			destinationHealth.left = g_HealthbarPositions[i].x + xOffset;
			destinationHealth.bottom = g_HealthbarPositions[i].y + yOffset;
			destinationHealth.width = g_HealthBarFillingTextures[i].width * g_Scaling * hpScale;
			destinationHealth.height = g_HealthBarFillingTextures[i].height * g_Scaling;
			DrawTexture(g_HealthBarFillingTextures[i], destinationHealth);
		}
	}
}
void DrawGameOver()
{
	if (g_GameOver)
	{
		const float width{ 300 * g_Scaling }, height{ 100 * g_Scaling };
		const Color4f color{ 33 / 255.0f, 31 / 255.0f, 51 / 255.0f, 0.75f };
		SetColor(color);
		FillRect(Point2f(g_WindowWidth / 2 - width / 2, g_WindowHeight / 2 - height / 2), width, height);

		Texture texture{ g_GameOverTextures[g_TankWinnerIndex] };
		Rectf destinationRect{};
		destinationRect.left = g_WindowWidth / 2 - texture.width / 2 * g_Scaling;
		destinationRect.bottom = g_WindowHeight / 2 - texture.height / 2 * g_Scaling;
		destinationRect.width = texture.width * g_Scaling;
		destinationRect.height = texture.height * g_Scaling;
		DrawTexture(texture, destinationRect);
	}
}
#pragma endregion ownDefinitions