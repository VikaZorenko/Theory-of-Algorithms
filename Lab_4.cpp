#include "stdafx.h"
#include <iostream>
#include <string>
#include <clocale>
#include<memory>
#include <Windows.h>
#include "Graph.h"

int main()
{
	setlocale(LC_ALL, "ru");
	SetConsoleCP(1251); 
	SetConsoleOutputCP(1251);
	Graph::Graph<std::string> g;
	g.addVertex("Абу-Даби", 24.44, 54.26);
	g.addVertex("Дубай", 25.21, 55.27);
	g.addVertex("Шарджа", 25.35, 55.52);
	g.addVertex("Аджман", 25.41, 55.52);
	g.addVertex("Умм-эль-Кайвайн", 25.52, 55.58);
	g.addVertex("Рас-эль-Хайма", 25.81, 55.97);
	g.addVertex("Хаур-Факкан", 25.35, 56.38);
	g.addVertex("Эль-Фуджайра", 25.15, 56.38);
	g.addVertex("Кайба", 25.07, 56.35);
	g.addVertex("Эль-Айн", 24.16, 55.74);
	g.addVertex("Хамим", 22.99, 54.3);
	g.addVertex("Тарак", 23.12, 53.63);
	g.addVertex("Мадинат-Зайед", 23.66, 53.68);
	g.addVertex("Тариф", 24.04, 53.75);
	g.addVertex("Рувайс", 24.09, 52.68);
	
	
	g.connect("Рувайс", "Тариф", 119);
	g.connect("Мадинат-Зайед", "Рувайс", 142);
	g.connect("Мадинат-Зайед", "Тариф", 64);
	g.connect("Мадинат-Зайед", "Тарак", 84);
	g.connect("Мадинат-Зайед", "Хамим", 130);
	g.connect("Тариф", "Абу-Даби", 121);
	g.connect("Тарак", "Хамим", 95);
	g.connect("Хамим", "Эль-Айн", 306);
	g.connect("Эль-Айн", "Эль-Фуджайра", 187);
	g.connect("Эль-Айн", "Дубай", 147);
	g.connect("Эль-Айн", "Абу-Даби", 183);
	g.connect("Кайба", "Эль-Фуджайра", 11);
	g.connect("Эль-Фуджайра", "Хаур-Факкан", 28);
	g.connect("Эль-Фуджайра", "Умм-эль-Кайвайн", 126);
	g.connect("Эль-Фуджайра", "Шарджа", 105);
	g.connect("Рас-эль-Хайма", "Хаур-Факкан", 99);
	g.connect("Рас-эль-Хайма", "Умм-эль-Кайвайн", 56);
	g.connect("Умм-эль-Кайвайн", "Аджман", 18);
	g.connect("Аджман", "Шарджа", 59);
	g.connect("Дубай", "Абу-Даби", 139);
	g.connect("Дубай", "Шарджа", 58);

	bool close = false;
	int oper;
	std::string start, end, st, ed;
	std::shared_ptr<Graph::Way> way;
	while (!close) {
		std::cout << "Выберите операцию:\n"
			<< "1. Поиск минимального пути с помощью алгоритма жадного алгоритма\n"
			<< "2. Поиск минимального пути с помощью алгоритма А*\n"
			<< "0. Закрыть\n";
		std::cout << "Операция: ";
		std::cin >> oper;
		switch (oper)
		{
		case 1:
			std::cout << "Введите город начала движения: ";
			std::cin >> start;
			std::cout << "Введите конечный город: ";
			std::cin >> end;
			try {
				way = g.findMinWayGreed(start, end);
			}
			catch (std::exception &err) {
				std::cout << err.what() << "\n\n";
				break;
			}
			std::cout << "Путь, найденый алгоритмом жадным алгоритмом:\n";
			g.printWay(way);
			std::cout << std::endl;
			break;
		case 2:
			std::cout << "Введите город начала движения: ";
			std::cin >> start;
			std::cout << "Введите конечный город: ";
			std::cin >> end;
			try {
				way = g.findMinWayA(start, end);
			}
			catch (std::exception &err) {
				std::cout << err.what() << "\n\n";
				break;
			}
			std::cout << "Путь, найденый алгоритмом А*:\n";
			g.printWay(way);
			std::cout << std::endl;
			break;
		case 0:
			close = true;
			break;
		default:
			std::cout << "Ошибка ввода!\n";
			break;
		}
	}
    return 0;
}

