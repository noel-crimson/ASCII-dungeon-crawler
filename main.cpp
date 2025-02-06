#include <stdlib.h>

#include <cmath>

#include <conio.h>
#include <ctime>
#include <cctype>
#include <map>
#include <queue>
#include <fstream>

#include "random.h"
#include "display.h"
#include "entities.h"
#include "customOperations.h"
#include "mapGeneration.h"

using namespace std;

struct Rat : Entity
{
	Rat() : Entity(2, 0, 2, 1, "r", "Rat") {};
};

struct Dog : Entity
{
	Dog() : Entity(4, 2, 1, 2, "h", "Hound") {};
};

struct Skeleton : Entity
{
	Skeleton() : Entity(5, 1, 3, 2, "o", "Skeleton") {};
};

struct Ghoul : Entity
{
	Ghoul() : Entity(15, 3, 4, 4, "Y", "Ghoul") {};
};

bool inLOS(Point viewer, Point tile, vector<vector<char>>& dungeon)
{
	//broken!!
	//jank!!!
	bool test1 = true;
	double losX = viewer.x;
	double losY = viewer.y;
	double xDir;
	double slope;
	if (tile.x == viewer.x) //check vertical lines
	{
		xDir = 0.5;
		if (tile.y > viewer.y)
		{
			slope = 1;
		}
		else
		{
			slope = -1;
		}
	}
	else //checking diagonals
	{
		slope = (tile.y - viewer.y) / (double)(tile.x - viewer.x);

		if (tile.x > viewer.x)
		{
			xDir = 0.2;
		}
		else
		{
			xDir = -0.2;
		}
	}

	Point p = viewer;
	while (p.x != tile.x || p.y != tile.y)
	{
		if (p.x != tile.x)
		{
			losX+= xDir;
			p.x = round(losX);
			//cout << "losX " << losX << " p.x " << p.x << " tile x " << tile.x << " xDir " << xDir << endl;
		}
		if (p.y != tile.y)
		{
			//if (p.x != tile.x)
			//{
				losY += slope * xDir;
			//}
			//else
			//{
				//losY += slope * 0.1 * xDir;
			//}
			p.y = round(losY);
			//cout << "losY " << losY << " p.y " << p.y << " tile y " << tile.y << " slope " << slope << endl;
		}
		

		if (p.x != tile.x || p.y != tile.y)
		{
			if (!isClearOfWalls(p, dungeon))
			{
			    //cout << "collision!";
				test1 = false;
				break;
			}
		}
	}

	return test1; 
}

vector<vector<bool>> generateLOSMaskRaycast(vector<vector<char>>& dungeon, int width, int height, Point p, int camX, int camY)
{
	
	vector<vector<bool>> LOSMask(10, vector<bool>(20, false));

	double x = p.x;
	double y = p.y;

	double dx = 1;
	double dy = 0;

	
	for (double dx = 1; dx >= -1; dx -= 0.0078125)
	{
		x = p.x;
		y = p.y;
		dy = sqrt(1 - (dx * dx));
		while ((floor(y) - camY)< 10 && (floor(x) - camX) < 20 && (floor(y) - camY) >= 0 && (floor(x) - camX) >= 0)
		{
			//cout << "1 dx " << dx << " dy " << dy << "[" << to_string(floor(y) - camY) << "][" << to_string(floor(x) - camX) << "]" << endl;
			LOSMask[floor(y) - camY][floor(x) - camX] = true;
			if (floor(y) == p.y && floor(x) == p.x)
			{
				LOSMask[floor(y) - camY][floor(x) - camX] = false;
			}
			if (!isClearOfWalls(Point(x, y), dungeon))
			{
				break;
			}
			x += dx;
			y += dy;
			
		}
	}
	
	for (double dx = -1; dx <= 1; dx += 0.0078125)
	{
		x = p.x;
		y = p.y;
		dy = -sqrt(1 - (dx * dx));
		while ((floor(y) - camY) < 10 && (floor(x) - camX) < 20 && (floor(y) - camY) >= 0 && (floor(x) - camX) >= 0)
		{
			LOSMask[floor(y) - camY][floor(x) - camX] = true;
			if (floor(y) == p.y && floor(x) == p.x)
			{
				LOSMask[floor(y) - camY][floor(x) - camX] = false;
			}
			if (!isClearOfWalls(Point(x, y), dungeon))
			{
				break;
			}
			x += dx;
			y += dy;
			//cout << "2 dx " << dx << " dy " << dy << "[" << to_string(floor(y) - camY) << "][" << to_string(floor(x) - camX) << "]" << endl;
		}
	}
	

	/*
	for (int i = 0; i < 10; i++)
	{
		cout << " ";
		for (int j = 0; j < 20; j++)
		{
			if (LOSMask[i][j])
			{
				cout << "1";
			}
			else
			{
				cout << "0";
			}
		}
		cout << endl;
	}
	*/
	//cout << endl;
	return LOSMask;
}

void gameTick(vector<mapEntity>& entities, vector<vector<char>>& dungeon, Player p, int camX, int camY, Log& log, int& combatFlag)
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i].graphic == 'M')
		{
			if (entities[i].isInView(camX, camY))
			{
				entities[i].isActive = true;
				//log.push("In view! " + entities[i].printPosition() + " vs camx " + to_string(camX) + " camY " + to_string(camY));
			}
			if (entities[i].isActive)
			{
				vector<vector<int>> validMoves = {
					{-1, -1}, {0, -1}, {1, -1},
					{-1, 0}, {1, 0},
					{-1, 1}, {0, 1}, {1, 1}
				};
				vector<int> movement = randomElementPop<vector<int>>(validMoves);
				while (!entities[i].canMove(movement, dungeon, entities) && validMoves.size() > 0)
				{
					movement = randomElementPop<vector<int>>(validMoves);
				}
				if (entities[i].canMove(movement, dungeon, entities))
				{
					entities[i].move(movement);
				}
				if (entities[i].x == p.x && entities[i].y == p.y)
				{
					combatFlag = i;
				}
			}
		}
	}
};

int main()
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	srand(time(NULL));
	int gold = 0;
	int combatFlag = -1;

	int floorWidth = 400;
	int floorHeight = 400;

	vector<vector<char>> dungeon(floorHeight, vector<char>(floorWidth, '#'));
	vector<mapEntity> entities = {};
	Log travelLog;

	vector<vector<Entity>> encounterPool1 =
	{
		{Skeleton(), Rat(), Rat()}, // 4
		{Skeleton(), Skeleton()}, // 4
		{Dog(), Dog()}, //4
		{Skeleton(), Dog()}, //4
		{Dog(), Rat(), Rat()}, //4
	};

	vector<vector<Entity>> encounterPool = encounterPool1; //this way it can be switched per floor

	vector <Room> allRooms = {};
	int roomCount = 12;
	
	//int maxRoomDistance = 16;

	int generationRadius = 20;

	vector<const char*> roomPool = { "room1.png", "room2.png", "room3.png", "room4.png", "room5.png", "room6.png", "room7.png", "room8.png", 
	"room9.png", "room10.png", "room11.png", "room12.png" };
	const char* roomFile = randomElement<const char*>(roomPool);

	while (allRooms.size() < roomCount)
	{
		int roomWidth;
		int roomHeight;
		bool validRoom = true;
		stbi_load(roomFile, &roomWidth, &roomHeight, 0, 0);
		//cout << roomWidth << " " << roomHeight << endl;
		Room newRoom(randomRange(1, min(generationRadius, floorWidth-roomWidth-1)), randomRange(1, min(generationRadius, floorHeight-roomHeight-1)), roomFile);
		for (int i = 0; i < allRooms.size(); i++)
		{
			if (newRoom.isOverlapping(allRooms[i]))
			{
				validRoom = false;
				generationRadius++;
				//cout << generationRadius << endl;
			}
			//else if (!newRoom.isWithinDistance(allRooms[i], maxRoomDistance))
			//{
				//validRoom = false;
				//maxRoomDistance++;6
			//}
		}
		if (validRoom)
		{
			allRooms.push_back(newRoom);
			dungeonGenerationFromImage(dungeon, entities, encounterPool, roomFile, newRoom.x, newRoom.y);
			roomFile = randomElement<const char*>(roomPool);
		}
	}
	//dungeon corridor generation

	for (int i = 0; i < allRooms.size(); i++)
	{
		allRooms[i].generateCorridorAnchor(dungeon); //generate corridor anchors for all rooms
		//cout << "Generated at " << allRooms[i].corridorAnchor.x << " " << allRooms[i].corridorAnchor.y << endl;
	}
	Node* roomTreeRoot = generateRoomTree(allRooms); //build a room tree
	roomTreeRoot->burnCorridors(dungeon); //place corridor tiles

	Room firstRoom = allRooms[0];

	Player p = Player(Point(firstRoom.x + firstRoom.width/2, firstRoom.y + firstRoom.height/2));

	int cameraX = max(0, p.x-10);
	int cameraY = max(0, p.y-5); //camera is placed at top left corner

	vector<int> movement = { 0, 0 };

	vector<Entity> minions = { Rat(), Rat(), Rat() };

	//entities.push_back(wanderingMonster(p.x+2, p.y+2, encounter1));

	//vector<mapEntity> entities = {potionPickup(25, 10), wanderingMonster(8,8)};

	//populate dungeon with monsters

	for (int i = 1; i < (roomCount / 2); i++)
	{
		//cout << "New mob" << endl;
		Room targetRoom = allRooms[randomRange(1, roomCount-1)];
		vector<Entity> randomEncounter = randomElement<vector<Entity>>(encounterPool);
		entities.push_back(wanderingMonster(findEmptyPointInRoom(targetRoom, dungeon), randomEncounter));

	}

	//drawing the dungeon to txt

	ofstream mapFile("map.txt");
	for (int i = 0; i < generationRadius+20; i++)
	{
		for (int j = 0; j < generationRadius+20; j++)
		{
			mapFile << dungeon[i][j];
		}
		mapFile << endl;
	}
	mapFile.close();

	//main loop

	while (p.sp > 0)
	{
		//cout << p.x << ", " << p.y << endl;
		drawBoard(dungeon, dungeon[0].size(), dungeon.size(), p, cameraX, cameraY, entities);
		travelLog.printLog();
		switch (_getch())
		{
		case '1':
			movement = { -1, 1 };
			break;
		case '2':
			movement = { 0, 1 };
			break;
		case '3':
			movement = { 1, 1 };
			break;
		case '4':
			movement = { -1, 0 };
			break;
		case '6':
			movement = { 1, 0 };
			break;
		case '7':
			movement = { -1, -1 };
			break;
		case '8':
			movement = { 0, -1 };
			break;
		case '9':
			movement = { 1, -1 };
			break;
		default:
			movement = { 0, 0 };
			break;
		}

		switch (dungeon[p.y + movement[1]][p.x + movement[0]])
		{
		case '#':
			travelLog.push("You bump into a wall.");
			break;
		default:
			p.move(movement, entities, travelLog, combatFlag);
			if (combatFlag == -1)
			{
				gameTick(entities, dungeon, p, cameraX, cameraY, travelLog, combatFlag);
			}
			if (combatFlag != -1)
			{
				travelLog.push("Encounter!");
				//vector<Entity> encounter = randomElement<vector<Entity>>(encounterPool);
				//combat(encounter, this);
				//bool allEnemiesDead = combat(entities[combatFlag].encounter, p, minions);
				//if (allEnemiesDead)
				if (1)
				{
					entities.erase(entities.begin() + combatFlag);
					//travelLog.push("Defeated the enemy horde!");
					travelLog.push("Placeholder for combat. Combat finished.");
				}
				else
				{
					if (p.sp > 0)
					{
						p.x -= movement[0];
						p.y -= movement[1];
						travelLog.push("Ran away from the enemies...!");
					}	
				}
				combatFlag = -1; 
				clearScreen();
			}
			
			break;
		}
		
		if (p.x >= (cameraX + 15))
		{
			cameraX = p.x - 15;
		}

		if (p.x <= (cameraX + 5 ))
		{
			cameraX = max(0, p.x - 5);
		}
		
		if (p.y >= (cameraY + 6))
		{
			cameraY = p.y - 6;
		}

		if (p.y <= (cameraY + 3))
		{
			cameraY = max(0, p.y - 3);
		}

		//system("cls");
		clearScreen();
		//setCursorPosition(0,0);
	}
	return 0;
}