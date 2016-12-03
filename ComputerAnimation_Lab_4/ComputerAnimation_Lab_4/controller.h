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
		int type;
		static const int BORDER = 0x01;
		static const int PILLAR = 0x02;
		SceneBorder() :xMin(0),xMax(0),yMin(0),yMax(0),zMin(0),zMax(0),type(BORDER)
		{}
};

class Controller
{
	public:
		void next(std::vector<Object*>*);
		void setBorder(SceneBorder border)
		{
			this->borders.push_back(border);
		};
		SceneBorder getBorder(int i)
		{
			return borders[i];
		};
	
	private:
		std::vector<Object*>* pObjects;
		std::vector<SceneBorder> borders;
		void collision();
		void speed();
		void setSpeed(Object*);
		void friction(Object*);
		void move();
		void setMove(Object*);
		void clearForce();

		void center();
		bool setCenterForce(Object*);
		Force userForce;
		DirectX::XMFLOAT3 objectCenter;
		bool checkCollisionAndSetForce(Object*, Object*);
		bool checkBorderAndSetGravity(Object*,SceneBorder);
		float Distance(DirectX::XMFLOAT3 f1, DirectX::XMFLOAT3 f2);
		float Distance(float x1, float y1, float x2, float y2);
};