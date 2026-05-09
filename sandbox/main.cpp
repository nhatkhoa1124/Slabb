#include <iostream>

#include "core/application.hpp"

using slabb::core::Application;

int main()
{
	Application app = {};
	app.init_subsystems();

	std::cout << "Main running\n";
	return 0;
}