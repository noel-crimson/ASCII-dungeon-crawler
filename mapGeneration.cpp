#include "mapGeneration.h"
#include "customOperations.h"
#include <queue>

int Node::totalNodes = 0;

Room::Room(int x0, int y0, const char* roomFile) : x(x0), y(y0)
{
	//int width, height, nrChannels;
	//unsigned char* mapImage = stbi_load("room1.png", &width, &height, &nrChannels, 0);
	stbi_load(roomFile, &width, &height, 0, 0);
	//points between which corridors are connected (per room, to minimize adjacent corridors) but it's not guaranteed to be a free square
}

Room::Room() {}

void Room::generateCorridorAnchor(std::vector<std::vector<char>> dungeon)
{
	corridorAnchor = findEmptyPointInRoom(*this, dungeon);
}

bool Room::isOverlapping(Room r2)
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

bool Room::isWithinDistance(Room r2, int dist)
{
	return ((abs(x - r2.x) + abs(y - r2.y)) < dist);
}

Point Room::randomPoint()
{
	return Point(randomRange(x + 1, x + width - 1), randomRange(y + 1, y + height - 1));
}


Point findEmptyPointInRoom(Room r, std::vector<std::vector<char>> dungeon)
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


void orderRooms(std::vector<Room>& r)
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


Node::Node(Room r0) : r(r0), parent(nullptr)
{
	nodeID = totalNodes;
	totalNodes++;
}

void Node::insert(Room r0)
{
	Node* newNode = new Node(r0);
	newNode->parent = this;
	branches.push_back(newNode);
}

void Node::iterateTree()
{
	std::queue<Node*> q;
	q.push(this);

	while (!q.empty())
	{
		Node* current = q.front();
		q.pop();

		std::cout << current->r.x << " " << current->r.y << " ";

		for (Node* child : current->branches)
		{
			q.push(child);
		}
	}
}

void Node::addToNearestRoom(Room newRoom)
{

	int distance = 0;

	//get a pointer to the closest node on the tree
	std::queue<Node*> q;
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

void Node::burnCorridors(std::vector<std::vector<char>>& dungeon)
{
	std::queue<Node*> q;
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

//generation algorithm
//order rooms in vector such that adjacent rooms are the closest to each other they can get
//pass room to tree, it gets attached to the nearest room in the tree

Node* generateRoomTree(std::vector<Room>& r)
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

void connectPoints(std::vector<std::vector<char>>& dungeon, Point p1, Point p2)
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