#include "controller.h"
using namespace DirectX;
void Controller::next(std::vector<Object*>* pObjects)
{
	this->pObjects = pObjects;
	collision();
	speed();
	move();
}

void Controller::collision()
{
	Object* current;
	for(int i = 0; i < pObjects->size(); i++)
	{
		current = (*pObjects)[i];
		if(current->Flag==Object::GROUND)continue;
		bool hasUserForce = false;
		if(i==1)
		for(int j = 0; j < (*pObjects)[1]->motion.forces.size(); j++)
		{
			if((*pObjects)[1]->motion.forces[j].Flag == Force::USER)
			{
				userForce = (*pObjects)[1]->motion.forces[j];
				hasUserForce = true;
			}
		}

		current->motion.forces.clear();
		for(int j = 0; j < pObjects->size(); j++)
		{
			if(i == j)continue;
			checkCollisionAndSetForce((*pObjects)[i], (*pObjects)[j]);
		}

		if(hasUserForce)
		{
			(*pObjects)[1]->motion.forces.push_back(userForce);
		}
	}
}

void Controller::speed()
{
	for(int i = 0; i < pObjects->size(); i++)
	{
		Object* pObject = (*pObjects)[i];
		if(pObject->Flag == Object::GROUND)continue;

		XMVECTOR forceCollision = XMVectorSet(0, 0, 0, 0);
		XMVECTOR forceSuspend = XMVectorSet(0, 0, 0, 0);
		XMVECTOR currentForce;

		for(int j = 0; j < pObject->motion.forces.size(); j++)
		{
			currentForce = XMLoadFloat3(&pObject->motion.forces[j].direction);
			if(pObject->motion.forces[j].Flag == Force::SUSPEND)
				forceSuspend += currentForce;
			else
				forceCollision += currentForce;
		}

		XMFLOAT3 forceSuspendF3;
		XMStoreFloat3(&forceSuspendF3, forceSuspend);

		if(forceSuspendF3.y < 0.5)
			forceCollision += XMLoadFloat3(&pObject->motion.gravity);

		forceCollision /= pObject->motion.mess;
		forceCollision += XMLoadFloat3(&pObject->motion.speed);

		XMStoreFloat3(&pObject->motion.speed, forceCollision);

		//pObject->motion.speed.x *= 0.99;
		//pObject->motion.speed.y *= 0.99;
		//pObject->motion.speed.z *= 0.99;

		//pObject->motion.rotation.x *= 0.999;
		//pObject->motion.rotation.y *= 0.999;
		//pObject->motion.rotation.z *= 0.999;
	}
}

void Controller::move()
{
	Object* pObject;
	for(int i = 0; i < pObjects->size(); i++)
	{
		pObject = (*pObjects)[i];
		if(pObject->Flag == Object::GROUND)continue;

		pObject->pos.x += pObject->motion.speed.x;
		if(pObject->motion.speed.y>0.02|| pObject->motion.speed.y<-0.02)
			pObject->pos.y += pObject->motion.speed.y;
		pObject->pos.z += pObject->motion.speed.z;

		float rotationMul = 30;
		pObject->angle.x += pObject->motion.rotation.x*rotationMul;
		pObject->angle.y += pObject->motion.rotation.y*rotationMul;
		pObject->angle.z += pObject->motion.rotation.z*rotationMul;
	}
}

float Distance(DirectX::XMFLOAT3 f1, DirectX::XMFLOAT3 f2)
{
	return sqrt((f1.x - f2.x) * (f1.x - f2.x) +
		(f1.y - f2.y) * (f1.y - f2.y) +
		(f1.z - f2.z) * (f1.z - f2.z));
}

bool Controller::checkCollisionAndSetForce(Object* pObj1, Object* pObj2)
{
	/*
	Force force;
	float reflect = 0.99;
	if(pObj2->Flag != Object::GROUND)
	{
		
		if(pObj1->pMesh->collision.type == CollisionBlock::SPHERE && pObj2->pMesh->collision.type == CollisionBlock::SPHERE)
		{
			float distance = Distance(pObj1->pos,pObj2->pos);
			if(distance > pObj1->pMesh->collision.x + pObj2->pMesh->collision.x+0.0001)
				return false;

			XMVECTOR forceDirection = XMVector3Normalize(XMLoadFloat3(&pObj1->pos) - XMLoadFloat3(&pObj2->pos));
			XMVECTOR objSpeed = -XMLoadFloat3(&pObj1->motion.speed);
			XMVECTOR OutputForce = XMVector3Dot(objSpeed, forceDirection)*forceDirection*pObj1->motion.mess*reflect;
			XMStoreFloat3(&force.direction,OutputForce);
			force.Flag = Force::COLLISION;
			pObj1->motion.forces.push_back(force);

			forceDirection *= (pObj1->pMesh->collision.x + pObj2->pMesh->collision.x - distance) * 0.1;
			XMStoreFloat3(&force.direction, forceDirection);
			force.Flag = Force::COLLISION;
			pObj1->motion.forces.push_back(force);

			objSpeed = XMLoadFloat3(&pObj2->motion.speed);
			OutputForce = XMVector3Dot(objSpeed, forceDirection)*forceDirection*pObj2->motion.mess*reflect;
			XMStoreFloat3(&force.direction, OutputForce);
			force.Flag = Force::COLLISION;
			pObj1->motion.forces.push_back(force);

			//rotation
			pObj1->motion.rotation.x = pObj1->motion.rotation.y = pObj1->motion.rotation.z = 0;
			if(pObj2->pos.y - pObj1->pos.y > 0)
			{
				pObj1->motion.rotation.z = pObj1->motion.speed.x;
				pObj1->motion.rotation.x = -pObj1->motion.speed.z;
			}
			else
			{
				pObj1->motion.rotation.z = -pObj1->motion.speed.x;
				pObj1->motion.rotation.x = pObj1->motion.speed.z;
			}

			if(pObj2->pos.x - pObj1->pos.x < 0)
			{
				pObj1->motion.rotation.y = -pObj1->motion.speed.z;
				pObj1->motion.rotation.z = -pObj1->motion.speed.y;
			}
			else
			{
				pObj1->motion.rotation.y = pObj1->motion.speed.z;
				pObj1->motion.rotation.z = pObj1->motion.speed.y;
			}

			if(pObj2->pos.z - pObj1->pos.z > 0)
			{
				pObj1->motion.rotation.y = -pObj1->motion.speed.x;
				pObj1->motion.rotation.x = pObj1->motion.speed.y;
			}
			else
			{
				pObj1->motion.rotation.y = pObj1->motion.speed.x;
				pObj1->motion.rotation.x = -pObj1->motion.speed.y;
			}
		}
		
	}
	else
	{
		if(pObj1->pos.y - pObj2->pos.y > pObj1->pMesh->collision.x)
			return false;

		if(pObj1->motion.speed.y >= 0)
			return false;

		force.direction.y = -pObj1->motion.speed.y*pObj1->motion.mess*reflect * 2;
		force.Flag = Force::COLLISION;
		pObj1->motion.forces.push_back(force);

		force.direction.y = 1;
		force.Flag = Force::SUSPEND;
		pObj1->motion.forces.push_back(force);

		//rotation
		pObj1->motion.rotation.x = pObj1->motion.rotation.y = pObj1->motion.rotation.z = 0;
		{
			pObj1->motion.rotation.z = -pObj1->motion.speed.x;
			pObj1->motion.rotation.x = pObj1->motion.speed.z;
		}

	}
	
	if(pObj1->pos.x > 10 && pObj1->motion.speed.x>0)
	{
		force.direction.x = force.direction.y = force.direction.z = 0;
		force.direction.x = -pObj1->motion.speed.x*pObj1->motion.mess*reflect * 2;
		force.Flag = Force::COLLISION;
		pObj1->motion.forces.push_back(force);
	}

	if(pObj1->pos.x < -10 && pObj1->motion.speed.x<0)
	{
		force.direction.x = force.direction.y = force.direction.z = 0;
		force.direction.x = -pObj1->motion.speed.x*pObj1->motion.mess*reflect * 2;
		force.Flag = Force::COLLISION;
		pObj1->motion.forces.push_back(force);
	}

	if(pObj1->pos.z > 10 && pObj1->motion.speed.z>0)
	{
		force.direction.x = force.direction.y = force.direction.z = 0;
		force.direction.z = -pObj1->motion.speed.z*pObj1->motion.mess*reflect * 2;
		force.Flag = Force::COLLISION;
		pObj1->motion.forces.push_back(force);
	}

	if(pObj1->pos.z < -10 && pObj1->motion.speed.z<0)
	{
		force.direction.x = force.direction.y = force.direction.z = 0;
		force.direction.z = -pObj1->motion.speed.z*pObj1->motion.mess*reflect * 2;
		force.Flag = Force::COLLISION;
		pObj1->motion.forces.push_back(force);
	}
	*/
	return true;
}