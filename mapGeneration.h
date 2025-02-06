#pragma once
#include "entities.h"
#include "display.h"
#include "random.h"

struct Room
{
	int x;
	int y;
	int width;
	int height;
	Point corridorAnchor = Point(0, 0);
	Room(int x0, int y0, const char* roomFile);
	Room();
	void generateCorridorAnchor(std::vector<std::vector<char>> dungeon);
	bool isOverlapping(Room r2);
	bool isWithinDistance(Room r2, int dist);
	Point randomPoint();
};

Point findEmptyPointInRoom(Room r, std::vector<std::vector<char>> dungeon);

void orderRooms(std::vector<Room>& r);

struct Node
{
	Node* parent;
	std::vector<Node*> branches;
	Room r;
	static int totalNodes;
	int nodeID;
	Node(Room r0);

	void insert(Room r0);

	void iterateTree();

	void addToNearestRoom(Room newRoom);

	void burnCorridors(std::vector<std::vector<char>>& dungeon);
};

//int Node::totalNodes = 0;

//generation algorithm
//order rooms in vector such that adjacent rooms are the closest to each other they can get
//pass room to tree, it gets attached to the nearest room in the tree

Node* generateRoomTree(std::vector<Room>& r);

void connectPoints(std::vector<std::vector<char>>& dungeon, Point p1, Point p2);