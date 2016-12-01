#include "controller.h"
using namespace DirectX;

#define PI 3.14159265359f
#define D(x) (x / PI * 180.0f)

void Controller::next(std::vector<Object*>* pObjects)
{
	this->pObjects = pObjects;
	collision();
	speed();
	move();
	clearForce();
}

void Controller::clearForce()
{
	for(int i = 0; i < pObjects->size(); i++)
	{
		if((*pObjects)[i]->motion.fixed)
			continue;
		(*pObjects)[i]->motion.forces.clear();
	}
}

void Controller::collision()
{
	for(int i = 0; i < pObjects->size(); i++)
	{
		for(int j = i+1; j < pObjects->size(); j++)
		{
			checkCollisionAndSetForce((*pObjects)[i], (*pObjects)[j]);
		}
		checkBorderAndSetGravity((*pObjects)[i]);
	}
}

void Controller::speed()
{
	for(int i = 0; i < pObjects->size(); i++)
	{
		setSpeed((*pObjects)[i]);
	}
}

void Controller::setSpeed(Object* pObj)
{
	if(pObj->motion.fixed)return;

	XMFLOAT3 forceCollision = {0,0,0};
	XMVECTOR forceSuspend = XMVectorSet(0, 0, 0, 0);
	XMVECTOR currentForce;

	for(int j = 0; j < pObj->motion.forces.size(); j++)
	{
		forceCollision.x += pObj->motion.forces[j].direction.x;
		forceCollision.y += pObj->motion.forces[j].direction.y;
		forceCollision.z += pObj->motion.forces[j].direction.z;
	}

	pObj->motion.speed.x += forceCollision.x / pObj->motion.mess;
	pObj->motion.speed.y += forceCollision.y / pObj->motion.mess;
	pObj->motion.speed.z += forceCollision.z / pObj->motion.mess;

	friction(pObj);
}

void Controller::friction(Object* pObj)
{
	if(pObj->motion.speed.x < pObj->motion.friction && pObj->motion.speed.x > -pObj->motion.friction)
		pObj->motion.speed.x = 0;
	else
		if(pObj->motion.speed.x > 0)
			pObj->motion.speed.x -= pObj->motion.friction;
		else
			pObj->motion.speed.x += pObj->motion.friction;

	if(pObj->motion.speed.y < pObj->motion.friction && pObj->motion.speed.y > -pObj->motion.friction)
		pObj->motion.speed.y = 0;
	else
		if(pObj->motion.speed.y > 0)
			pObj->motion.speed.y -= pObj->motion.friction;
		else
			pObj->motion.speed.y += pObj->motion.friction;

	if(pObj->motion.speed.z < pObj->motion.friction && pObj->motion.speed.z > -pObj->motion.friction)
		pObj->motion.speed.z = 0;
	else
		if(pObj->motion.speed.z > 0)
			pObj->motion.speed.z -= pObj->motion.friction;
		else
			pObj->motion.speed.z += pObj->motion.friction;
}

void Controller::move()
{
	Object* pObject;
	for(int i = 0; i < pObjects->size(); i++)
	{
		setMove((*pObjects)[i]);
	}
}

void Controller::setMove(Object* pObj)
{
	if(pObj->motion.fixed)return;
	
	if(pObj->motion.speed.x < -pObj->motion.friction || pObj->motion.speed.x > pObj->motion.friction)
		pObj->pos.x += pObj->motion.speed.x;
	if(pObj->motion.speed.y < -pObj->motion.friction || pObj->motion.speed.y > pObj->motion.friction)
		pObj->pos.y += pObj->motion.speed.y;
	if(pObj->motion.speed.z < -pObj->motion.friction || pObj->motion.speed.z > pObj->motion.friction)
		pObj->pos.z += pObj->motion.speed.z;

	XMVECTOR speedDirect = XMVector3Normalize(XMLoadFloat3(&pObj->motion.speed));
	XMVECTOR UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Left = XMVector3Normalize(XMVector3Cross(speedDirect, UP));
	XMVECTOR Front = XMVector3Normalize(XMVector3Cross(UP, Left));
	XMFLOAT3 angle;

	
	XMStoreFloat3( &angle, XMVector3AngleBetweenVectors(Front, speedDirect));
	if(pObj->motion.speed.y>0)
	{
		pObj->angle.x = -D(angle.x);
		if(pObj->motion.speed.x>-0.000001&&pObj->motion.speed.x<0.000001)
			if(pObj->motion.speed.z>-0.000001&&pObj->motion.speed.z<0.000001)
				pObj->angle.x = -90;
	}
	else
	{
		pObj->angle.x = D(angle.x);
		if(pObj->motion.speed.x > -0.000001&&pObj->motion.speed.x < 0.000001)
			if(pObj->motion.speed.z > -0.000001&&pObj->motion.speed.z < 0.000001)
				pObj->angle.x = 90;
	}

	if(pObj->motion.speed.y > -0.000001&&pObj->motion.speed.y < 0.000001)
		pObj->angle.x = 0;

	XMStoreFloat3(&angle, XMVector3AngleBetweenVectors(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), speedDirect));

	if(pObj->motion.speed.x>0)
		pObj->angle.y = D(angle.x);
	else
		pObj->angle.y = -D(angle.x);
	
}

float Controller::Distance(DirectX::XMFLOAT3 f1, DirectX::XMFLOAT3 f2)
{
	return sqrt((f1.x - f2.x) * (f1.x - f2.x) +
		(f1.y - f2.y) * (f1.y - f2.y) +
		(f1.z - f2.z) * (f1.z - f2.z));
}

bool Controller::checkCollisionAndSetForce(Object* pObj1, Object* pObj2)
{
	if(pObj1->motion.fixed || pObj2->motion.fixed)
		return false;

	float r;
	r = Distance(pObj1->pos, pObj2->pos) * 5;
	Force F;
	F.direction.x = pObj1->pos.x - pObj2->pos.x;
	F.direction.y = pObj1->pos.y - pObj2->pos.y;
	F.direction.z = pObj1->pos.z - pObj2->pos.z;
	float l;
	l = Distance(F.direction, {0,0,0});
	F.direction.x /= r*r*l;
	F.direction.y /= r*r*l;
	F.direction.z /= r*r*l;
	F.Flag = Force::COLLISION;
	pObj1->motion.forces.push_back(F);
	F.direction.x *= -1;
	F.direction.y *= -1;
	F.direction.z *= -1;
	pObj2->motion.forces.push_back(F);
	return true;
}

bool Controller::checkBorderAndSetGravity(Object* pObj)
{
	if(pObj->motion.fixed)
		return false;
	Force gravity;
	gravity.direction.x = pObj->motion.mess * pObj->motion.gravity.x;
	gravity.direction.y = pObj->motion.mess * pObj->motion.gravity.y;
	gravity.direction.z = pObj->motion.mess * pObj->motion.gravity.z;
	gravity.Flag = gravity.COLLISION;

	// down
	if(pObj->pos.y > pObj->pModel->collision.y + border.yMin)
	{
		pObj->motion.forces.push_back(gravity);
	}
	else
	{
		if(pObj->motion.speed.y < 0)
		{
			gravity.direction.y = -pObj->motion.speed.y*pObj->motion.mess * 2 * 0.98;
			pObj->motion.forces.push_back(gravity);
		}
	}

	// up
	if(pObj->pos.y > border.yMax - pObj->pModel->collision.y)
	{
		Force F;
		if(pObj->motion.speed.y > 0)
		{
			F.direction.y = -pObj->motion.speed.y*pObj->motion.mess * 2 * 0.98;
			F.Flag = F.COLLISION;
			pObj->motion.forces.push_back(F);
		}
	}

	// x+
	if(pObj->pos.x > border.xMax)
	{
		Force F;
		if(pObj->motion.speed.x > 0)
		{
			F.direction.x = -pObj->motion.speed.x*pObj->motion.mess * 2 * 0.98;
			F.Flag = F.COLLISION;
			pObj->motion.forces.push_back(F);
		}
	}

	// x-
	if(pObj->pos.x < border.xMin)
	{
		Force F;
		if(pObj->motion.speed.x < 0)
		{
			F.direction.x = -pObj->motion.speed.x*pObj->motion.mess * 2 * 0.98;
			F.Flag = F.COLLISION;
			pObj->motion.forces.push_back(F);
		}
	}

	// z+
	if(pObj->pos.z > border.zMax)
	{
		Force F;
		if(pObj->motion.speed.z > 0)
		{
			F.direction.z = -pObj->motion.speed.z*pObj->motion.mess * 2 * 0.98;
			F.Flag = F.COLLISION;
			pObj->motion.forces.push_back(F);
		}
	}

	// z-
	if(pObj->pos.z < border.zMin)
	{
		Force F;
		if(pObj->motion.speed.z < 0)
		{
			F.direction.z = -pObj->motion.speed.z*pObj->motion.mess * 2 * 0.98;
			F.Flag = F.COLLISION;
			pObj->motion.forces.push_back(F);
		}
	}

	return true;
}

