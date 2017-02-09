#include "Game/Generators/MapGenerators/CavernMap/PerlinWorm.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/ExtendedMath.hpp"


const float PerlinWorm::minDistanceToTravel = 3.f;
const float PerlinWorm::addDistanceToTravel = 3.f;
const float PerlinWorm::yawChangeRange = 180.f;

PerlinWorm::PerlinWorm(IntVector2 startPoint, float startYaw)
	: CurrentPoint(startPoint),
	Yaw(startYaw)
{

}
PerlinWorm::~PerlinWorm()
{

}

const IntVector2 PerlinWorm::GetNextPoint()
{
	Vector2 direction = Vector2(CosDegrees(Yaw), SinDegrees(Yaw));
	direction.normalize();
	float distance = minDistanceToTravel + (float)(rand() % (int)addDistanceToTravel);
	Vector2 distCoord = (direction * distance);
	CurrentPoint += IntVector2((int)floor(distCoord.x), (int)floor(distCoord.y));
	Yaw += (1.f / (float)((rand() % 100) + 1)) * yawChangeRange;
	return CurrentPoint;
}
