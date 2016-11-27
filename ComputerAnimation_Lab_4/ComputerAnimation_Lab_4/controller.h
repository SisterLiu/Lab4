#pragma once
#include "userInput.h"
#include "model.h"
#include "dx11Displayer.h"
#include <vector>

class Controller
{
	public:
		void next(std::vector<Object*>*);
	private:
		std::vector<Object*>* pObjects;

		void collision();
		void speed();
		void move();
		Force userForce;
		bool checkCollisionAndSetForce(Object*, Object*);
};