#include "pch.h"
#include "Game.h"
#include <iostream>

//Basic game functions
#pragma region gameFunctions											
void Start()
{
	//LoadTextures();
	InitTanks();
	g_pGridMap = new TileState[g_Rows * g_Cols];
}

void Draw()
{
	ClearBackground();
	DrawGrid();
	for (int i{}; i < g_PlayerCount; ++i) 
	{
		for (int j{}; j < g_MaxProjectiles; ++j) 
		{
			if (g_Tanks[i].projectiles[j].active == true)
			{
				DrawProjectiles();
			}
		}
	}
	DrawTanks();

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
	DeleteTanks();
	delete[] g_pGridMap;
	g_pGridMap = nullptr;
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
 			for (int j{}; j < g_MaxProjectiles; ++j)
			{
				if (g_Tanks[i].projectiles[j].active == false)
				{
					g_Tanks[i].projectiles[j].active = true;
					g_Tanks[i].projectiles[j].position = g_Tanks[i].position;
					g_Tanks[i].projectiles[j].angle = g_Tanks[i].angle;
					break;
				}
			}
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

void InitTanks()
{
	for (int i{}; i < g_PlayerCount; i++)
	{
		// textures
		if (!TextureFromFile("Resources/healthbar_" + std::to_string(i) + ".png", g_HealthBarTextures[i]))
			std::cout << "Resources/healthbar_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromFile("Resources/healthbar_filling_" + std::to_string(i) + ".png", g_HealthBarFillingTextures[i]))
			std::cout << "Resources/healthbar_filling_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		if (!TextureFromFile("Resources/tank_" + std::to_string(i) + ".png", g_Tanks[i].texture))
			std::cout << "Resources/tank_" + std::to_string(i) + ".png " << "could not be loaded!" << '\n';

		// attributes
		g_Tanks[i].position = g_TankStartPositions[i];
		g_Tanks[i].angle = 0.0f;
		g_Tanks[i].currentHP = g_TankHP;
		g_Tanks[i].maxHP = g_TankHP;
		g_Tanks[i].speed = g_TankSpeed;
		g_Tanks[i].turnSpeed = g_TankTurnSpeed;
	}

	TextureFromFile("Resources/tile_0.png", g_WoodTexture);

	// single textures
	if (!TextureFromFile("Resources/healthbar_background.png", g_HealthBarBackgroundTexture))
		std::cout << "Resources/healthbar_background.png " << "could not be loaded!" << '\n';

	if (!TextureFromFile("Resources/projectile_0.png", g_ProjectileStandardTexture))
		std::cout << "Resources/projectile_0.png " << "could not be loaded!" << '\n';
}

void DeleteTanks()
{
	for (int i{}; i < g_PlayerCount; i++)
	{
		DeleteTexture(g_HealthBarTextures[i]);
		DeleteTexture(g_HealthBarFillingTextures[i]);
		DeleteTexture(g_Tanks[i].texture);
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
				g_Tanks[i].projectiles[j].position.x += cosf(g_Tanks[i].projectiles[j].angle) * g_Tanks[i].projectiles[j].speed * elapsedSec;
				g_Tanks[i].projectiles[j].position.y += sinf(g_Tanks[i].projectiles[j].angle) * g_Tanks[i].projectiles[j].speed * elapsedSec;


				if (g_Tanks[i].projectiles[j].position.x >= g_WindowWidth || g_Tanks[i].projectiles[j].position.y >= g_WindowHeight || g_Tanks[i].projectiles[j].position.x <= 0 || g_Tanks[i].projectiles[j].position.y <= 0)
				{
					g_Tanks[i].projectiles[j].active = false;
				}
			}
		}
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
			if (g_Tanks[i].projectiles[j].active) 
			{
				Rectf destinationProjectile{};
				const float offset{ 10.0f };
				destinationProjectile.left = (g_Tanks[i].projectiles[j].position.x + cosf(g_Tanks[i].angle) * offset) - (g_ProjectileStandardTexture.width / 2) * g_Scaling;
				destinationProjectile.bottom = (g_Tanks[i].projectiles[j].position.y + sinf(g_Tanks[i].angle) * offset) - (g_ProjectileStandardTexture.height / 2) * g_Scaling;
				destinationProjectile.width = g_ProjectileStandardTexture.width * g_Scaling;
				destinationProjectile.height = g_ProjectileStandardTexture.height * g_Scaling;
				DrawTexture(g_ProjectileStandardTexture, destinationProjectile, g_Tanks[i].projectiles[j].angle);
			}
		}
	}
}

void DrawHealthBars() 
{
	for (int i{}; i < g_PlayerCount; i++)
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

void DrawGrid()
{
	for (int i{}; i < g_Rows * g_Cols; ++i) 
	{
		Rectf destinationRect{};

		destinationRect.left = ((i % g_Cols) * g_CellSize) * g_Scaling;
		destinationRect.bottom = ((i / g_Cols) * g_CellSize) * g_Scaling;
		destinationRect.width = g_WoodTexture.width * g_Scaling;
		destinationRect.height = g_WoodTexture.height * g_Scaling;
		DrawTexture(g_WoodTexture, destinationRect);
	}
}
#pragma endregion ownDefinitions