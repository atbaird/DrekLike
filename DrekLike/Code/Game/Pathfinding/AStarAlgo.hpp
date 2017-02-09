#pragma once

#ifndef ASTARALGO_HPP
#define ASTARALGO_HPP
#include "Engine/Math/IntVector2.hpp"
#include "Game/Map/Map.hpp"
#include <vector>

//built in reference to: https://www.youtube.com/watch?v=eTx6HQ9Veas

class AStarAlgo;

extern AStarAlgo* g_algoForViewAll;

struct AStarNode
{
	AStarNode(const IntVector2& pos, const float& hCost, const float& gLocalCost, AStarNode* parent = nullptr);
	void RecalcAStarVals(const float& gLocalCost, AStarNode* newParent);

	AStarNode* m_parent;
	IntVector2 stepPos;
	float g_total_cost;
	float h_total_cost;
	float f_total_cost;
};
struct BeginAndEndCoords
{
	IntVector2 begin;
	IntVector2 end;
	const bool operator<(const BeginAndEndCoords& other) const;
};
struct SavedPath
{
	std::vector<AStarNode*> openList;
	std::vector<AStarNode*> closedList;
};

class AStarAlgo
{
private:
	//variables
	std::vector<AStarNode*> m_openList;
	std::vector<AStarNode*> m_closedList;
	IntVector2 m_start;
	IntVector2 m_end;
	bool m_currentlyRunning;
	bool m_pathFound;

public:
	AStarAlgo(const IntVector2& start = IntVector2(0,0), const IntVector2& end = IntVector2(1,1));
	~AStarAlgo();
	void CalculateAStarAlgo(Map* map, const IntVector2& start, const IntVector2& end, const int& mask, const int& stepLimit = 100);
	void SetStartAndEnd(const IntVector2& start, const IntVector2& end);
	const bool CalculateAStarAlgoInternal(Map* map, const int& mask);

	//setters
	void ClearLists();
	void Restart(Map* map, const IntVector2& start, const IntVector2& end);

	//render
	void Render(const Vector2& renderSize) const;

	//Getters
	const IntVector2 GetNextPositionFromClosedListForPosition(const IntVector2& pos, bool& exists);
	const std::vector<IntVector2> GetClosedListAsTileCoordsOnly() const;
	const std::vector<IntVector2> GetOpenListAsTileCoordsOnly() const;
	const std::vector<AStarNode*> GetOpenList() const;
	const std::vector<AStarNode*> GetClosedList() const;
	const IntVector2 GetStart() const { return m_start; };
	const IntVector2 GetEnd() const { return m_end; };
	const bool GetCurrentlyRunning() const { return m_currentlyRunning; };
	const bool GetPathFound() const { return m_pathFound; };

private:
	AStarNode* FindOpenNodeWithPositionOnOpenList(const IntVector2& position);
	const bool IsPositionOnClosedList(const IntVector2& position) const;
	void ClearWithoutDeletingPath();

	//adders
	const bool AddNodeToOpenList(AStarNode* node);

	//operators
	AStarNode* FindLowestFNodeOnOpenListAndRemoveIt();
	void AddToClosedList(AStarNode* activeNode);
	const bool RecursivelyBuildPathToStartFromNode(AStarNode* activeNode);
	void UpdateNodeValues(AStarNode* existingNode, const float& localG, AStarNode* parentNode);
};
#endif