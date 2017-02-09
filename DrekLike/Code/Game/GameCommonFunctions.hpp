#pragma once

#ifndef GAMECOMMONFUNCTIONS_HPP
#define GAMECOMMONFUNCTIONS_HPP
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include <vector>
#include <string>

extern std::vector<std::string> s_messageBoxStrings;

struct LineTraceStruct
{
	Vector2 fStart = Vector2(0.f,0.f);
	Vector2 fEnd = Vector2(0.f,0.f);
	Vector2 rayDisplacement = Vector2(0.f, 0.f);
	Vector2 tDelta = Vector2(0.f, 0.f);
	IntVector2 tileStep = IntVector2(0, 0);
	IntVector2 offsetToLeadingEdge = IntVector2(0, 0);
	Vector2 firstVerticalIntersection = Vector2(0.f, 0.f);
	Vector2 tOfNextCrossing = Vector2(0.f, 0.f);
};

const LineTraceStruct LoadLineTraceStruct(const IntVector2& startTile, const IntVector2& endTile);
const Vector2 CalcDistanceBetweenTwoTiles(const IntVector2& startTile, const IntVector2& endTile);
const Vector2 CalculateShortestDistanceToTilePointAlongTileLine(const IntVector2& point, const IntVector2& lineStart, const IntVector2& lineEnd, 
	const float& lineRadius, IntVector2& endTargetPosition);
const bool CircleTest(const IntVector2& current, const IntVector2& testingAgainst, const float& radius);


#endif