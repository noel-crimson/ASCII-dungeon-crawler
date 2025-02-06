#include "entities.h"
#include "random.h"
#include "display.h"


Point::Point(int x0, int y0) 
{
    x = x0;
    y = y0;
}

std::string Point::printPoint()
{
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}


Entity::Entity() : hpMax(1), hp(1), pow(1), dmgDie(0), soul(1), graphic("?"), name("NaN") {};

Entity::Entity(int hpMaxVal, int powVal, int dmgDieVal, int soulVal, std::string graphicVal, std::string nameVal)
	: hpMax(hpMaxVal), hp(hpMaxVal), pow(powVal), dmgDie(dmgDieVal), soul(soulVal), graphic(graphicVal), name(nameVal) {};

int Entity::dmgRoll()
{
	return rollD(dmgDie, pow);
}

std::string Entity::printHP()
{
	std::stringstream result;
	result << "(HP: " << hp << "/" << hpMax << ")";
	return result.str();
}

Log::Log() {};

void Log::push(std::string msg, bool instantPrint)
{
	//string msg = ssmsg.str();
	if (instantPrint)
	{
		std::cout << msg << std::endl;
	}

	messages.push_back(msg);
	if (messages.size() > 13)
	{
		messages.erase(messages.begin());
	}
	//ssmsg.str("");
};

void Log::printLog()
{
	for (int i = 0; i < messages.size(); i++)
	{
		std::cout << messages[i] << std::endl;
	}
}

mapEntity::mapEntity(Point p, char graphicValue) : x(p.x), y(p.y), graphic(graphicValue) {};

void mapEntity::move(std::vector<int> movement)
{
	x += movement[0];
	y += movement[1];
}

std::string mapEntity::printPosition()
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

bool mapEntity::canMove(std::vector<int> movement, std::vector<std::vector<char>>& map, std::vector<mapEntity>& entities)
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
bool mapEntity::overlapsWall(std::vector<std::vector<char>>& map)
{
	return (map[y][x] == '#');
}

bool mapEntity::isInView(int camX, int camY)
{
	return ((x < (VIEWPORTX + camX)) && (x >= camX) && (y < (VIEWPORTY + camY)) && (y >= camY));
}

bool isClear(Point p, std::vector<std::vector<char>>& dungeon, std::vector<mapEntity>& entities)
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

bool isClearOfWalls(Point p, std::vector<std::vector<char>>& dungeon)
{
	bool returnValue = true;
	if ((dungeon[p.y][p.x] == '#'))
	{
		returnValue = false;
	}
	return returnValue;
}


Player::Player(Point p) : mapEntity(p)
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
	summonPool = {};
}

void Player::move(std::vector<int> movement, std::vector<mapEntity>& mapEntities, Log& log, int& combatFlag)
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

std::string Player::printSP()
{
	std::stringstream result;
	result << "(SP: " << sp << "/" << spMax << ")";
	return result.str();
}

itemPickup::itemPickup(Point p, char graphicValue) : mapEntity(p, graphicValue) {}


potionPickup::potionPickup(Point p) : itemPickup(p, 'p') {};


wanderingMonster::wanderingMonster(Point p, std::vector<Entity> e) : mapEntity(p, 'M')
{
	encounter = e;
	isActive = false;
}