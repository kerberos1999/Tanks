#include "pch.h"
#include "Game.h"
#include <iostream>

//Basic game functions
#pragma region gameFunctions											
void Start()
{
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
}

void Update(float elapsedSec)
{
	// physics
	
	UpdateProjectiles(elapsedSec);
	UpdateTanks(elapsedSec);
}

void End()
{
	//DeleteTextures();
	DeleteGame();
}
#pragma endregion gameFunctions

//Keyboard and mouse input handling
#pragma region inputHandling											
void OnKeyDownEvent(SDL_Keycode key)
{

}

void OnKeyUpEvent(SDL_Keycode key)
{
	for (int i{}; i < g_PlayerCount; ++i) 
	{
		// shoot projectile if key pressed
		if (g_TankControls[i].fireKey == key)
		{
			FireProjectile(g_Tanks[i]);
		}
	}

	switch (key)
	{
	case SDLK_1:
		--g_Tanks[0].currentHP;
		break;
	case SDLK_2:
		++g_Tanks[0].currentHP;
		break;
	case SDLK_3:
		--g_Tanks[1].currentHP;
		break;
	case SDLK_4:
		++g_Tanks[1].currentHP;
		break;
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

void InitGame()
{
	// init map
	g_pGridMap = new TileState[g_Rows * g_Cols];
	for (int i{}; i < g_Rows * g_Cols; ++i)
	{
		int row{ i / g_Cols }, col{ i % g_Cols };
		if (row == 0 || row == g_Rows - 1 || col == 0 || col == g_Cols - 1)
		{
			g_pGridMap[i] = TileState::unbreakableWall;
		}
		else 
		{
			int randomNumber{ rand() % 101 };
			if (randomNumber <= 85)
			{
				g_pGridMap[i] = TileState::empty;
			}
			else if (randomNumber <= 98)
			{
				g_pGridMap[i] = TileState::unbreakableWall;
			}
			else
			{
				g_pGridMap[i] = TileState::woodenBox;
			}
		}
	}

	for (int i{}; i < g_PlayerCount; ++i)
	{
		// textures
		if (!TextureFromFile("Resources/healthbar_" + std::to_string(i) + ".png", g_HealthBarTextures[i]))
			std::cout << "Resources/healthbar_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromFile("Resources/healthbar_filling_" + std::to_string(i) + ".png", g_HealthBarFillingTextures[i]))
			std::cout << "Resources/healthbar_filling_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromFile("Resources/tank_" + std::to_string(i) + ".png", g_Tanks[i].texture))
			std::cout << "Resources/tank_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		// init position and angle of tanks (facing to the center)
		g_Tanks[i].position = g_TankStartPositions[i];
		g_Tanks[i].angle = atan2f(g_WindowHeight / 2 - g_Tanks[i].position.y, g_WindowWidth / 2 - g_Tanks[i].position.x);

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

void DeleteGame()
{
	// delete map
	delete[] g_pGridMap;
	g_pGridMap = nullptr;

	for (int i{}; i < g_PlayerCount; ++i)
	{
		DeleteTexture(g_HealthBarTextures[i]);
		DeleteTexture(g_HealthBarFillingTextures[i]);
		DeleteTexture(g_Tanks[i].texture);
	}

	// tiles
	for (int i{}; i < int(TileState::last); i++)
	{
		if (!TextureFromFile("Resources/tile_" + std::to_string(i) + ".png", g_TileTextures[i]))
			std::cout << "Resources/tile_" + std::to_string(i) + ".png" << " could not be loaded!" << '\n';
	}

	DeleteTexture(g_HealthBarBackgroundTexture);
	DeleteTexture(g_ProjectileStandardTexture);
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

void UpdateTanks(float elapsedSec)
{
	// input for all tanks
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	for (int i{}; i < g_PlayerCount; i++)
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

		// moving
		if (pStates[g_TankControls[i].upKey])
		{
			g_Tanks[i].position.x += cosf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
			g_Tanks[i].position.y += sinf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
		}
		else if (pStates[g_TankControls[i].downKey])
		{
			g_Tanks[i].position.x -= cosf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
			g_Tanks[i].position.y -= sinf(g_Tanks[i].angle) * g_Tanks[i].speed * elapsedSec;
		}
	}

	// TODO: Collision detection

}

void UpdateProjectiles(float elapsedSec)
{
	for (int i{}; i < g_PlayerCount; ++i) 
	{
		for (int j{}; j < g_MaxProjectiles; ++j) 
		{
			if (g_Tanks[i].projectiles[j].active)
			{
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

		int row{ i / g_Cols }, column{ i % g_Cols }, tileState{ int(g_pGridMap[i]) };

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
		Rectf destinationRect{};
		destinationRect.left = g_Tanks[i].position.x - (g_Tanks[i].texture.width / 2) * g_Scaling;
		destinationRect.bottom = g_Tanks[i].position.y - (g_Tanks[i].texture.height / 2) * g_Scaling;
		destinationRect.width = g_Tanks[i].texture.width * g_Scaling;
		destinationRect.height = g_Tanks[i].texture.height * g_Scaling;
		DrawTexture(g_Tanks[i].texture, destinationRect, g_Tanks[i].angle);
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
			float hpScale = g_Tanks[i].currentHP / g_Tanks[i].maxHP,
				xOffset = 16.0f * g_Scaling, yOffset = 3.0f * g_Scaling;
			destinationHealth.left = g_HealthbarPositions[i].x + xOffset;
			destinationHealth.bottom = g_HealthbarPositions[i].y + yOffset;
			destinationHealth.width = g_HealthBarFillingTextures[i].width * g_Scaling * hpScale;
			destinationHealth.height = g_HealthBarFillingTextures[i].height * g_Scaling;
			DrawTexture(g_HealthBarFillingTextures[i], destinationHealth);
		}
		else 
		{
			g_Tanks[i].currentHP = 0;
		}
	}
}


#pragma endregion ownDefinitions