#pragma once
#include "userInput.h"
#include "model.h"
#include "dx11Displayer.h"
#include <vector>

class SceneBorder
{
	public:
		float xMin;
		float xMax;
		float yMin;
		float yMax;
		float zMin;
		float zMax;
		SceneBorder() :xMin(0),xMax(0),yMin(0),yMax(0),zMin(0),zMax(0)
		{}
};

class Controller
{
	public:
		void next(std::vector<Object*>*);
		void setBorder(SceneBorder border)
		{
			this->border = border;
		};
		SceneBorder getBorder()
		{
			return border;
		};
	
	private:
		std::vector<Object*>* pObjects;
		SceneBorder border;
		void collision();
		void speed();
		void setSpeed(Object*);
		void friction(Object*);
		void move();
		void setMove(Object*);
		void clearForce();
		Force userForce;
		bool checkCollisionAndSetForce(Object*, Object*);
		bool checkBorderAndSetGravity(Object*);
		float Distance(DirectX::XMFLOAT3 f1, DirectX::XMFLOAT3 f2);
};