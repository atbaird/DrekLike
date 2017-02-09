#include "Game/Pathfinding/AStarAlgo.hpp"
#include "Engine/Renderer/Renderer.hpp"


//---------------------------------------
//AStarStepScore
AStarNode::AStarNode(const IntVector2& pos, const float& hCost, const float& gLocalCost, AStarNode* parent)
	: stepPos(pos),
	m_parent(parent),
	g_total_cost(gLocalCost),
	h_total_cost(hCost),
	f_total_cost(0.f)
{
	RecalcAStarVals(gLocalCost, parent);
}
void AStarNode::RecalcAStarVals(const float& gLocalCost, AStarNode* newParent)
{
	//parent
	m_parent = nullptr;
	m_parent = newParent;

	//calculate G
	if (m_parent != nullptr)
	{
		g_total_cost = gLocalCost + m_parent->g_total_cost;
	}
	else
	{
		g_total_cost = gLocalCost;
	}

	//Calculate F
	f_total_cost = g_total_cost + h_total_cost;
}
const bool BeginAndEndCoords::operator<(const BeginAndEndCoords& other) const
{
	if (begin < other.begin && end < other.end)
	{
		return true;
	}
	return false;
}


//-----------------------------------------
//AStarAlgo
AStarAlgo* g_algoForViewAll = nullptr;

AStarAlgo::AStarAlgo(const IntVector2& start, const IntVector2& end)
	:m_start(start),
	m_end(end),
	m_pathFound(false)
{

}
AStarAlgo::~AStarAlgo()
{
	ClearLists();
}

//----------------------------------------
//Run algorithm

void AStarAlgo::CalculateAStarAlgo(Map* map, const IntVector2& start, const IntVector2& end, const int& mask, const int& stepLimit)
{
	m_currentlyRunning = true;
	m_pathFound = false;
	m_start = start;
	m_end = end;
	Restart(map, start, end);
	for (int i = 0; i < stepLimit && m_pathFound == false; i++)
	{
		bool foundAPath = CalculateAStarAlgoInternal(map, mask);
		if (foundAPath == true)
		{
			break;
		}
	}

	m_currentlyRunning = false;
}

void AStarAlgo::SetStartAndEnd(const IntVector2& start, const IntVector2& end)
{
	m_start = start;
	m_end = end;
	if (m_pathFound)
	{
		ClearWithoutDeletingPath();
	}
	else
	{
		ClearLists();
	}
}

const bool AStarAlgo::CalculateAStarAlgoInternal(Map* map, const int& mask)
{
	if (m_pathFound)
	{
		return true;
	}
	if (map == nullptr || m_openList.empty() == true)
	{
		return false;
	}
	AStarNode* activeNode = FindLowestFNodeOnOpenListAndRemoveIt();
	AddToClosedList(activeNode);
	if (activeNode->stepPos == m_end)
	{
		 m_pathFound = RecursivelyBuildPathToStartFromNode(activeNode);
		 return true;
	}

	//Go through valid neighbors to find costs of moving towards those nodes.
	std::vector<IntVector2> positions = map->FindAdjacentValidPositionsThatMeetMask(activeNode->stepPos, mask);
	for (size_t i = 0; i < positions.size(); i++)
	{
		IntVector2 position = positions.at(i);
		if (IsPositionOnClosedList(position))
		{
			continue;
		}
		IntVector2 diffAbs = IntVector2(abs(activeNode->stepPos.x - position.x), abs(activeNode->stepPos.y - position.y));
		float localG = map->ComputeLocalG(position, activeNode->stepPos, mask);
		float parentG = activeNode->g_total_cost;
		if (localG == FLT_MAX)
		{
			continue;
		}
		float h = map->ComputeEstimatedHCostFromPosToPos(position, m_end);
		AStarNode* existingNode = FindOpenNodeWithPositionOnOpenList(position);
		if (existingNode != nullptr)
		{
			if ((localG + parentG) < existingNode->g_total_cost)
			{
				UpdateNodeValues(existingNode, localG, activeNode);
				continue;
			}
		}
		AStarNode* newNode = new AStarNode(position, h, localG, activeNode);
		AddNodeToOpenList(newNode);
		
	}

	return false;
}

//---------------------------------------------------
//setters
void AStarAlgo::ClearLists()
{
	for (size_t i = 0; i < m_closedList.size(); i++)
	{
		if (m_closedList.at(i) != nullptr)
		{
			delete m_closedList.at(i);
			m_closedList[i] = nullptr;
		}
	}
	for (size_t i = 0; i < m_openList.size(); i++)
	{
		if (m_openList.at(i) != nullptr)
		{
			delete m_openList.at(i);
			m_openList[i] = nullptr;
		}
	}
	m_closedList.clear();
	m_openList.clear();
}
void AStarAlgo::Restart(Map* map, const IntVector2& start, const IntVector2& end)
{
	ClearLists();
	m_start = start;
	m_end = end;
	m_pathFound = false;
	if (map == nullptr)
	{
		return;
	}
	if(m_openList.empty())
	{
		float h = map->ComputeEstimatedHCostFromPosToPos(m_start, m_end);
		AStarNode* startNode = new AStarNode(m_start, h, 0.0f, nullptr);
		AddNodeToOpenList(startNode);
	}
}

//---------------------------------------------------
//render
void InsertQuad(std::vector<Vertex_PCT>& verts, std::vector<Vertex_PCT>& lines, const Vector3& blPos, const Vector3& trPos, const Rgba& color)
{
	verts.push_back(Vertex_PCT(blPos, color));
	verts.push_back(Vertex_PCT(Vector3(trPos.x, blPos.y), color));
	verts.push_back(Vertex_PCT(trPos, color));
	verts.push_back(Vertex_PCT(Vector3(blPos.x, trPos.y), color));


	lines.push_back(Vertex_PCT(blPos, Rgba(1.f, 1.f, 1.f, 1.f)));
	lines.push_back(Vertex_PCT(Vector3(trPos.x, blPos.y), Rgba(1.f, 1.f, 1.f, 1.f)));

	lines.push_back(Vertex_PCT(Vector3(trPos.x, blPos.y), Rgba(1.f, 1.f, 1.f, 1.f)));
	lines.push_back(Vertex_PCT(trPos, Rgba(1.f, 1.f, 1.f, 1.f)));

	lines.push_back(Vertex_PCT(trPos, Rgba(1.f, 1.f, 1.f, 1.f)));
	lines.push_back(Vertex_PCT(Vector3(blPos.x, trPos.y), Rgba(1.f, 1.f, 1.f, 1.f)));

	lines.push_back(Vertex_PCT(Vector3(blPos.x, trPos.y), Rgba(1.f, 1.f, 1.f, 1.f)));
	lines.push_back(Vertex_PCT(blPos, Rgba(1.f, 1.f, 1.f, 1.f)));

	/*

	Vector2 realTileLocation = Vector2(tileCoord.x * m_TileRenderSize.x, tileCoord.y * m_TileRenderSize.y);
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y, 0.f), colorToRender, bl));
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y, 0.f), colorToRender, br));
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y + m_TileRenderSize.y, 0.f), colorToRender, tr));
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y + m_TileRenderSize.y, 0.f), colorToRender, tl));
	*/
}
void AStarAlgo::Render(const Vector2& renderSize) const
{
	if (g_Renderer == nullptr)
	{
		return;
	}

	std::vector<IntVector2> m_aStarOpenList = GetOpenListAsTileCoordsOnly();
	std::vector<IntVector2> m_aStarClosedList = GetClosedListAsTileCoordsOnly();

	std::vector<Vertex_PCT> verticesForPath;
	std::vector<Vertex_PCT> verticesForLines;

	//open list
	for (size_t i = 0; i < m_aStarOpenList.size(); i++)
	{
		IntVector2 tilePos = m_aStarOpenList.at(i);
		Vector3 bl = Vector3((float)tilePos.x * renderSize.x, (float)tilePos.y * renderSize.y);
		Vector3 tr = bl + Vector3(renderSize.x, renderSize.y);
		InsertQuad(verticesForPath, verticesForLines, bl, tr, Rgba(0.f, 0.f, 1.f, 1.f));
	}

	//closed list
	for (size_t i = 0; i < m_aStarClosedList.size(); i++)
	{
		IntVector2 tilePos = m_aStarClosedList.at(i);
		Vector3 bl = Vector3((float)tilePos.x * renderSize.x, (float)tilePos.y * renderSize.y);
		Vector3 tr = bl + Vector3(renderSize.x, renderSize.y);
		InsertQuad(verticesForPath, verticesForLines, bl, tr, Rgba(1.f, 0.f, 0.f, 1.f));
	}

	////start
	//Vector3 blStart = Vector3((float)m_start.x * renderSize.x, (float)m_start.y * renderSize.y);
	//Vector3 trStart = blStart + Vector3(renderSize.x, renderSize.y);
	//InsertQuad(verticesForPath, verticesForLines, blStart, trStart, Rgba(0.5f, 0.5f, 1.f, 1.f));

	//end
	Vector3 blEnd = Vector3((float)m_end.x * renderSize.x, (float)m_end.y * renderSize.y);
	Vector3 trEnd = blEnd + Vector3(renderSize.x, renderSize.y);
	InsertQuad(verticesForPath, verticesForLines, blEnd, trEnd, Rgba(0.f, 1.f, 0.f, 1.f));

	//render
	g_Renderer->DrawVertexArray(verticesForPath, PRIMITIVE_QUADS, nullptr);
	g_Renderer->DrawVertexArray(verticesForLines, PRIMITIVE_LINES, nullptr);
}


//-----------------------------------------
//getters
const IntVector2 AStarAlgo::GetNextPositionFromClosedListForPosition(const IntVector2& pos, bool& existsOnTheList)
{
	//if (pos == m_end)
	//{
	//	existsOnTheList = true;
	//	return m_end;
	//}
	const bool fal = false;
	const bool tru = true;
	existsOnTheList = fal;
	IntVector2 pos1 = IntVector2(-1, -1);
	for (size_t i = 0; i < m_closedList.size(); i++)
	{
		AStarNode* node = m_closedList.at(i);
		if (existsOnTheList == tru)
		{
			pos1 = node->stepPos;
			break;
		}
		if (node->stepPos == pos)
		{
			existsOnTheList = tru;
		}
	}
	if (pos1 == IntVector2(-1, -1))
	{
		existsOnTheList = false;
	}

	return pos1;
}
const std::vector<IntVector2> AStarAlgo::GetClosedListAsTileCoordsOnly() const
{
	std::vector<IntVector2> list;
	for (size_t i = 0; i < m_closedList.size(); i++)
	{
		list.push_back(m_closedList.at(i)->stepPos);
	}
	return list;
}
const std::vector<IntVector2> AStarAlgo::GetOpenListAsTileCoordsOnly() const
{
	std::vector<IntVector2> list;
	for (size_t i = 0; i < m_openList.size(); i++)
	{
		list.push_back(m_openList.at(i)->stepPos);
	}
	return list;
}
const std::vector<AStarNode*> AStarAlgo::GetOpenList() const
{
	return m_openList;
}
const std::vector<AStarNode*> AStarAlgo::GetClosedList() const
{
	return m_closedList;
}

AStarNode* AStarAlgo::FindOpenNodeWithPositionOnOpenList(const IntVector2& position)
{
	for (size_t i = 0; i < m_openList.size(); i++)
	{
		if (m_openList.at(i) != nullptr && m_openList.at(i)->stepPos == position)
		{
			return m_openList.at(i);
		}
	}
	return nullptr;
}
const bool AStarAlgo::IsPositionOnClosedList(const IntVector2& position) const
{
	if (m_closedList.size() == 0)
	{
		return false;
	}
	for (size_t i = 0; i < m_closedList.size(); i++)
	{
		if (m_closedList.at(i) != nullptr && m_closedList.at(i)->stepPos == position)
		{
			return true;
		}
	}
	return false;
}

//Adders
const bool AStarAlgo::AddNodeToOpenList(AStarNode* node)
{
	if (node == nullptr)
	{
		return false;
	}
	m_openList.push_back(node);

	return true;
}

//operators
AStarNode* AStarAlgo::FindLowestFNodeOnOpenListAndRemoveIt()
{
	if (m_openList.size() == 0)
	{
		return nullptr;
	}
	AStarNode* lowestFNode = nullptr;
	float lowestF = 0.f;
	int index = -1;
	bool enteredOnce = false;
	for (int i = 0; i < (int)m_openList.size(); i++)
	{
		if (m_openList.at(i) != nullptr && (enteredOnce == false || (lowestF < m_openList.at(i)->f_total_cost)))
		{
			lowestFNode = nullptr;
			lowestFNode = m_openList.at(i);
			lowestF = m_openList.at(i)->f_total_cost;
			index = i;
			enteredOnce = true;
		}
	}
	if (lowestFNode != nullptr)
	{
		m_openList.erase(m_openList.begin() + index);
	}

	return lowestFNode;
}
void AStarAlgo::AddToClosedList(AStarNode* activeNode)
{
	if (activeNode == nullptr)
	{
		return;
	}
	if (m_pathFound == false)
	{
		m_closedList.push_back(activeNode);
	}
	else
	{
		delete activeNode;
	}
}
const bool AStarAlgo::RecursivelyBuildPathToStartFromNode(AStarNode* activeNode)
{
	if (activeNode == nullptr || activeNode->stepPos != m_end)
	{
		return false;
	}
	AStarNode* cur = activeNode;

	while (1)
	{
		if (cur->m_parent == nullptr)
		{
			if (cur->stepPos != m_start)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{

			IntVector2 diff = cur->stepPos - cur->m_parent->stepPos;
			IntVector2 absDiff = IntVector2(abs(diff.x), abs(diff.y));

			if (absDiff.x > 1 || absDiff.y > 1)
			{
				return false;
			}
			else
			{
				AStarNode* par = cur->m_parent;
				cur = nullptr;
				cur = par;
			}
		}
	}
	return false;
}

void AStarAlgo::UpdateNodeValues(AStarNode* existingNode, const float& localG, AStarNode* parentNode)
{
	existingNode->RecalcAStarVals(localG, parentNode);
}
void AStarAlgo::ClearWithoutDeletingPath()
{
	m_openList.clear();
	m_closedList.clear();
	m_pathFound = false;
}