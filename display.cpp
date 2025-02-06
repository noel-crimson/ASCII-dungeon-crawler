#define STB_IMAGE_IMPLEMENTATION
#include "display.h"

void setCursorPosition(int x, int y)
{
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

std::vector<std::string> SCREEN_BUFFER = {};

void clearScreen(int clearWidth)
{
	setCursorPosition(0, 0);
	for (int i = 0; i < 40; i++)
	{
		std::string emptyLine(clearWidth, ' ');
		std::cout << emptyLine << std::endl;
	}
	setCursorPosition(0, 0);
}

void printBuffer()
{
	setCursorPosition(0, 0);
	for (int i = 0; i < SCREEN_BUFFER.size(); i++)
	{
		std::cout << SCREEN_BUFFER[i];
		std::cout << "                              " << std::endl;
	}
	for (int i = 0; i < 30; i++)
	{
		std::cout << "                                            " << std::endl;
	}
	SCREEN_BUFFER = {};
	setCursorPosition(0, 0);
}

void drawBoard(std::vector<std::vector<char>>& dungeon, int width, int height, Player p, int camX, int camY, std::vector<mapEntity> ent)
{
	//std::vector<std::vector<bool>> visibleSquares = generateLOSMaskRaycast(dungeon, width, height, Point(p.x, p.y), camX, camY);
	std::cout << "+--------------------+" << std::endl;
	for (int i = camY; i < height && i < camY + 10; i++)
	{
		std::cout << "|";
		for (int j = camX; j < width && j < camX + 20; j++)
		{
			if (i == p.y && j == p.x)
			{
				std::cout << "@";
			}
			else
			{
				if (false)//!visibleSquares[i-camY][j-camX])
				{
					std::cout << " ";
				}
				else
				{
					bool occupiedByEntity = false;
					for (int k = 0; k < ent.size(); k++)
					{
						if (i == ent[k].y && j == ent[k].x)
						{
							occupiedByEntity = true;
							std::cout << ent[k].graphic;
						}
					}
					if (!occupiedByEntity)
					{
						std::cout << dungeon[i][j];
					}
				}
			}
		}
		std::cout << "|";
		std::cout << std::endl;
	}
	std::cout << "+--------------------+" << std::endl;
}

void dungeonGenerationFromImage(std::vector<std::vector<char>>& dungeon, std::vector<mapEntity>& entities, std::vector<std::vector<Entity>> encounterPool, const char* imageFile, int xOrigin, int yOrigin)
{
	int width, height, nrChannels;
	unsigned char* mapImage = stbi_load(imageFile, &width, &height, &nrChannels, 0);
	//cout << "width " << width << endl;
	//cout << "height " << height << endl;

	//int row = 0;
	for (int i = yOrigin; i < height + yOrigin; i++)
	{
		for (int j = xOrigin; j < width + xOrigin; j++)
		{
			int index = (j - xOrigin) * 3 + (i - yOrigin) * 3 * width;
			//cout << mapImage[index] << " " << mapImage[index + 1] << " " << mapImage[index + 2] << endl;
			if (!(mapImage[index] == 0 && mapImage[index + 1] == 0 && mapImage[index + 2] == 0)) //not black
			{
				dungeon[i][j] = '.';
			}
			if (mapImage[index] == 255 && mapImage[index + 1] == 0 && mapImage[index + 2] == 255) //pink
			{
				entities.push_back(potionPickup(Point(j, i)));
			}
			if (mapImage[index] == 255 && mapImage[index + 1] == 0 && mapImage[index + 2] == 0) //red
			{
				entities.push_back(wanderingMonster(Point(j, i), encounterPool[rand() % encounterPool.size()]));
			}
		}
		//row++;
	}
	//cout << "Width is " << width << " height is " << height << " channels are " << nrChannels << endl;
	stbi_image_free(mapImage);
};
