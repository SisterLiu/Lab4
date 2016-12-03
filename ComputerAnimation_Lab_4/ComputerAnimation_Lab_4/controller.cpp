#include "controller.h"
using namespace DirectX;

#define PI 3.14159265359f
#define D(x) (x / PI * 180.0f)

void Controller::next(std::vector<Object*>* pObjects)
{
	this->pObjects = pObjects;

	//	set Object center
	objectCenter = {0,0,0};
	int moveObjectCount = 0;
	for(int i = 0; i < pObjects->size(); i++)
	{
		if((*pObjects)[i]->motion.fixed)
			continue;
		moveObjectCount++;
		objectCenter.x += (*pObjects)[i]->pos.x;
		objectCenter.y += (*pObjects)[i]->pos.y;
		objectCenter.z += (*pObjects)[i]->pos.z;
	}
	if(moveObjectCount != 0)
	{
		objectCenter.x /= moveObjectCount;
		objectCenter.y /= moveObjectCount;
		objectCenter.z /= moveObjectCount;
	}

	collision();
	center();
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
		for(int j=0;j<borders.size();j++)
			checkBorderAndSetGravity((*pObjects)[i],borders[j]);
		setCenterForce((*pObjects)[i]);
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

	//	Limit Speed
	XMVECTOR speedV = XMLoadFloat3(&pObj->motion.speed);
	XMFLOAT3 speed3;
	XMStoreFloat3(&speed3, XMVector3Length(speedV));
	if(speed3.x > 1)
	{
		speedV = XMVector3Normalize(speedV);
		speedV *= 1;
		XMStoreFloat3(&pObj->motion.speed, speedV);
	}

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

float Controller::Distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

bool Controller::checkCollisionAndSetForce(Object* pObj1, Object* pObj2)
{
	if(pObj1->motion.fixed || pObj2->motion.fixed)
		return false;

	if(Distance(pObj1->pos, pObj2->pos) > 15)
		return false;

	XMVECTOR forceV;
	forceV = XMLoadFloat3(&pObj1->pos) - XMLoadFloat3(&pObj2->pos);
	XMVECTOR r;
	r = XMVector3Length(forceV) ;
	forceV = XMVector3Normalize(forceV);
	Force F;
	XMFLOAT3 angle;

	//	Object 1
	XMStoreFloat3(&angle, XMVector3AngleBetweenVectors(forceV, -XMLoadFloat3(&pObj1->motion.speed)));
	if(D(angle.x) < 30)
	{
		XMVECTOR bufferForce;
		bufferForce = XMVector3Cross( XMLoadFloat3(&pObj1->motion.speed), forceV);
		bufferForce = XMVector3Cross(bufferForce, XMLoadFloat3(&pObj1->motion.speed));
		bufferForce = XMVector3Normalize(bufferForce);
		XMStoreFloat3(&F.direction, bufferForce / r / r);
	}
	else
	{
		XMVECTOR bufferForce;
		bufferForce = XMVector3Cross(XMLoadFloat3(&pObj1->motion.speed), forceV);
		bufferForce = XMVector3Cross(bufferForce, XMLoadFloat3(&pObj1->motion.speed));
		bufferForce = XMVector3Normalize(bufferForce);
		XMStoreFloat3(&F.direction, forceV / r / r/ 10);
	}
	pObj1->motion.forces.push_back(F);

	//	Object 2
	XMStoreFloat3(&angle, XMVector3AngleBetweenVectors(forceV, XMLoadFloat3(&pObj2->motion.speed)));
	if(D(angle.x) < 30)
	{
		XMVECTOR bufferForce;
		bufferForce = XMVector3Cross(XMLoadFloat3(&pObj2->motion.speed), -forceV);
		bufferForce = XMVector3Cross(bufferForce, XMLoadFloat3(&pObj2->motion.speed));
		bufferForce = XMVector3Normalize(bufferForce);
		XMStoreFloat3(&F.direction, bufferForce / r / r);
	}
	else
	{
		XMVECTOR bufferForce;
		bufferForce = XMVector3Cross(XMLoadFloat3(&pObj2->motion.speed), -forceV);
		bufferForce = XMVector3Cross(bufferForce, XMLoadFloat3(&pObj2->motion.speed));
		bufferForce = XMVector3Normalize(bufferForce);
		XMStoreFloat3(&F.direction, bufferForce / r / r / 10);
	}
	pObj2->motion.forces.push_back(F);

	return true;
}

bool Controller::checkBorderAndSetGravity(Object* pObj, SceneBorder border)
{
	if(pObj->motion.fixed)
		return false;

	if(border.type == SceneBorder::BORDER)
	{
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
		if(pObj->pos.y > border.yMax)
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
	}

	if(border.type == SceneBorder::PILLAR)
	{
		float forceSize = 0.1;
		float rPillar = (border.xMax - border.xMin) / 2;
		float xCenter = (border.xMax + border.xMin) / 2;
		float zCenter = (border.zMax + border.zMin) / 2;

		Force F;
		float r = Distance(pObj->pos.x, pObj->pos.z, xCenter, zCenter);

		if(r > rPillar + pObj->pModel->collision.x*3)
			return false;
		if(pObj->pos.y > border.yMax + pObj->pModel->collision.x * 3)
			return false;
		
		if(pObj->pos.y > border.yMax)
		{
			F.direction.y = 1*forceSize;
			pObj->motion.forces.push_back(F);
		}
		else
		{
			F.direction.x = pObj->pos.x - xCenter;
			F.direction.z = pObj->pos.z - zCenter;
			XMVECTOR FV = XMVector3Normalize(XMLoadFloat3(&F.direction))*forceSize;
			XMStoreFloat3(&F.direction, FV);
			pObj->motion.forces.push_back(F);
		}
	}
	return true;
}

void Controller::center()
{
	for(int i = 0; i < pObjects->size(); i++)
	{
		if((*pObjects)[i]->motion.fixed)
			continue;
		setCenterForce((*pObjects)[i]);
	}
}

bool Controller::setCenterForce(Object* pObj)
{
	if(pObj->motion.fixed)
		return false;

	XMVECTOR allForce = XMVectorSet(0,0,0,0);
	for(int i = 0; i < pObj->motion.forces.size(); i++)
	{
		allForce += XMLoadFloat3(&pObj->motion.forces[i].direction);
	}

	float r = Distance(pObj->pos, objectCenter);
	bool hasCenterForce = true;

	XMVECTOR centerForceUnit = XMVector3Normalize(XMLoadFloat3(&objectCenter) - XMLoadFloat3(&pObj->pos));
	XMFLOAT3 angle;
	
	XMStoreFloat3(&angle, XMVector3AngleBetweenVectors(XMLoadFloat3(&pObj->motion.speed), centerForceUnit));
	if(D(angle.x) < 70)
		hasCenterForce = false;
	

	for(int i = 0; i < pObj->motion.forces.size(); i++)
	{
		XMStoreFloat3(&angle, XMVector3AngleBetweenVectors(allForce, XMLoadFloat3(&pObj->motion.forces[i].direction)));
		if(D(angle.x) > 90)
			hasCenterForce = false;
	}

	centerForceUnit *= r*0.01;
	Force centerForce;
	XMStoreFloat3(&centerForce.direction, centerForceUnit);
	if(hasCenterForce)
		pObj->motion.forces.push_back(centerForce);
	return true;
}
