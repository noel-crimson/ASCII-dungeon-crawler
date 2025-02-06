#pragma once
#include <vector>
#include "entities.h"
#include "display.h"
#include <string>
#include <conio.h>
#include <chrono>
#include <thread>
#include "customOperations.h"
#include <random>

Entity summonMenu(Player& p)
{
	Entity dummy;
	std::string input;
	std::cout << "By the avatar of " << p.name << "... " << p.printSP() << std::endl;

	for (int i = 0; i < p.summonPool.size(); i++)
	{
		std::cout << p.summonPool[i].name << " (Cost: " << p.summonPool[i].soul * 3 << " SP)" << std::endl;
	}

	std::cout << "Summon by name, or CANCEL:";
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	while (true)
	{
		std::cin >> input;

		if (input == "cancel")
		{
			return dummy;
		}
		for (int i = 0; i < p.summonPool.size(); i++)
		{
			std::cout << p.summonPool[i].name << " ";
			if (lowercaseString(input) == lowercaseString(p.summonPool[i].name))
			{
				if (p.summonPool[i].soul * 3 >= p.sp)
				{
					std::cout << "Insufficient SP to summon " << p.summonPool[i].name << " servant!" << std::endl;
				}
				else
				{
					p.sp -= p.summonPool[i].soul * 3;
					return Entity(p.summonPool[i]);
				}
			}
		}
		std::cout << std::endl;

	}
}

bool combat(std::vector<Entity> enemies, Player& p, std::vector<Entity>& minions)
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
	std::string turn = "player";
	combatLog.push("BEGIN", false);
	while (p.sp > 0)
	{
		while (turn == "player")
		{
			clearScreen();
			std::cout << "= = = = ROUND " << std::to_string(roundCount) << " = = = =" << std::endl;
			std::cout << " = = FOE " << std::endl << std::endl;
			for (int i = 0; i < enemies.size(); i++)
			{
				if (enemies[i].hp > 0)
				{
					std::cout << "   " << enemies[i].graphic;
				}
				else
				{
					std::cout << "   " << "X";
				}
			}
			std::cout << std::endl << "- - - - - - - - - - -" << std::endl;
			for (int i = 0; i < enemies.size(); i++)
			{
				std::cout << "   ";
				if (target == i)
				{
					std::cout << "^";
				}
				else
				{
					std::cout << " ";
				}
			}
			std::cout << std::endl;
			for (int i = 0; i < minions.size(); i++)
			{
				std::cout << "   " << minions[i].graphic;
			}
			std::cout << std::endl;
			std::cout << "          LEGION = = " << std::endl;

			std::cout << "TARGET [1-" << enemies.size() << "]: " << enemies[target].name << " " << enemies[target].printHP() << std::endl;
			std::cout << p.name << " " << p.printSP() << std::endl;
			if (minions.size() > 0 && !finalNecromancy)
			{
				std::cout << "(A)ttack Indiscriminately" << std::endl;
			}
			else if (enemies[target].hp > 0)
			{
				std::cout << "(A)ttack in Desperation" << std::endl;
			}

			if (!finalNecromancy)
			{
				std::cout << "(S)ummon the Dead" << std::endl;
			}

			if (enemies[target].hp > 0 && minions.size() > 0)
			{
				std::cout << "(B)reak This One (1SP)" << std::endl;
			}

			if (enemies[target].hp > 0)
			{
				std::cout << "(D)ebilitating Force (" << floor(p.spMax * 0.2) << " SP)" << std::endl;
			}
			else
			{
				std::cout << "(R)essurect the Dead" << std::endl;
				std::cout << "(C)onsume the Dead" << std::endl;
			}
			if (roundCount == 1)
			{
				std::cout << "(F)lee Unnoticed (5SP)" << std::endl;
			}
			std::cout << "..." << std::endl;

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
						combatLog.push(minions.back().name + " lives again! It takes rank " + std::to_string(minions.size()) + "!");
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
					combatLog.push(p.name + "'s avatar subsumes the " + enemies[target].name + " soul! Recovered " + std::to_string(enemies[target].soul) + " Soul Points! " + p.printSP());
					enemies.erase(enemies.begin() + target);
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
					std::this_thread::sleep_for(std::chrono::milliseconds(1500));
					return false;
				}
				break;
			}

			if (finalNecromancy)
			{
				combatLog.push("Ritual complete!");
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				combatLog.push("Gain " + std::to_string(reward) + " dregs.");
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				combatLog.push("Your influence grows. (MAX SP " + std::to_string(p.spMax) + "->" + std::to_string(p.spMax + 1) + ")");
				p.spMax++;
				p.dregs += reward;
				std::this_thread::sleep_for(std::chrono::milliseconds(2500));
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
					combatLog.push("-> " + minions[i].name + " strikes " + enemies[target].name + " for " + std::to_string(damage) + " damage! " + enemies[target].printHP());
					std::this_thread::sleep_for(std::chrono::milliseconds(350));
					if (enemies[target].hp <= 0)
					{
						combatLog.push("-> The " + enemies[target].name + " falls!");
						deadEnemies++;
						std::this_thread::sleep_for(std::chrono::milliseconds(500));
						if (deadEnemies == totalEnemies)
						{
							break;
						}
						if (focusedAttack && deadEnemies != totalEnemies && i + 1 < minions.size())
						{
							combatLog.push("The marked one has fallen! The assault continues at random!");
							randomAttack = true;
							std::this_thread::sleep_for(std::chrono::milliseconds(350));
						}
					}
				}
				if (minions.size() == 0)
				{
					enemies[target].hp--;
					combatLog.push("-> " + p.name + "'s avatar flails at " + enemies[target].name + " for a singular point of damage!" + enemies[target].printHP());
					std::this_thread::sleep_for(std::chrono::milliseconds(700));
					if (enemies[target].hp <= 0)
					{
						combatLog.push("Miraculously, it is enough! " + enemies[target].name + " perishes!");
						deadEnemies++;
						std::this_thread::sleep_for(std::chrono::milliseconds(700));
					}
					else
					{
						combatLog.push("It is a futile effort!");
						std::this_thread::sleep_for(std::chrono::milliseconds(700));
					}
				}
				turn = "enemy";
				randomAttack = false;
				focusedAttack = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
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
						combatLog.push(enemies[i].name + " hits the avatar for " + std::to_string(damage) + " damage! " + p.printSP());
						std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
						combatLog.push("<- " + enemies[i].name + " strikes your " + minions[target].name + " for " + std::to_string(damage) + " damage! " + minions[target].printHP());
						std::this_thread::sleep_for(std::chrono::milliseconds(350));
						if (minions[target].hp <= 0)
						{
							combatLog.push("<- " + enemies[i].name + " returns " + minions[target].name + " to the dead...!");
							minions.erase(minions.begin() + target);
							std::this_thread::sleep_for(std::chrono::milliseconds(500));
						}
					}
				}
			}
		}
		turn = "player";
		roundCount++;
		spellCooldown = false;
		target = 0;
		combatLog.push("ROUND " + std::to_string(roundCount));
	}
}