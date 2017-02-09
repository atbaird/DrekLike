#include "Game/GameCommonFunctions.hpp"
#include <cmath>

std::vector<std::string> s_messageBoxStrings;

const LineTraceStruct LoadLineTraceStruct(const IntVector2& startTile, const IntVector2& endTile)
{
	LineTraceStruct toReturn;

	toReturn.fStart = Vector2((float)startTile.x, (float)startTile.y);
	toReturn.fEnd = Vector2((float)endTile.x, (float)endTile.y);
	toReturn.rayDisplacement = toReturn.fEnd - toReturn.fStart;
	toReturn.tDelta = Vector2(0.f, 0.f);
	toReturn.tileStep = IntVector2(0, 0);
	toReturn.offsetToLeadingEdge = IntVector2(0, 0);
	toReturn.firstVerticalIntersection = Vector2(0.f, 0.f);
	toReturn.tOfNextCrossing = Vector2(0.f, 0.f);

	toReturn.tDelta.x = 1.f / toReturn.rayDisplacement.x;
	toReturn.tDelta.y = 1.f / toReturn.rayDisplacement.y;
	toReturn.tileStep.x = 1;
	toReturn.tileStep.y = 1;
	if (toReturn.rayDisplacement.x < 0.f)
	{
		toReturn.tileStep.x = -1;
	}
	if (toReturn.rayDisplacement.y < 0.f)
	{
		toReturn.tileStep.y = -1;
	}
	toReturn.offsetToLeadingEdge.x = (toReturn.tileStep.x + 1) / 2;
	toReturn.offsetToLeadingEdge.y = (toReturn.tileStep.y + 1) / 2;
	toReturn.firstVerticalIntersection.x = (float)(startTile.x + toReturn.offsetToLeadingEdge.x);
	toReturn.firstVerticalIntersection.y = (float)(startTile.y + toReturn.offsetToLeadingEdge.y);
	toReturn.tOfNextCrossing.x = abs(toReturn.firstVerticalIntersection.x - toReturn.fStart.x) * toReturn.tDelta.x;
	toReturn.tOfNextCrossing.y = abs(toReturn.firstVerticalIntersection.y - toReturn.fStart.y) * toReturn.tDelta.y;

	return toReturn;
}
const Vector2 CalcDistanceBetweenTwoTiles(const IntVector2& startTile, const IntVector2& endTile)
{
	Vector2 distance = Vector2((float)endTile.x, (float)endTile.y) - Vector2((float)startTile.x, (float)startTile.y);
	return distance;
}
const Vector2 CalculateShortestDistanceToTilePointAlongTileLine(const IntVector2& point, const IntVector2& lineStart, const IntVector2& lineEnd, 
	const float& lineRadius, IntVector2& endTarget)
{
	bool setOnce = false;
	Vector2 shortestDistance;
	LineTraceStruct lineTrace = LoadLineTraceStruct(lineStart, lineEnd);

	IntVector2 tCurrent = lineStart;
	while (tCurrent != lineEnd)
	{
		if (abs(lineTrace.tOfNextCrossing.x < abs(lineTrace.tOfNextCrossing.y)))
		{
			if (abs(lineTrace.tOfNextCrossing.x) > 1.0f)
			{
				break;
			}
			tCurrent.x += lineTrace.tileStep.x;
			Vector2 distance = CalcDistanceBetweenTwoTiles(tCurrent, point);
			Vector2 normDist = distance;
			normDist.normalize();
			Vector2 lineRaVari = normDist * lineRadius;
			distance = distance - lineRaVari;
			if (setOnce == false || (abs(distance.GetLength()) < abs(shortestDistance.GetLength())))
			{
				shortestDistance = distance;
				endTarget = IntVector2(point.x + (int)ceil(distance.x), point.y + (int)ceil(distance.y));
			}
			lineTrace.tOfNextCrossing.x += lineTrace.tDelta.x;
		}
		else
		{
			if (abs(lineTrace.tOfNextCrossing.y) > 1.0f)
			{
				break;
			}
			tCurrent.y += lineTrace.tileStep.y;
			Vector2 distance = CalcDistanceBetweenTwoTiles(tCurrent, point);
			Vector2 normDist = distance;
			normDist.normalize();
			Vector2 lineRaVari = normDist * lineRadius;
			distance = distance - lineRaVari;
			if (setOnce == false || (abs(distance.GetLength()) < abs(shortestDistance.GetLength())))
			{
				shortestDistance = distance;
				endTarget = IntVector2(point.x + (int)ceil(distance.x), point.y + (int)ceil(distance.y));
			}
			lineTrace.tOfNextCrossing.y += lineTrace.tDelta.y;
		}
	}
	return shortestDistance;
}

const bool CircleTest(const IntVector2& current, const IntVector2& testingAgainst, const float& radius)
{
	Vector2 fCur = Vector2((float)current.x, (float)current.y);
	Vector2 fTest = Vector2((float)testingAgainst.x, (float)testingAgainst.y);
	Vector2 difference = fTest - fCur;
	difference.floorAll();
	float length = difference.GetLength();
	if (length <= radius)
	{
		return true;
	}
	return false;
}