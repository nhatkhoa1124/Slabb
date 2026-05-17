#include <iostream>

#include "core/application.hpp"

using slabb::core::Application;

int main()
{
	try
	{
		auto app = std::make_unique<Application>();
		app->init_subsystems();
		app->run();
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		return -1;
	}

	return 0;
}