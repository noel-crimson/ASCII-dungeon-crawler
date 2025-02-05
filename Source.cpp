#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <random>
#include <ctime>
#include <cctype>
#include <map>
#include <queue>
#include <fstream>

#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

const int VIEWPORTX = 20;
const int VIEWPORTY = 10;

struct Entity;

struct Player;

struct mapEntity;

struct Room;

struct Point;

bool isClear(Point p, vector<vector<char>>& dungeon, vector<mapEntity>& entities);

struct Point
{
	int x;
	int y;
	Point(int x0, int y0)
	{
		x = x0;
		y = y0; 
	}
	string printPoint()
	{
		return ("(" + to_string(x) + ", " + to_string(y) + ")");
	}
};

Point findEmptyPointInRoom(Room r, vector<vector<char>> dungeon);

void connectPoints(vector<vector<char>>& dungeon, Point p1, Point p2);

//bool combat(vector<Entity>& enemies, Player* p);

//void gameTick(vector<mapEntity>& entities, vector<vector<char>>& dungeon, Player p);

void setCursorPosition(int x, int y)
{
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

vector<std::string> SCREEN_BUFFER = {};

void clearScreen(int clearWidth = 30)
{
	setCursorPosition(0, 0);
	for (int i = 0; i < 40; i++)
	{
		string emptyLine(clearWidth, ' ');
		cout << emptyLine << endl;
	}
	setCursorPosition(0, 0);
}

void printBuffer()
{
	setCursorPosition(0, 0);
	for (int i = 0; i < SCREEN_BUFFER.size(); i++)
	{
		cout << SCREEN_BUFFER[i];
		cout << "                              " << endl;
	}
	for (int i = 0; i < 30; i++)
	{
		cout << "                                            " << endl;
	}
	SCREEN_BUFFER = {};
	setCursorPosition(0, 0);
}

int rollD(int dice, int bonus = 0)
{
	int result = rand() % dice + 1 + bonus;
	//cout << "d" << dice << " = " << result << endl;
	return result;
}

int randomRange(int min, int max)
{
	if (min > max)
	{
		int temp = max;
		min = max;
		max = temp;
	}
	int result = min + (rand() % (max-min+1));
	//cout << "d" << dice << " = " << result << endl;
	return result;
}

string lowercaseString(string str)
{
	for (int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}
	//cout << "To lowercase: " << str << endl;
	return str;
}

template<typename T> T randomElement(vector<T>& arr)
{
	return arr[rollD(arr.size(),-1)];
}

template<typename T> T randomElementPop(vector<T>& arr)
{
	int id = rollD(arr.size(), -1);
	T returnValue = arr[id];
	arr.erase(arr.begin() + id);
	return returnValue;
}

int distanceBetween(int x1, int y1, int x2, int y2)
{
	return floor(sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2)));
}

struct Log
{
	vector<string> messages = {};
	Log() {};
	void push(string msg, bool instantPrint = true)
	{
		//string msg = ssmsg.str();
		if (instantPrint)
		{
			cout << msg << endl;
		}
		
		messages.push_back(msg);
		if (messages.size() > 13)
		{
			messages.erase(messages.begin());
		}
		//ssmsg.str("");
	};
	void printLog() 
	{
		for (int i = 0; i < messages.size(); i++)
		{
			cout << messages[i] << endl;
		}
	}
};

struct Entity
{
	int hp=1;
	int hpMax=1;
	int pow=0;
	int dmgDie = 0;
	int soul = 0;
	bool stunned = false;
	string graphic = "?";
	string name = "Bob";
	Entity() : hpMax(1), hp(1), pow(1), dmgDie(0), soul(1), graphic("?"), name("NaN") {};

	Entity(int hpMaxVal, int powVal, int dmgDieVal, int soulVal, string graphicVal="?", string nameVal="Bob")
	: hpMax(hpMaxVal), hp(hpMaxVal), pow(powVal), dmgDie(dmgDieVal), soul(soulVal)
	{
		graphic = graphicVal;
		name = nameVal;
	}

	int dmgRoll()
	{
		return rollD(dmgDie, pow);
	}
	string printHP()
	{
		stringstream result;
		result << "(HP: " << hp << "/" << hpMax << ")";
		return result.str();
	}

};

//hp, pow, dmgDie, souls, graphic, name

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

struct mapEntity
{
	vector<Entity> encounter = {};
	int x = 0;
	int y = 0;
	char graphic = '?';
	int isActive = false;
	mapEntity(Point p, char graphicValue = '?') : x(p.x), y(p.y), graphic(graphicValue) {};
	void move(vector<int> movement)
	{
		x += movement[0];
		y += movement[1];
	}
	string printPosition()
	{
		return "(" + to_string(x) + ", " + to_string(y) + ")";
	}
	bool canMove(vector<int> movement, vector<vector<char>>& map, vector<mapEntity>& entities)
	{
		/*
		bool returnValue = true;
		if ((map[y + movement[1]][x + movement[0]] == '#'))
		{
			returnValue = false;
		}
		for (mapEntity e : entities)
		{
			if (y + movement[1] == e.y && x + movement[0] == e.x) //check against every entity it could collide with
			{
				returnValue = false;
			}
		}*/
		
		return isClear(Point(x + movement[0], y + movement[1]), map, entities);
		//return returnValue;
	}
	bool overlapsWall(vector<vector<char>>& map)
	{
		return (map[y][x] == '#');
	}
	bool isInView(int camX, int camY)
	{
		return ((x < (VIEWPORTX + camX)) && (x>=camX) && (y < (VIEWPORTY + camY)) && (y >= camY));
	}
};

bool isClearOfWalls(Point p, vector<vector<char>>& dungeon)
{
	bool returnValue = true;
	if ((dungeon[p.y][p.x] == '#'))
	{
		returnValue = false;
	}
	return returnValue;
}

bool isClear(Point p, vector<vector<char>>& dungeon, vector<mapEntity>& entities)
{
	bool returnValue = true;
	if ((dungeon[p.y][p.x] == '#'))
	{
		returnValue = false;
	}
	for (mapEntity e : entities)
	{
		if (p.y == e.y && p.x == e.x) //check against every entity it could collide with
		{
			returnValue = false;
		}
	}
	return returnValue;
}

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

struct Player : Entity, mapEntity
{
	int sp = 1;
	int spMax = 1;
	int level = 1;
	int xpNext = 1;
	int dregs = 0;
	int gold = 0;
	int control = 10;
	//map<string, Entity> summonPool;
	vector<Entity> summonPool;
	Player(Point p) : mapEntity(p)
	{
		name = "Noel";
		sp = 30;
		spMax = 30;
		dregs = 0;
		pow = 0;
		dmgDie = 4;
		gold = 0;
		control = 10;
		//summonPool = { {"rat", Rat()}};
		summonPool = { Rat() };
	}
	void move(vector<int> movement, vector<mapEntity>& mapEntities, Log& log, int& combatFlag)
	{
		x += movement[0];
		y += movement[1];
		for (int k = 0; k < mapEntities.size(); k++)
		{
			if (x == mapEntities[k].x && y == mapEntities[k].y)
			{
				switch (mapEntities[k].graphic)
				{
				case 'p': //potion
					log.push("Got a potion!");
					mapEntities.erase(mapEntities.begin() + k);
					break;
				case 'M': //encounter
					combatFlag = k;
					break;
				}
				break; //leaves for loop
			}
		}
	}
	string printSP()
	{
		stringstream result;
		result << "(SP: " << sp << "/" << spMax << ")";
		return result.str();
	}
};

struct itemPickup : mapEntity
{
	itemPickup(Point p, char graphicValue = '!') : mapEntity(p, graphicValue) {}
};

struct potionPickup : itemPickup
{
	potionPickup(Point p) : itemPickup(p, 'p') {};
};

struct wanderingMonster : mapEntity
{
	bool isActive = false;
	wanderingMonster(Point p, vector<Entity> e) : mapEntity(p, 'M') 
	{
		encounter = e;
		isActive = false;
	}
};

struct Room
{
	int x=1;
	int y=1;
	int width=1;
	int height=1;
	Point corridorAnchor = Point(0, 0);
	Room(int x0, int y0, const char* roomFile) : x(x0), y(y0)
	{
		//int width, height, nrChannels;
		//unsigned char* mapImage = stbi_load("room1.png", &width, &height, &nrChannels, 0);
		stbi_load(roomFile, &width, &height, 0, 0);
		//points between which corridors are connected (per room, to minimize adjacent corridors) but it's not guaranteed to be a free square
	}
	Room() {}
	void generateCorridorAnchor(vector<vector<char>> dungeon)
	{
		corridorAnchor = findEmptyPointInRoom(*this, dungeon);
	}
	bool isOverlapping(Room r2)
	{
		/*
		cout << x << " " << y << " " << r2.x << " " << r2.y << endl;
		cout << (x + width - 1) << " " << (r2.x + 1) << endl;
		cout << (r2.x + r2.width - 1) << " " << (x + 1) << endl;
		cout << (y + height - 1) << " " << (r2.y + 1) << endl;
		cout << (r2.y + r2.height - 1) << " " << (y + 1) << endl;
		*/
		return !(
				(x + width) < (r2.x) ||  // rect1 is left of rect2
				(r2.x + r2.width) < (x) ||  // rect2 is left of rect1
				(y + height) < (r2.y) || // rect1 is above rect2
				(r2.y + r2.height) < (y)
				);  // rect2 is above rect1
	}
	bool isWithinDistance(Room r2, int dist)
	{
		return ((abs(x - r2.x) + abs(y - r2.y)) < dist);
	}
	Point randomPoint()
	{
		return Point(randomRange(x+1, x + width-1), randomRange(y+1, y + height-1));
	}
};

//hpMax, powl, dmgDie, xp, gold, graphic, name

Entity summonMenu(Player& p)
{
	Entity dummy;
	string input;
	cout << "By the avatar of " << p.name << "... " << p.printSP() << endl;

	for (int i = 0; i < p.summonPool.size(); i++)
	{
		cout << p.summonPool[i].name << " (Cost: " << p.summonPool[i].soul * 3 << " SP)" << endl;
	}

	cout << "Summon by name, or CANCEL:";
	sleep_for(milliseconds(200));
	while (true)
	{
		cin >> input;

		if (input == "cancel")
		{
			return dummy;
		}
		for (int i = 0; i < p.summonPool.size(); i++)
		{
			cout << p.summonPool[i].name << " ";
			if (lowercaseString(input) == lowercaseString(p.summonPool[i].name))
			{
				if (p.summonPool[i].soul * 3 >= p.sp)
				{
					cout << "Insufficient SP to summon " << p.summonPool[i].name << " servant!" << endl;
				}
				else
				{
					p.sp -= p.summonPool[i].soul * 3;
					return Entity(p.summonPool[i]);
				}
			}
		}
		cout << endl;

	}
}

void orderRooms(vector<Room>& r)
{
	int nextNearest;
	int lowestDistance;
	Room temp;
	for (int i = 1; i < r.size(); i++)
	{
		nextNearest = i;
		lowestDistance = distanceBetween(r[i - 1].corridorAnchor.x, r[i - 1].corridorAnchor.y, r[nextNearest].corridorAnchor.x, r[nextNearest].corridorAnchor.y);
		for (int j = i + 1; j < r.size(); j++)
		{
			if (distanceBetween(r[i - 1].corridorAnchor.x, r[i - 1].corridorAnchor.y, r[j].corridorAnchor.x, r[j].corridorAnchor.y) < lowestDistance)
			{
				nextNearest = j;
				lowestDistance = distanceBetween(r[i - 1].corridorAnchor.x, r[i - 1].corridorAnchor.y, r[j].corridorAnchor.x, r[j].corridorAnchor.y);
				
			}
			//cout << i << " " << j << " " << lowestDistance << endl;
		}
		//swap r[i] and  r[nexNearest]
		temp = r[i];
		r[i] = r[nextNearest];
		r[nextNearest] = temp;
	}
}

struct Node
{
	Node* parent;
	vector<Node*> branches;
	Room r;
	static int totalNodes;
	int nodeID;
	Node(Room r0) : r(r0), parent(nullptr) 
	{ 
		nodeID = totalNodes;
		totalNodes++;
	}

	void insert(Room r0)
	{
		Node* newNode = new Node(r0);
		newNode->parent = this;
		branches.push_back(newNode);
	}

	void iterateTree()
	{
		queue<Node*> q;
		q.push(this);

		while (!q.empty())
		{
			Node* current = q.front();
			q.pop();

			cout << current->r.x << " " << current->r.y << " ";

			for (Node* child : current->branches)
			{
				q.push(child);
			}
		}
	}

	void addToNearestRoom(Room newRoom)
	{
	
		int distance = 0;

		//get a pointer to the closest node on the tree
		queue<Node*> q;
		q.push(this);
		int shortestDistance = abs(q.front()->r.corridorAnchor.x - newRoom.corridorAnchor.x) + abs(q.front()->r.corridorAnchor.y - newRoom.corridorAnchor.y);
		Node* nearestRoomPointer = q.front();
		while (!q.empty())
		{
			Node* current = q.front();
			q.pop();
			//modify each node here
			//find closest room 
			distance = abs(current->r.corridorAnchor.x - newRoom.corridorAnchor.x) + abs(current->r.corridorAnchor.y - newRoom.corridorAnchor.y);
			//cout << "Distance " << current << " to new room: " << distance << endl;
			if (distance < shortestDistance)
			{
				shortestDistance = distance;
				nearestRoomPointer = current;
				//cout << "New shortest at address " << nearestRoomPointer << endl;
			}
			
			for (Node* child : current->branches)
			{
				q.push(child);
			}
		}
		//cout << "Connected to " << nearestRoomPointer->nodeID << " at " << nearestRoomPointer << endl;
		nearestRoomPointer->insert(newRoom);
	}
	
	void burnCorridors(vector<vector<char>>& dungeon)
	{
		queue<Node*> q;
		q.push(this);

		while (!q.empty())
		{
			Node* current = q.front();
			q.pop();

			for (Node* child : current->branches)
			{
				//connectPoints(dungeon, findEmptyPointInRoom(current->r, dungeon), findEmptyPointInRoom(child->r, dungeon));
				//cout << "Drilling from " << current->r.corridorAnchor.x << " " << current->r.corridorAnchor.y << endl;
				connectPoints(dungeon, current->r.corridorAnchor, child->r.corridorAnchor);
			}

			for (Node* child : current->branches)
			{
				q.push(child);
			}
		}
	}

};

int Node::totalNodes = 0;

//generation algorithm
//order rooms in vector such that adjacent rooms are the closest to each other they can get
//pass room to tree, it gets attached to the nearest room in the tree


Node* generateRoomTree(vector<Room>& r) 
{
	orderRooms(r);
	Node* root = new Node(r[0]);
	//r.erase(r.begin());
	for (int i = 1; i < r.size(); i++)
	{
		//cout << "Room " << i << endl;
		root->addToNearestRoom(r[i]); 
	}
	return root;
}

void connectPoints(vector<vector<char>>& dungeon, Point p1, Point p2)
{
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;

	if (rollD(2) == 1) // horizontal first
	{
		while (x1 != x2 || y1 != y2)
		{
			dungeon[y1][x1] = '.';
			if (abs(y2 - y1) == 1)
			{
				y1 = y2;
			}
			else if (x2 > x1)
			{
				x1++;
			}
			else if (x2 < x1)
			{
				x1--;
			}
			else if (y2 > y1)
			{
				y1++;
			}
			else if (y2 < y1)
			{
				y1--;
			}
		}
	}
	else
	{
		while (x1 != x2 || y1 != y2)
		{
			dungeon[y1][x1] = '.';
			if (abs(x2 - x1) == 1)
			{
				x1 = x2;
			}
			else if (y2 > y1)
			{
				y1++;
			}
			else if (y2 < y1)
			{
				y1--;
			}
			else if (x2 > x1)
			{
				x1++;
			}
			else if (x2 < x1)
			{
				x1--;
			}
		}
	}
}

Point findEmptyPointInRoom(Room r, vector<vector<char>> dungeon)
{
	Point p = r.randomPoint();
	while (dungeon[p.y][p.x] == '#')
	{
		//gets stuck here sometimes
		//cout << "Here!! " << p.printPoint() << endl;
		p = r.randomPoint();
	}
	return p;
}

bool combat(vector<Entity> enemies, Player& p, vector<Entity>& minions)
{
	clearScreen();
	//setCursorPosition(0, 0);
	int roundCount = 1;
	int target = 0;
	int deadEnemies = 0;
	int totalEnemies = enemies.size();
	bool focusedAttack = false;
	bool randomAttack = false;
	bool spellCooldown = false;
	bool finalNecromancy = false;
	int reward = 0;
	for (int i = 0; i < enemies.size(); i++)
	{
		reward += enemies[i].soul;
	}
	Log combatLog;
	string turn = "player";
	combatLog.push("BEGIN", false);
	while (p.sp > 0)
	{
		while (turn == "player")
		{
			clearScreen();
			cout << "= = = = ROUND " << to_string(roundCount) << " = = = =" << endl;
			cout << " = = FOE " << endl << endl;
			for (int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i].hp > 0)
				{
					cout << "   " << enemies[i].graphic;
				}
				else
				{
					cout << "   " << "X";
				}
			}
			cout << endl << "- - - - - - - - - - -" << endl;
			for (int i = 0; i < enemies.size(); i++)
			{
				cout << "   ";
				if (target == i)
				{
					cout << "^";
				}
				else
				{
					cout << " ";
				}
			}
			cout << endl;
			for (int i = 0; i < minions.size(); i++)
			{
				cout << "   " << minions[i].graphic;
			}
			cout << endl;
			cout << "          LEGION = = " << endl;

			cout << "TARGET [1-" << enemies.size() << "]: " << enemies[target].name << " " << enemies[target].printHP() << endl;
			cout << p.name << " " << p.printSP() << endl;
			if (minions.size() > 0 && !finalNecromancy)
			{
				cout << "(A)ttack Indiscriminately" << endl;
			}
			else if (enemies[target].hp > 0)
			{
				cout << "(A)ttack in Desperation" << endl;
			}

			if (!finalNecromancy)
			{
				cout << "(S)ummon the Dead" << endl;
			}

			if (enemies[target].hp > 0 && minions.size() > 0)
			{
				cout << "(B)reak This One (1SP)" << endl;
			}

			if (enemies[target].hp > 0)
			{
				cout << "(D)ebilitating Force (" << floor(p.spMax * 0.2) << " SP)" << endl;
			}
			else
			{
				cout << "(R)essurect the Dead" << endl;
				cout << "(C)onsume the Dead" << endl;
			}
			if (roundCount == 1)
			{
				cout << "(F)lee Unnoticed (5SP)" << endl;
			}
			cout << "..." << endl;

			combatLog.printLog();

			switch (_getch())
			{
				case '1':
					target = 0;
					break;
				case '2':
					target = 1;
					break;
				case '3':
					target = 2;
					break;
				case '4':
					target = 3;
					break;
				case 'a':
					if (minions.size() > 0)
					{
						combatLog.push("The legion attacks indiscriminately!");
						randomAttack = true;
					}
					else
					{
						if (enemies[target].hp == 0)
						{
							combatLog.push("Target already dead!");
						}
						else
						{
							combatLog.push("The avatar swings at " + enemies[target].name + " in desperation!");
							focusedAttack = true;
						}
					}
					break;
				case 's':
					if (minions.size() >= 4)
					{
						combatLog.push("Legion is full - select replacement!");
					}
					else //delete this when functionality for swapping minions exists
					{
						Entity newMinion = summonMenu(p);
						if (newMinion.name != "NaN")
						{
							minions.push_back(newMinion);
							combatLog.push("From the ashes rises a " + newMinion.name + " servant!");
						}
					}
					break;
				case 'b':
					if (p.sp < 1)
					{
						combatLog.push("Insufficient SP!");
					}
					else if (enemies[target].hp == 0)
					{
						combatLog.push("Target already dead!");
					}
					else
					{
						p.sp--;
						combatLog.push("The legion attacks " + enemies[target].name + " with prejudice! " + p.printSP());
						focusedAttack = true;
					}
					break;
				case 'd':
					if (spellCooldown || enemies[target].hp <= 0)
					{
						combatLog.push("Cannot cast!");
					}
					else if (p.sp <= floor(p.spMax * 0.2))
					{
						combatLog.push("Insufficient SP!");
					}
					else
					{
						p.sp -= floor(p.spMax * 0.2);
						spellCooldown = true;
						enemies[target].stunned = true;
						combatLog.push("You seize the " + enemies[target].name + "! It can no longer act! " + p.printSP());
					}
					break;
				case 'r':
					if (enemies[target].hp > 0)
					{
						combatLog.push("Cannot ressurect what isn't dead!");
					}
					else
					{
						if (minions.size() > 4)
						{
							combatLog.push("Legion is full!");
						}
						else
						{
							minions.push_back(enemies[target]);
							minions.back().hp = minions.back().hpMax;
							enemies.erase(enemies.begin() + target);
							combatLog.push(minions.back().name + " lives again! It takes rank " + to_string(minions.size()) + "!");
							sleep_for(milliseconds(1000));
						}
					}
					break;
				case 'c':
					if (enemies[target].hp > 0)
					{
						combatLog.push("Cannot consume what isn't dead!");
					}
					else
					{
						p.sp = min(p.spMax, p.sp + enemies[target].soul);
						combatLog.push(p.name + "'s avatar subsumes the " + enemies[target].name + " soul! Recovered " + to_string(enemies[target].soul) + " Soul Points! " + p.printSP());
						enemies.erase(enemies.begin() + target);
						sleep_for(milliseconds(1000));
					}
					break;
				case 'f':
					if (roundCount != 1)
					{
						combatLog.push("Too late to run!");
					}
					else if (p.sp <= 5)
					{
						combatLog.push("Insufficient SP! Too weak to run!");
					}
					else
					{
						p.sp -= 5;
						combatLog.push("You run like hell!");
						sleep_for(milliseconds(1500));
						return false;
					}
					break;
			}

			if (finalNecromancy)
			{
				combatLog.push("Ritual complete!");
				sleep_for(milliseconds(500));
				combatLog.push("Gain " + to_string(reward) + " dregs.");
				sleep_for(milliseconds(500));
				combatLog.push("Your influence grows. (MAX SP " + to_string(p.spMax) + "->" + to_string(p.spMax + 1) + ")");
				p.spMax++;
				p.dregs += reward;
				sleep_for(milliseconds(2500));
				return true;
			}

			if (target >= enemies.size())
			{
				target = enemies.size() - 1;
			}

			if (focusedAttack || randomAttack)
			{
				for (int i = 0; i < minions.size(); i++)
				{
					int damage = minions[i].dmgRoll();
					if (randomAttack)
					{
						target = randomRange(0, enemies.size() - 1);
						while (enemies[target].hp <= 0)
						{
							target = randomRange(0, enemies.size() - 1);
						}
					}
					enemies[target].hp = max(0, enemies[target].hp - damage);
					combatLog.push("-> " + minions[i].name + " strikes " + enemies[target].name + " for " + to_string(damage) + " damage! " + enemies[target].printHP());
					sleep_for(milliseconds(350));
					if (enemies[target].hp <= 0)
					{
						combatLog.push("-> The " + enemies[target].name + " falls!");
						deadEnemies++;
						sleep_for(milliseconds(500));
						if (deadEnemies == totalEnemies)
						{
							break;
						}
						if (focusedAttack && deadEnemies != totalEnemies && i + 1 < minions.size())
						{
							combatLog.push("The marked one has fallen! The assault continues at random!");
							randomAttack = true;
							sleep_for(milliseconds(350));
						}
					}
				}
				if (minions.size() == 0)
				{
					enemies[target].hp--;
					combatLog.push("-> " + p.name + "'s avatar flails at " + enemies[target].name + " for a singular point of damage!" + enemies[target].printHP());
					sleep_for(milliseconds(700));
					if (enemies[target].hp <= 0)
					{
						combatLog.push("Miraculously, it is enough! " + enemies[target].name + " perishes!");
						deadEnemies++;
						sleep_for(milliseconds(700));
					}
					else
					{
						combatLog.push("It is a futile effort!");
						sleep_for(milliseconds(700));
					}
				}
				turn = "enemy";
				randomAttack = false;
				focusedAttack = false;
				sleep_for(milliseconds(300));
			}

			if (deadEnemies == totalEnemies)
			{
				combatLog.push("All enemies vanquished! Perform one final necromancy.");
				finalNecromancy = true;
			}
		}
		for (int i = 0; i < enemies.size(); i++)
		{
			if (enemies[i].hp > 0)
			{
				if (enemies[i].stunned)
				{
					combatLog.push(enemies[i].name + " cannot act, but recovers from the hex!");
					enemies[i].stunned = false;
				}
				else
				{
					target = randomRange(0, minions.size() - 1);
					int damage = enemies[i].dmgRoll();
					if (minions.size() == 0)
					{
						p.sp = max(0, p.sp - damage);
						combatLog.push(enemies[i].name + " hits the avatar for " + to_string(damage) + " damage! " + p.printSP());
						sleep_for(milliseconds(500));
						if (p.sp <= 0)
						{
							combatLog.push(enemies[i].name + " lands the killing blow on " + p.name + "'s avatar...!");
							combatLog.push("Return to oblivion!");
							return false;
						}
					}
					else
					{
						minions[target].hp = max(0, minions[target].hp - damage);
						combatLog.push("<- " + enemies[i].name + " strikes your " + minions[target].name + " for " + to_string(damage) + " damage! " + minions[target].printHP());
						sleep_for(milliseconds(350));
						if (minions[target].hp <= 0)
						{
							combatLog.push("<- " + enemies[i].name + " returns " + minions[target].name + " to the dead...!");
							minions.erase(minions.begin() + target);
							sleep_for(milliseconds(500));
						}
					}
				}
			}
		}
		turn = "player";
		roundCount++;
		spellCooldown = false;
		target = 0;
		combatLog.push("ROUND " + to_string(roundCount));
	}
}

void drawBoard(vector<vector<char>>& dungeon, int width, int height, Player p, int camX, int camY, vector<mapEntity> ent)
{
	vector<vector<bool>> visibleSquares = generateLOSMaskRaycast(dungeon, width, height, Point(p.x, p.y), camX, camY);
	cout << "+--------------------+" << endl;
	for (int i = camY; i < height && i < camY+10; i++)
	{
		cout << "|";
		for (int j = camX; j < width && j < camX+20; j++)
		{
			if (i == p.y && j == p.x)
			{
				cout << "@";
			}
			else
			{
				if (false)//!visibleSquares[i-camY][j-camX])
				{
					cout << " ";
				}
				else
				{
					bool occupiedByEntity = false;
					for (int k = 0; k < ent.size(); k++)
					{
						if (i == ent[k].y && j == ent[k].x)
						{
							occupiedByEntity = true;
							cout << ent[k].graphic;
						}
					}
					if (!occupiedByEntity)
					{
						cout << dungeon[i][j];
					}
				}
			}
		}
		cout << "|";
		cout << endl;
	}
	cout << "+--------------------+" << endl;
}

void dungeonGenerationFromImage(vector<vector<char>>& dungeon, vector<mapEntity>& entities, vector<vector<Entity>> encounterPool, const char* imageFile, int xOrigin, int yOrigin)
{
	int width, height, nrChannels;
	unsigned char* mapImage = stbi_load(imageFile, &width, &height, &nrChannels, 0);
	//cout << "width " << width << endl;
	//cout << "height " << height << endl;

	//int row = 0;
	for (int i = yOrigin; i < height+yOrigin; i++)
	{
		for (int j = xOrigin; j < width+xOrigin; j++)
		{
			int index = (j-xOrigin) * 3 + (i-yOrigin) * 3 * width;
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
				bool allEnemiesDead = combat(entities[combatFlag].encounter, p, minions);
				if (allEnemiesDead)
				{
					entities.erase(entities.begin() + combatFlag);
					travelLog.push("Defeated the enemy horde!");
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

		/*
		if (entities[1].isInView(cameraX, cameraY))
		{
			travelLog.push("Visible! " + entities[1].printPosition());
		}
		else
		{
			travelLog.push("Invisible! " + entities[1].printPosition());
		};
		*/

		//system("cls");
		clearScreen();
		//setCursorPosition(0,0);
	}
	return 0;
}