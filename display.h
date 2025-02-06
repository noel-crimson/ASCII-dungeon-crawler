#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <iostream>
#include "entities.h"

#include <stb_image.h>

const int VIEWPORTX = 20;
const int VIEWPORTY = 10;

void setCursorPosition(int x, int y);

void clearScreen(int clearWidth = 30);

void printBuffer();

void drawBoard(std::vector<std::vector<char>>& dungeon, int width, int height, Player p, int camX, int camY, std::vector<mapEntity> ent);

void dungeonGenerationFromImage(std::vector<std::vector<char>>& dungeon, std::vector<mapEntity>& entities, std::vector<std::vector<Entity>> encounterPool, const char* imageFile, int xOrigin, int yOrigin);


