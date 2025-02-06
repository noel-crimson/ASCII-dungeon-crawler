#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

struct Point
{
    int x;
    int y;
    Point(int x0, int y0);
    std::string printPoint();
};

struct Entity
{
	int hp;
	int hpMax;
	int pow;
	int dmgDie;
	int soul;
	bool stunned;
	std::string graphic;
	std::string name;
	Entity();

	Entity(int hpMaxVal, int powVal, int dmgDieVal, int soulVal, std::string graphicVal = "?", std::string nameVal = "Bob");

	int dmgRoll();

	std::string printHP();
};

struct Log
{
	std::vector<std::string> messages = {};
	Log();
	void push(std::string msg, bool instantPrint = true);
	void printLog();
};


struct mapEntity
{
	std::vector<Entity> encounter = {};
	int x;
	int y;
	char graphic;
	int isActive;
	mapEntity(Point p, char graphicValue = '?');
	void move(std::vector<int> movement);
	std::string printPosition();
	bool canMove(std::vector<int> movement, std::vector<std::vector<char>>& map, std::vector<mapEntity>& entities);
	bool overlapsWall(std::vector<std::vector<char>>& map);;
	bool isInView(int camX, int camY);
};

bool isClear(Point p, std::vector<std::vector<char>>& dungeon, std::vector<mapEntity>& entities);

bool isClearOfWalls(Point p, std::vector<std::vector<char>>& dungeon);


struct Player : Entity, mapEntity
{
	int sp;
	int spMax;
	int level;
	int xpNext;
	int dregs;
	int gold;
	int control;
	std::vector<Entity> summonPool;
	Player(Point p);
	void move(std::vector<int> movement, std::vector<mapEntity>& mapEntities, Log& log, int& combatFlag);
	std::string printSP();
};

struct itemPickup : mapEntity
{
	itemPickup(Point p, char graphicValue = '!');
};

struct potionPickup : itemPickup
{
	potionPickup(Point p);
};

struct wanderingMonster : mapEntity
{
	bool isActive = false;
	wanderingMonster(Point p, std::vector<Entity> e);
};