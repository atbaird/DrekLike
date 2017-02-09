#include "Game/TheGame.hpp"
#include "Game/Generators/MapGenerators/MapGenerator.hpp"
#include "Game/FieldOfView/FieldOfViewFloodFill.hpp"
#include "Game/Items/ItemFactory.hpp"


//--------------------------------------------
TheGame* g_TheGame = nullptr;
const IntVector2 TheGame::s_centralMousePosition = IntVector2(800, 700);
const Vector2 TheGame::s_mouseYawPitchSensitivity = Vector2(0.022f, 0.022f);


//-------------------------------------------
//Generator Creators


//--------------------------------------------
TheGame::TheGame()
	: WindowWidth(16.f),
	WindowHeight(9.f),
	m_mainCamera(Vector3(1.f, -5.f, 1.f), EulerAngles(0.f, 0.f, 0.f), true),
	cameraVelocity(0.f, 0.f, 0.f),
	previousState(State_Invalid),
	currentState(State_StartUp),
	stateToChangeTo(State_Invalid),
	stateEnding(false),
	stateChanging(false),
	stateFinished(false),
	stateStarting(true),
	stepsBeforeCheckSpawn(5),
	stepsSinceLastCheckedSpawn(0),
	itemsToSpawn(20),
	npcsToSpawn(50),
	secondsSinceLastStateChange(0.f),
	timeSecondsSinceLastStep(0.f),
	timeSecondsBetweenStepsIfRealTime(1.f),
	m_mIsHeldDown(false),
	m_nIsHeldDown(false),
	m_qIsHeldDown(false),
	m_yIsHeldDown(false),
	m_iIsHeldDown(false),
	m_sIsHeldDown(false),
	m_pIsHeldDown(false),
	m_cIsHeldDown(false),
	m_gIsHeldDown(false),
	m_justSavedGame(false),
	m_playerAutoUsing(false),
	m_tabIsHeldDown(false),
	m_returnIsHeldDown(false),
	m_spaceIsHeldDown(false),
	m_escapeIsHeldDown(false),
	m_automaticGeneration(false),
	m_createdPlayerOnce(false),
	m_quitNow(false),
	m_font(nullptr),
	m_gameSpriteSheet(nullptr),
	m_player(nullptr),
	mapTypeSelected(0),
	simulationDelta(0.1f),
	YouAreDeadColor(1.f,1.f,1.f,0.f),
	m_playingBackgroundColor(0.f, 0.f, 0.f, 1.f)
{
	inventoryStartX = WindowWidth * (4.f / 5.f);
	InitializeTileDefinitions();
	m_WholeWindowBlackBoxFadeOnDeath.push_back(Vertex_PCT(Vector3(0.f, 0.f), Rgba(0.f, 0.f, 0.f, 0.f)));
	m_WholeWindowBlackBoxFadeOnDeath.push_back(Vertex_PCT(Vector3(WindowWidth, 0.f), Rgba(0.f, 0.f, 0.f, 0.f)));
	m_WholeWindowBlackBoxFadeOnDeath.push_back(Vertex_PCT(Vector3(WindowWidth, WindowHeight), Rgba(0.f, 0.f, 0.f, 0.f)));
	m_WholeWindowBlackBoxFadeOnDeath.push_back(Vertex_PCT(Vector3(0.f, WindowHeight), Rgba(0.f, 0.f, 0.f, 0.f)));


	startMessageBoxHeight = WindowHeight - m_messageBarHeight;

	m_messageBoxBackground.push_back(Vertex_PCT(Vector3(0.f, startMessageBoxHeight), Rgba(1.f, 1.f, 1.f, 1.f)));
	m_messageBoxBackground.push_back(Vertex_PCT(Vector3(WindowWidth, startMessageBoxHeight), Rgba(1.f, 1.f, 1.f, 1.f)));
	m_messageBoxBackground.push_back(Vertex_PCT(Vector3(WindowWidth, WindowHeight), Rgba(1.f, 1.f, 1.f, 1.f)));
	m_messageBoxBackground.push_back(Vertex_PCT(Vector3(0.f, WindowHeight), Rgba(1.f, 1.f, 1.f, 1.f)));
	s_messageBoxStrings.push_back("Press \'ESC\' to go to the Pause Menu.");
	g_algoForViewAll = new AStarAlgo();
	m_gameSpriteSheet = new SpriteSheet("Data/Images/Alexander_Baird_Custom_Font Sprite_Sheet.png", 8, 16);
	EnvironmentGenerationProcess::LoadAllBlueprintFiles();
	Generator::LoadAllFeatures();
	//EnvironmentGenerationProcess::LoadEnvironmentBlueprints();
	//EnvironmentGenerationProcess::LoadNPCFactoryBlueprints();
	MapGenerator::RegisterAllMapGenerators(numberOfMapGenerators);
	Generator::LoadAllNPCS();
	numberOfMapGenerators = EnvironmentGenerationProcess::GetGeneratorLocations().size();
}
TheGame::~TheGame()
{
	if (g_map != nullptr)
	{
		delete g_map;
		g_map = nullptr;
	}
	if (m_gameSpriteSheet != nullptr)
	{
		delete m_gameSpriteSheet;
		m_gameSpriteSheet = nullptr;
	}

	Agent::ClearAllAgents();
	if (m_player != nullptr)
	{
		delete m_player;
		m_player = nullptr;
	}
	if (g_algoForViewAll != nullptr)
	{
		delete g_algoForViewAll;
		g_algoForViewAll = nullptr;
	}
	Faction::ClearAllFactions();
	Item::ClearAllItems();
	GeneratorRegistration::ClearAllGeneratorRegistrations();
}
void TheGame::InitializeTileDefinitions()
{
	TileDefinition airTile = TileDefinition(TILE_AIR, Rgba(0.9f, 0.9f, 0.9f, 1.f), 'Z' + 4, true, false, false, false, false);
	TileDefinition stoneTile = TileDefinition(TILE_STONE, Rgba(0.2f, 0.2f, 0.2f, 1.f), '#', true, true, true, false, false);
	TileDefinition waterTile = TileDefinition(TILE_WATER, Rgba(0.f, 0.f, 1.f, 1.f), 'Z' + 34, true, false, false, true, false);
	TileDefinition lavaTile = TileDefinition(TILE_LAVA, Rgba(1.f, 0.657f, 0.f, 1.f), 'Z' + 34, true, false, false, true, true);
	TileDefinition::AddOrUpdateTileDefinition(airTile);
	TileDefinition::AddOrUpdateTileDefinition(stoneTile);
	TileDefinition::AddOrUpdateTileDefinition(waterTile);
	TileDefinition::AddOrUpdateTileDefinition(lavaTile);
}

void TheGame::SetFont(Font* font)
{
	m_font = nullptr;
	m_font = font;
}

void TheGame::CameraControls(const float& deltaSeconds)
{
	//Looking
	IntVector2 mousePosition;
	g_input->GetFirstCursorPosition(mousePosition);
	IntVector2 cursorDelta = mousePosition - s_centralMousePosition;
	g_input->setCursorPosition(s_centralMousePosition);
	g_input->setCursorShow(false);

	float camRoll = 0.f;
	float camYaw = m_mainCamera.m_orientation.m_zYaw - ((float)cursorDelta.x * s_mouseYawPitchSensitivity.x); //180.f + degreesIs
	float camPitch = m_mainCamera.m_orientation.m_yPitch + ((float)cursorDelta.y * s_mouseYawPitchSensitivity.y);

	m_mainCamera.m_orientation = EulerAngles(camRoll, camPitch, camYaw);
	m_mainCamera.FixAndClampAngles();

	//Movement
	XYZPlaneMovement(deltaSeconds);
}
void TheGame::XYZPlaneMovement(const float& deltaSeconds)
{
	float movementMultiplier = deltaSeconds;
	if (g_input->isKeyDown(VK_SHIFT) == true)
	{
		movementMultiplier *= 2.f;
	}
	cameraVelocity = Vector3(0.f, 0.f, 0.f);

	//forward and back
	if (g_input->isKeyDown('W') == true && g_input->isKeyDown('S') == false)
	{
		Vector3 forwardXY = m_mainCamera.GetForwardXY() * movementMultiplier;
		cameraVelocity += forwardXY;
	}
	else if (g_input->isKeyDown('S') == true && g_input->isKeyDown('W') == false)
	{
		Vector3 forwardXY = m_mainCamera.GetForwardXY() * movementMultiplier;
		cameraVelocity -= forwardXY;
	}

	//left and right
	if (g_input->isKeyDown('A') == true && g_input->isKeyDown('D') == false)
	{
		Vector3 leftXY = (m_mainCamera.GetLeftXY()) * movementMultiplier;
		cameraVelocity += leftXY;
	}
	else if (g_input->isKeyDown('D') == true && g_input->isKeyDown('A') == false)
	{
		Vector3 leftXY = (m_mainCamera.GetLeftXY()) * movementMultiplier;
		cameraVelocity -= leftXY;
	}

	//up and down
	if (g_input->isKeyDown('Z') == true && g_input->isKeyDown(VK_SPACE) == false)
	{
		cameraVelocity -= Vector3(0.f, 0.f, movementMultiplier);
	}
	else if (g_input->isKeyDown(VK_SPACE) == true && g_input->isKeyDown('Z') == false)
	{
		cameraVelocity += Vector3(0.f, 0.f, movementMultiplier);
	}
	m_mainCamera.m_position = m_mainCamera.m_position + cameraVelocity;
}
void TheGame::Update(const float& deltaSeconds, const bool& focus)
{
	m_focused = focus;
	switch (currentState)
	{
	case State_StartUp:
		StartUpUpdate(deltaSeconds);
		break;
	case State_MainMenu:
		MainMenuUpdate(deltaSeconds);
		break;
	case State_PickMap:
		PickMapUpdate(deltaSeconds);
		break;
	case State_Generation:
		GenerationUpdate(deltaSeconds);
		break;
	case State_Playing:
		PlayingUpdate(deltaSeconds);
		break;
	case State_Paused:
		PausedUpdate(deltaSeconds);
		break;
	case State_ConfirmExit:
		ConfirmExitUpdate(deltaSeconds);
		break;
	case State_Shutdown:
		ShutdownUpdate(deltaSeconds);
		break;
	case State_Continue:
		ContinueUpdate(deltaSeconds);
		break;
	default:
		break;
	}
	UpdateKeysHeldDown();
	if (stateChanging == true && stateFinished == true)
	{
		ChangeStates(stateToChangeTo);
		stateChanging = false;
		stateStarting = true;
	}

	if (g_map != nullptr)
	{
		std::vector<Agent*> visibleAgents;
		g_map->FinalizeMap(m_player, visibleAgents);

		Agent::SetAllAgentsInvisible();
		for (size_t i = 0; i < visibleAgents.size(); i++)
		{
			visibleAgents.at(i)->SetVisibleToPlayer(true);
		}
		for (size_t i = 0; i < Item::s_itemsVisibleToPlayer.size(); i++)
		{
			Item::s_itemsVisibleToPlayer.at(i)->SetVisibleToPlayer(true);
		}
	}
}
void TheGame::DrawDebugAxisLines() const
{
	g_Renderer->DepthTestingTrueFalse(true);
	g_Renderer->DrawGridLinesNEU(2.f, 2.f, 2.f, 3.f);
	g_Renderer->DepthTestingTrueFalse(false);
	g_Renderer->DrawGridLinesNEU(2.f, 2.f, 2.f, 1.f);
}
void TheGame::SetUp3DView() const
{
	float aspect = (16.f / 9.f);
	float fovDegreesVertical = 70.f;
	float zNear = 0.1f;
	float zFar = 1000.f;

	g_Renderer->SetPerspective(fovDegreesVertical, aspect, zNear, zFar);
}
void TheGame::SetUp2DView(const Rgba& screenColor) const
{
	g_Renderer->clearScreen(screenColor, true);
	g_Renderer->SetOrtho(Vector2(0.f, 0.f), Vector2(WindowWidth, WindowHeight));
}
void TheGame::Render() const
{
	switch (currentState)
	{
	case State_StartUp:
		StartUpRender();
		break;
	case State_MainMenu:
		MainMenuRender();
		break;
	case State_PickMap:
		PickMapRender();
		break;
	case State_Generation:
		GenerationRender();
		break;
	case State_Playing:
		PlayingRender();
		break;
	case State_Paused:
		PausedRender();
		break;
	case State_ConfirmExit:
		ConfirmExitRender();
		break;
	case State_Shutdown:
		ShutdownRender();
		break;
	case State_Continue:
		ContinueRender();
		break;
	default:
		break;
	}
}


bool TheGame::isQuitting()
{
	return m_quitNow;
}

void TheGame::ClearTheGame()
{
	if (g_map != nullptr)
	{
		delete g_map;
		g_map = nullptr;
	}
	if (m_player != nullptr)
	{
		delete m_player;
		m_player = nullptr;
		m_createdPlayerOnce = false;
		for (size_t i = 0; i < m_WholeWindowBlackBoxFadeOnDeath.size(); i++)
		{
			m_WholeWindowBlackBoxFadeOnDeath.at(i).m_color.Alpha = 0.f;
		}
	}
	Agent::ClearAllAgents();
	Item::ClearAllItems();
	Feature::DestroyAllFeatures();
	s_messageBoxStrings.clear();
	s_messageBoxStrings.push_back("Press \'ESC\' to go to the Pause Menu.");
}

//---------------------------------------------------------
//
void TheGame::UpdateKeysHeldDown()
{
	for (int i = 0; i < numberOfNumberKeys; i++)
	{
		if (numberKeysHeldDown[i] == true && g_input->isKeyDown(numberKeyValues[i]) == false)
		{
			numberKeysHeldDown[i] = false;
		}
	}
	if (m_iIsHeldDown == true && g_input->isKeyDown('I') == false)
	{
		m_iIsHeldDown = false;
	}
	if (m_gIsHeldDown == true && g_input->isKeyDown('G') == false)
	{
		m_gIsHeldDown = false;
	}
	if (m_mIsHeldDown == true && g_input->isKeyDown('M') == false)
	{
		m_mIsHeldDown = false;
	}
	if (m_nIsHeldDown == true && g_input->isKeyDown('N') == false)
	{
		m_nIsHeldDown = false;
	}
	if (m_qIsHeldDown == true && g_input->isKeyDown('Q') == false)
	{
		m_qIsHeldDown = false;
	}
	if (m_yIsHeldDown == true && g_input->isKeyDown('Y') == false)
	{
		m_yIsHeldDown = false;
	}
	if (m_sIsHeldDown == true && g_input->isKeyDown('S') == false)
	{
		m_sIsHeldDown = false;
	}
	if (m_escapeIsHeldDown == true && g_input->isKeyDown(VK_ESCAPE) == false)
	{
		m_escapeIsHeldDown = false;
	}
	if (m_spaceIsHeldDown == true && g_input->isKeyDown(VK_SPACE) == false)
	{
		m_spaceIsHeldDown = false;
	}
	if (m_returnIsHeldDown == true && g_input->isKeyDown(VK_RETURN) == false)
	{
		m_returnIsHeldDown = false;
	}
	if (m_tabIsHeldDown == true && g_input->isKeyDown(VK_TAB) == false)
	{
		m_tabIsHeldDown = false;
	}
	if (m_pIsHeldDown == true && g_input->isKeyDown('P') == false)
	{
		m_pIsHeldDown = false;
	}
	if (m_cIsHeldDown == true && g_input->isKeyDown('C') == false)
	{
		m_cIsHeldDown = false;
	}
}

void TheGame::ChangeStates(const TempGameStates& state)
{
	if (state != State_Invalid)
	{
		if (state != currentState)
		{
			previousState = currentState;
			currentState = state;
		}
		stateFinished = false;
		secondsSinceLastStateChange = 0.f;
		stateToChangeTo = State_Invalid;
	}
}
void TheGame::StateChangeTest(bool& keyHeld, unsigned char keyVal, const TempGameStates& changeToThisState)
{
	if (keyHeld == false && g_input->isKeyDown(keyVal) == true)
	{
		keyHeld = true;
		if (stateChanging == false)
		{
			stateChanging = true;
			stateToChangeTo = changeToThisState;
		}
	}
}
const bool TheGame::SaveAll()
{
	if (g_map == nullptr || m_player == nullptr)
	{
		return false;
	}
	XMLNode mapData = XMLNode::createXMLTopNode("MapData");
	mapData.addAttribute("simulationClock", std::to_string(simulationClock).c_str());
	mapData.addAttribute("numOfMonstersToMaintain", std::to_string(npcsToSpawn).c_str());
	mapData.addAttribute("stepsSinceLastCheckedSpawn", std::to_string(stepsSinceLastCheckedSpawn).c_str());
	g_map->WriteToXMLNode(mapData);

	XMLNode factions = mapData.addChild("Factions");
	Faction::WriteAllToXMLNode(factions);

	XMLNode GroundItems = mapData.addChild("ItemBlueprints");
	for (size_t i = 0; i < Item::s_ItemsOnTheGround.size(); i++)
	{
		Item::s_ItemsOnTheGround.at(i)->WriteToXMLNode(GroundItems);
	}
	XMLNode Features = mapData.addChild("FeatureBlueprints");
	for (size_t i = 0; i < Feature::s_FeaturesOnMap.size(); i++)
	{
		Feature::s_FeaturesOnMap.at(i)->WriteToXMLNode(Features);
	}
	XMLNode agents = mapData.addChild("Agents");
	for (std::map<float, Agent*>::iterator it = m_turnOrderMap.begin(); it != m_turnOrderMap.end(); it++)
	{
		if (it->second != nullptr)
		{
			XMLNode agent = agents.addChild("Agent");
			agent.addAttribute("turnOrder", std::to_string(it->first).c_str());
			it->second->WriteToXMLNode(agent);
		}
	}
	mapData.writeToFile("Data/Saves/SaveGame.Save.xml");
	return true;
}
const bool TheGame::LoadAll(const std::string& gamefileLoc)
{
	bool couldNotLoad = false;
	XMLNode root = EngineXMLParser::ParseXMLFile(gamefileLoc, "MapData", couldNotLoad);

	//mapData.addAttribute("simulationClock", std::to_string(simulationClock).c_str());
	if (couldNotLoad == true)
	{
		return false;
	}

	EnvironmentGenerationProcess gameData;
	gameData.PopulateFromXMLNode(root);
	simulationClock = (float)atof(gameData.GetAttrValueByName("simulationClock").c_str());
	npcsToSpawn = atoi(gameData.GetAttrValueByName("numOfMonstersToMaintain").c_str());
	stepsSinceLastCheckedSpawn = atoi(gameData.GetAttrValueByName("stepsSinceLastCheckedSpawn").c_str());
	g_map = new Map();
	m_player = new Player();
	bool SetPlayer = false;
	Agent::AddAgentToAllAgents(m_player);

	std::vector<GenerationProcessData*> children = gameData.GetChildren();
	for (size_t childIdx = 0; childIdx < children.size(); childIdx++)
	{
		//Map
		//Factions
		//ItemBlueprints //aka, items on ground.
		//FeatureBlueprints
		//Agents
		GenerationProcessData* curChild = children.at(childIdx);
		if (curChild != nullptr)
		{
			std::string name = curChild->GetName().name;
			if (SimpleStrCmp(name, "Map"))
			{
				g_map->ReadFromXMLNode(curChild);
			}
			else if (SimpleStrCmp(name, "Factions"))
			{
				Faction::ReadAllFromXMLNode(curChild);
			}
			else if (SimpleStrCmp(name, "ItemBlueprints"))
			{
				std::vector<GenerationProcessData*> items = curChild->GetChildren();
				for (size_t itemIdx = 0; itemIdx < items.size(); itemIdx++)
				{
					GenerationProcessData* item = items.at(itemIdx);
					if (item != nullptr)
					{
						Item* itemToLoad = new Item();
						itemToLoad->ReadFromXMLNode(item);
						itemToLoad->SetSpriteSheet(m_gameSpriteSheet);
						Item::s_AllItems.push_back(itemToLoad);
						Item::s_ItemsOnTheGround.push_back(itemToLoad);
					}
				}
			}
			else if (SimpleStrCmp(name, "FeatureBlueprints"))
			{
				std::vector<GenerationProcessData*> features = curChild->GetChildren();
				for (size_t i = 0; i < features.size(); i++)
				{
					GenerationProcessData* featureDat = features.at(i);
					if (featureDat != nullptr)
					{
						Feature* newFeat = new Feature();
						newFeat->ReadFromXMLNode(featureDat);
						newFeat->SetSpriteSheet(m_gameSpriteSheet);
						Feature::s_FeaturesOnMap.push_back(newFeat);
					}
				}
			}
			else if (SimpleStrCmp(name, "Agents"))
			{
				std::vector<GenerationProcessData*> agents = curChild->GetChildren();
				for (size_t agentIdx = 0; agentIdx < agents.size(); agentIdx++)
				{
					std::string turnOrderVal = agents.at(agentIdx)->GetAttributeByNameString("turnOrder");
					float turnOrder = (float)atof(turnOrderVal.c_str());
					GenerationProcessData* agentDat = agents.at(agentIdx)->GetChildByIndex(0);
					std::string blueprintName = agentDat->GetName().name;
					if (SimpleStrCmp(blueprintName, "PlayerBlueprint"))
					{
						m_player->ReadFromXMLNode(agentDat);
						m_player->SetSpriteSheet(m_gameSpriteSheet);
						m_turnOrderMap.insert(std::pair<float, Agent*>(turnOrder, m_player));
						SetPlayer = true;
					}
					else
					{
						NPC* age = NPC::CreateNPC();
						age->ReadFromXMLNode(agentDat);
						age->SetSpriteSheet(m_gameSpriteSheet);
						Agent::s_Agents.push_back(age);
						m_turnOrderMap.insert(std::pair<float, Agent*>(turnOrder, age));
					}
				}
			}
		}

	}
	IntVector2 mapSize = g_map->GetMapSize();
	g_map->ChangeTileRenderSize(Vector2(WindowWidth / (float)mapSize.x, (startMessageBoxHeight) / (float)mapSize.y));
	Agent::ReconnectPointersToOtherAgents();

	try
	{
		remove(gamefileLoc.c_str());
		s_messageBoxStrings.push_back("\'" + gamefileLoc + "\' was removed.");
		EnvironmentGenerationProcess::LoadSaveFiles();
	}
	catch (...)
	{
		s_messageBoxStrings.push_back("failed to remove \'" + gamefileLoc + "\'");
	}
	return true;
}

//------------------------------------------------------------------
//StateUpdates
void TheGame::StartUpUpdate(const float& deltaSeconds)
{
	stateToChangeTo = State_Invalid;
	secondsSinceLastStateChange += deltaSeconds;
	stateChanging = false;
	stateEnding = false;

	LoadNumberKeyValues();
	stateChanging = true;
	stateStarting = false;
	if (stateChanging == true)
	{
		stateFinished = true;
		stateToChangeTo = State_MainMenu;
	}
}
void TheGame::MainMenuUpdate(const float& deltaSeconds)
{
	if (m_focused == true)
	{
		stateToChangeTo = State_Invalid;
		secondsSinceLastStateChange += deltaSeconds;
		stateChanging = false;
		stateEnding = false;
		m_justSavedGame = false;

		StateChangeTest(m_qIsHeldDown, 'Q', State_ConfirmExit);
		StateChangeTest(m_cIsHeldDown, 'C', State_Continue);
		StateChangeTest(m_nIsHeldDown, 'N', State_PickMap);

		stateStarting = false;
		if (stateChanging == true)
		{
			stateFinished = true;
		}
	}
}
void TheGame::PickMapUpdate(const float& deltaSeconds)
{
	if (m_focused == true)
	{
		stateToChangeTo = State_Invalid;
		secondsSinceLastStateChange += deltaSeconds;
		stateChanging = false;
		stateEnding = false;

		bool previouslyStateGen = false;
		for (int i = 0; i < numberOfNumberKeys; i++)
		{
			StateChangeTest(numberKeysHeldDown[i], numberKeyValues[i], State_Generation);
			if (previouslyStateGen == false && stateToChangeTo == State_Generation)
			{
				previouslyStateGen = true;
				mapTypeSelected = i;
			}
		}
		if (stateToChangeTo == State_Generation && mapTypeSelected < numberOfMapGenerators)
		{

			//m_currentGenerator = (MapGenerator*)GeneratorRegistration::CreateGeneratorByname(mapGenName);
		}
		StateChangeTest(m_escapeIsHeldDown, VK_ESCAPE, State_MainMenu);

		if (m_tabIsHeldDown == false && g_input->isKeyDown(VK_TAB) == true)
		{
			m_tabIsHeldDown = true;
			m_automaticGeneration = !m_automaticGeneration;
		}

		stateStarting = false;
		if (stateChanging == true)
		{
			stateFinished = true;
		}
	}
}
void TheGame::GenerationUpdate(const float& deltaSeconds)
{
	stateToChangeTo = State_Invalid;
	secondsSinceLastStateChange += deltaSeconds;
	stateChanging = false;
	stateEnding = false;

	if (stateFinished == false)
	{

		if (mapTypeSelected < numberOfMapGenerators)
		{
			if (g_map == nullptr && mapTypeSelected < numberOfMapGenerators)
			{
				std::vector<std::string> generatorLocs = EnvironmentGenerationProcess::GetGeneratorLocations();
				std::string mapGenName = generatorLocs.at(mapTypeSelected);
				g_map = new Map(IntVector2(20,20));
				MapGenerator::RunGenerationInstructions(mapGenName, g_map, Vector2(WindowWidth, WindowHeight), m_messageBarHeight);
				stateFinished = true;
			}
			IntVector2 lastVisited = IntVector2(0, 0);
		}
		else
		{
			stateFinished = true;
		}
	}
	else
	{
		if (m_focused == true)
		{
			if (m_automaticGeneration == false)
			{
				Feature::SetSpriteSheetAllFeatures(m_gameSpriteSheet);
				StateChangeTest(m_escapeIsHeldDown, VK_ESCAPE, State_PickMap);
				StateChangeTest(m_spaceIsHeldDown, VK_SPACE, State_Generation);
				StateChangeTest(m_returnIsHeldDown, VK_RETURN, State_Playing);
				if (stateToChangeTo == State_Generation || stateToChangeTo == State_PickMap)
				{
					delete g_map;
					g_map = nullptr;
				}
				if (stateToChangeTo == State_Playing)
				{
					if (g_map != nullptr)
					{
						g_map->SetAllUnseenAndUnvisible();
					}
				}
			}
			else
			{
				if (stateChanging == false)
				{
					stateChanging = true;
					stateToChangeTo = State_Playing;
					if (g_map != nullptr)
					{
						g_map->SetAllUnseenAndUnvisible();
					}
				}
			}
		}
	}
	stateStarting = false;
}

void TheGame::PlayingUpdate(const float& deltaSeconds)
{
	if (m_focused == true)
	{
		stateToChangeTo = State_Invalid;
		secondsSinceLastStateChange += deltaSeconds;
		stateChanging = false;
		stateEnding = false;

		if (m_justSavedGame == true)
		{
			StateChangeTest(m_escapeIsHeldDown, VK_ESCAPE, State_MainMenu);
			if (stateChanging == true)
			{
				stateFinished = true;
				ClearTheGame();
			}
			return;
		}

		//Fill world with monsters
		if (stateStarting == true && previousState == State_Generation)
		{
			if (m_player == nullptr)
			{
				std::vector<Tile_Coord> validCoords = g_map->GetAllUnoccupiedTileCoordsOfType(TILE_AIR);
				size_t random = rand() % validCoords.size();
				m_player = new Player();
				m_createdPlayerOnce = true;
				Agent::AddAgentToAllAgents(m_player);
				bool canUse = false;
				IntVector2 wantTOUse(1, g_map->GetMapSize().y / 2);
				for (size_t i = 0; i < validCoords.size(); i++)
				{
					if (validCoords.at(i) == wantTOUse)
					{
						canUse = true;
						break;
					}
				}
				if (canUse == true)
				{
					m_player->SetBLPositionTrue(wantTOUse);
				}
				else
				{
					m_player->SetBLPositionTrue(validCoords.at(random));
				}
				Item* ringOfLevitation = ItemFactory::GenerateItemByName("Ring of Levitation", true);
				ringOfLevitation->SetSpriteSheet(m_gameSpriteSheet);
				ringOfLevitation->SetBLPosition(m_player->GetBlTilePos());
				m_player->SetSpriteSheet(m_gameSpriteSheet);
			}

			Generator::GenerateNItems(itemsToSpawn, m_gameSpriteSheet);
			Generator::GenerateNMonsters(npcsToSpawn, m_gameSpriteSheet);
			m_turnOrderMap.clear();
			m_turnOrderMap.insert(std::pair<float, Agent*>(0.0f, m_player));
			for (size_t i = 1; i < Agent::s_Agents.size(); i++)
			{
				if (Agent::s_Agents.at(i) != nullptr)
				{
					m_turnOrderMap.insert(std::pair<float, Agent*>(0.0f, Agent::s_Agents.at(i)));
				}
			}
			simulationClock = 0.0f;
		}
		if (m_mIsHeldDown == false && g_input->isKeyDown('M') == true)
		{
			m_mIsHeldDown = true;
			if (g_map != nullptr)
			{
				g_map->ToggleMapVisibleAlways();
			}
		}
		stateStarting = false;
		if (m_sIsHeldDown == false && g_input->isKeyDown('S') == true)
		{
			m_sIsHeldDown = true;
			bool saveSuccessful = SaveAll();
			if (saveSuccessful == true)
			{
				s_messageBoxStrings.push_back("Saved Successfully; save sent to \'Data/Saves/SaveGame.Save.xml\'");
				s_messageBoxStrings.push_back("Push \'esc\' to be brought back to the main menu.");
				m_justSavedGame = true;
				return;
			}
			else
			{
				s_messageBoxStrings.push_back("Did not save; either the map or the player is null");
			}
		}

		if (m_iIsHeldDown == false && g_input->isKeyDown('I') == true && m_player != nullptr && m_player->GetItemMenuActive() == false)
		{
			m_iIsHeldDown = true;
			m_player->ToggleItemMenuActive();
		}
		if (m_player != nullptr && m_player->GetItemMenuActive() == true)
		{
			PlayingInventoryUpdate(deltaSeconds);
			return;
		}
		if (m_player != nullptr && m_gIsHeldDown == false && g_input->isKeyDown('G') == true)
		{
			m_gIsHeldDown = true;
			m_player->ToggleGodMode();
			if (m_player->GetGodModeEnabled())
			{
				s_messageBoxStrings.push_back("God Mode Enabled.");
			}
			else
			{
				s_messageBoxStrings.push_back("God Mode Disabled.");
			}
		}

		if (m_pIsHeldDown == false && g_input->isKeyDown('P') == true)
		{
			m_pIsHeldDown = true;

			if (m_playerAutoUsing == true)
			{
				//IntVector2 newPos = m_aStarClosedList.at(0);
				//m_player->SetBLPosition(newPos);
				g_algoForViewAll->CalculateAStarAlgoInternal(g_map, m_player->GetMask());
				m_aStarClosedList = g_algoForViewAll->GetClosedListAsTileCoordsOnly();
				m_aStarOpenList = g_algoForViewAll->GetOpenListAsTileCoordsOnly();
			}
			else
			{
				m_aStarStart = m_player->GetBlTilePos();
				std::vector<IntVector2> openTiles = g_map->GetAllNonSolidValidTileCoords();
				int random = rand() % openTiles.size();
				m_aStarEnd = openTiles.at(random);
				g_algoForViewAll->Restart(g_map, m_aStarStart, m_aStarEnd);
				g_algoForViewAll->CalculateAStarAlgoInternal(g_map, m_player->GetMask());
				//g_algoForViewAll->CalculateAStarAlgo(g_map, m_aStarStart, m_aStarEnd);
				m_aStarClosedList = g_algoForViewAll->GetClosedListAsTileCoordsOnly();
				m_aStarOpenList = g_algoForViewAll->GetOpenListAsTileCoordsOnly();
				m_playerAutoUsing = true;
			}

		}
		else if (m_player->GetMoved())
		{
			m_playerAutoUsing = false;
		}

		//Regular Update

		bool simulating = true;
		bool simulationWaiting = false;
		if (m_player != nullptr && m_player->GetIsAlive() == true)
		{
			secondsSincePlayerDied = 0.0f;
		}

		if (m_player != nullptr)
		{
			std::vector<Agent*> agentsCurrentlyVisibleFromPlayer;
			FieldOfViewFloodFill::CalculateFieldOfViewForAgent(m_player, m_player->GetSightRadius(), g_map, agentsCurrentlyVisibleFromPlayer, false);
			m_player->SetVisibleAgents(agentsCurrentlyVisibleFromPlayer);
			Agent::SetListActorsVisibleToPlayer(agentsCurrentlyVisibleFromPlayer);
		}

		//Update PLayer and NPCs
		while (simulating)
		{
			std::multimap<float, Agent*>::iterator agentIt = m_turnOrderMap.begin();
			Agent* agent = agentIt->second;
			if (agent != nullptr && agent->GetIsAlive() == true)
			{
				if (agentIt->first > simulationClock)
				{
					if ((int)Agent::s_Agents.size() < (npcsToSpawn + 1))
					{
						stepsSinceLastCheckedSpawn += 1;
						if (stepsSinceLastCheckedSpawn >= stepsBeforeCheckSpawn)
						{
							stepsBeforeCheckSpawn = 0;
							Generator::GenerateNMonsters(1, m_gameSpriteSheet);
						}
					}
					else
					{
						stepsSinceLastCheckedSpawn = 0;
					}
					break;
				}
				if (!agent->IsReadyToUpdate())
				{
					agent->Update(deltaSeconds);
					if (!agent->IsReadyToUpdate())
					{
						simulationWaiting = true;
						break;
					}
				}
				m_turnOrderMap.erase(agentIt);

				float duration = agent->Update(deltaSeconds);
				m_turnOrderMap.insert(std::pair<float, Agent*>((simulationClock + duration), agent));
			}
			else
			{
				m_turnOrderMap.erase(agentIt);
			}
		}
		if (simulationWaiting == false)
		{
			simulationClock += simulationDelta;
		}

		//Step Update
		for (size_t i = 0; i < Agent::s_Agents.size(); i++)
		{
			if (Agent::s_Agents.at(i) != nullptr)
			{
				Agent::s_Agents.at(i)->UpdateStep();
			}
		}
		if ((m_player != nullptr && m_player->GetIsAlive() == false) || (m_createdPlayerOnce == true && m_player == nullptr))
		{
			secondsSincePlayerDied += deltaSeconds;
			for (size_t i = 0; i < m_WholeWindowBlackBoxFadeOnDeath.size(); i++)
			{
				if (secondsSincePlayerDied < secondsBeforeFullyFadeToBlackSincePlayerDied)
				{
					m_WholeWindowBlackBoxFadeOnDeath.at(i).m_color.Alpha = secondsSincePlayerDied / secondsBeforeFullyFadeToBlackSincePlayerDied;
				}
				else
				{
					m_WholeWindowBlackBoxFadeOnDeath.at(i).m_color.Alpha = 1.f;
				}
			}
			float current = secondsSincePlayerDied - secondsBeforeFullyFadeToBlackSincePlayerDied;
			if (current <= 0.f)
			{
				YouAreDeadColor.Alpha = 0.f;
			}
			else if (current >= secondsAfterFadeToBlackToFadeInYouAreDeadText)
			{
				YouAreDeadColor.Alpha = 1.f;
			}
			else
			{
				YouAreDeadColor.Alpha = current / secondsAfterFadeToBlackToFadeInYouAreDeadText;
			}
		}


		for (size_t i = 0; i != m_turnOrderMap.size(); i++)
		{
			std::multimap<float, Agent*>::iterator agentIt = m_turnOrderMap.begin();
			bool isEnd = false;
			for (size_t j = 0; j < i; j++)
			{
				agentIt++;
				if (agentIt == m_turnOrderMap.end())
				{
					isEnd = true;
					break;
				}
			}
			if (isEnd == false && agentIt != m_turnOrderMap.end() && agentIt->second != nullptr && agentIt->second->GetIsAlive() == false)
			{
				std::string name = agentIt->second->GetEntityName();
				s_messageBoxStrings.push_back(name + " Died.");
				m_turnOrderMap.erase(agentIt);
				i--;
			}
		}

		Agent::FinalPassBeforeRemoveAgents();
		Agent::RemoveDeadNPCs();
		StateChangeTest(m_escapeIsHeldDown, VK_ESCAPE, State_Paused);

		if (stateChanging == true)
		{
			stateFinished = true;
		}
	}
}
void TheGame::PlayingInventoryUpdate(const float& deltaSeconds)
{
	deltaSeconds;
	if (m_iIsHeldDown == false && m_returnIsHeldDown == false && g_input->isKeyDown(VK_RETURN) == true 
		&& m_player != nullptr && m_player->GetItemMenuActive() == true)
	{
		m_returnIsHeldDown = true;
		m_player->ToggleItemMenuActive();
	}
}
void TheGame::PausedUpdate(const float& deltaSeconds)
{
	if (m_focused == true)
	{
		stateToChangeTo = State_Invalid;
		secondsSinceLastStateChange += deltaSeconds;
		stateChanging = false;
		stateEnding = false;

		StateChangeTest(m_escapeIsHeldDown, VK_ESCAPE, State_Playing);
		StateChangeTest(m_mIsHeldDown, 'M', State_MainMenu);
		StateChangeTest(m_qIsHeldDown, 'Q', State_ConfirmExit);

		if (stateChanging == true)
		{
			if (stateToChangeTo != State_Playing && stateToChangeTo != State_ConfirmExit)
			{
				ClearTheGame();
			}
			stateFinished = true;
		}
		stateStarting = false;
	}

}
void TheGame::ConfirmExitUpdate(const float& deltaSeconds)
{
	if (m_focused == true)
	{
		stateToChangeTo = State_Invalid;
		secondsSinceLastStateChange += deltaSeconds;
		stateChanging = false;
		stateEnding = false;

		StateChangeTest(m_nIsHeldDown, 'N', previousState);
		StateChangeTest(m_yIsHeldDown, 'Y', State_Shutdown);

		if (stateChanging == true)
		{
			stateFinished = true;
		}
		stateStarting = false;
	}
}
void TheGame::ShutdownUpdate(const float& deltaSeconds)
{
	stateToChangeTo = State_Invalid;
	secondsSinceLastStateChange += deltaSeconds;
	stateChanging = false;
	stateEnding = false;

	if (g_map != nullptr)
	{
		delete g_map;
		g_map = nullptr;
	}
	if (m_player != nullptr)
	{
		delete m_player;
		m_player = nullptr;
	}
	Agent::ClearAllAgents();
	Item::ClearAllItems();
	Feature::DestroyAllFeatures();

	m_quitNow = true;
	stateStarting = false;

	stateChanging = true;
	stateFinished = true;
}
void TheGame::ContinueUpdate(const float& deltaSeconds)
{
	stateToChangeTo = State_Invalid;
	secondsSinceLastStateChange += deltaSeconds;
	stateChanging = false;
	stateEnding = false;
	bool previouslyStateGen = false;
	std::string fileLoc;
	std::vector<std::string> saveFileLocs = EnvironmentGenerationProcess::GetSaveFileLocations();
	for (int i = 0; i < numberOfNumberKeys && i < (int)saveFileLocs.size(); i++)
	{
		StateChangeTest(numberKeysHeldDown[i], numberKeyValues[i], State_Playing);
		if (previouslyStateGen == false && stateToChangeTo == State_Playing)
		{
			previouslyStateGen = true;
			mapTypeSelected = i;
			fileLoc = saveFileLocs.at(i);
		}
	}
	if (previouslyStateGen == true && fileLoc != "")
	{
		LoadAll(fileLoc);
	}
	else
	{
		stateToChangeTo = State_Continue;
		stateChanging = false;
	}

	StateChangeTest(m_escapeIsHeldDown, VK_ESCAPE, State_MainMenu);
	if (stateChanging == true)
	{
		stateFinished = true;
	}
	stateStarting = false;
}


//---------------------------------------------------------------------
//StateRenders
void TheGame::StartUpRender() const
{
	SetUp2DView(Rgba(0.f, 0.f, 1.f, 1.f));

}
void TheGame::MainMenuRender() const
{
	SetUp2DView(Rgba(0.f, 1.f, 1.f, 1.f));
	if (m_font != nullptr)
	{
		/*
		void RenderText(const Vector3& position, const std::string& toDraw, const float& fontSize = 1.f, const Vector3& direction = Vector3(1.f, 0.f, 0.f),
		const Vector3& up = Vector3(0.f,1.f, 0.f),
		const Rgba& color = Rgba(1.f, 1.f, 1.f, 1.f), const bool& enableNewLines = false) const;
		*/
		m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 1.5f, 0.f), "DREKLIKE", m_fontSize * 2.f, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 3.f, 0.f), "Main Menu", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 4.f, 0.f), "Press \'N\' for New Game.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		Vector3 quitPos;
		m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 5.f, 0.f), "Press \'C\' to Continue", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		quitPos = Vector3((WindowWidth / 2.f), WindowHeight - 6.f);
		m_font->RenderText(quitPos, "Press \'Q\' to Quit.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
	}
}
void TheGame::PickMapRender() const
{
	SetUp2DView(Rgba(1.f, 0.f, 1.f, 1.f));
	if (m_font != nullptr)
	{
		m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 1.f, 0.f), "Map Selection", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 3.f, 0.f), "Press \'TAB\' to toggle automatic generation on or off.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		std::string toggled = "Automatic Generation is ";
		if (m_automaticGeneration == true)
		{
			toggled += "ON.";
		}
		else
		{
			toggled += "OFF.";
		}
		m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 4.f, 0.f), toggled, m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);

		float startYDown = 4.75f;
		float trueFontSize = m_fontSize * (2.f / 3.f);
		float amountToStepDown = 0.4f;
		std::vector<std::string> fileLocs = EnvironmentGenerationProcess::GetGeneratorLocations();
		for (size_t i = 0; i < numberOfNumberKeys && i < fileLocs.size(); i++)
		{
			std::string fileLoc = fileLocs.at(i);
			fileLoc = ReplaceStringWithString(fileLoc, "Data/Environments/", "");
			std::string message = "Press \'" + std::to_string(i) + "\' to " + fileLocs.at(i);
			m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - (startYDown + (amountToStepDown * i))), message, trueFontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		}

	}
}
void TheGame::GenerationRender() const
{
	SetUp2DView(Rgba(0.f, 0.f, 0.f, 1.f));
	if (g_map != nullptr)
	{
		g_map->Render(m_gameSpriteSheet);
		g_Renderer->DrawVertexArray(m_messageBoxBackground, PRIMITIVE_QUADS, nullptr);
		Feature::RenderAllFeatures(m_playingBackgroundColor);
	}
	if (m_font != nullptr)
	{
		m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight) - 1.f, 0.f), "Map Generation Phase", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.2f, 0.5f, 1.f), Font_Center_Aligned);

		if (stateFinished == false)
		{
			m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight / 2.f, 0.f), "Loading...", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.2f, 0.5f, 1.f), Font_Center_Aligned);
		}
		else
		{
			m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight / 2.f, 0.f), "Load Complete!", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.2f, 1.0f, 1.f), Font_Center_Aligned);
			if (m_automaticGeneration == false)
			{
				m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight / 2.f) - 1.f, 0.f), "Press \'SPACE\' to restart Generation phase.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.2f, 1.f, 1.f), Font_Center_Aligned);
				m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight / 2.f) - 2.f, 0.f), "Press \'ESC\' to return to Map Selection.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.2f, 1.0f, 1.f), Font_Center_Aligned);
				m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight / 2.f) - 3.f, 0.f), "Press \'RETURN\' to begin play.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.2f, 1.0f, 1.f), Font_Center_Aligned);
			}
		}
	}
}

void TheGame::PlayingRender() const
{
	SetUp2DView(m_playingBackgroundColor);
	//Render Dialogue box
	g_Renderer->DrawVertexArray(m_messageBoxBackground, PRIMITIVE_QUADS, nullptr);
	float displaceAmount = (m_fontSize * 20.f);
	if (m_font != nullptr)
	{
		if ((int)s_messageBoxStrings.size() <= numOfMessageLines)
		{
			for (size_t i = 0; i < s_messageBoxStrings.size(); i++)
			{
				Vector3 position = Vector3(0.f, WindowHeight - displaceAmount - (displaceAmount * (i)));
				m_font->RenderText(position, s_messageBoxStrings.at(i), m_messageFontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f));
			}
		}
		else
		{
			int posInd = 1;
			for (size_t i = (s_messageBoxStrings.size() - (numOfMessageLines)); i < s_messageBoxStrings.size(); i++)
			{
				size_t pos = (s_messageBoxStrings.size() - i);
				Vector3 position = Vector3(0.f, WindowHeight - (displaceAmount * pos));
				m_font->RenderText(position, s_messageBoxStrings.at(s_messageBoxStrings.size() - posInd), m_messageFontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f));
				posInd++;
			}
		}
	}

	//Render Map
	bool showFullMap = false;
	if (g_map != nullptr)
	{
		g_map->Render(m_gameSpriteSheet);
		showFullMap = g_map->GetViewFullMapRegardlessOfVisibility();
		if (g_algoForViewAll != nullptr && m_playerAutoUsing)
		{
			Vector2 renderSize = g_map->GetTileRenderSize();
			g_algoForViewAll->Render(renderSize);
		}
		Rgba color = m_playingBackgroundColor;
		color.Alpha = 0.8f;
		Feature::RenderAllFeatures(m_playingBackgroundColor);
		Item::RenderAllGroundItems(color, showFullMap);
	}

	//Render Agents
	for (size_t i = 0; i < Agent::s_Agents.size(); i++)
	{
		if (Agent::s_Agents.at(i) != nullptr)
		{
			Agent::s_Agents.at(i)->SetRenderReguardless(showFullMap);
			Agent::s_Agents.at(i)->Render(m_playingBackgroundColor);
		}
	}

	//Render player specific data (such as health bar.
	if (m_player != nullptr)
	{
		float healthPerc = (float)m_player->GetCurrentHealth() / (float)m_player->GetMaxHealth();

		std::vector<Vertex_PCT> healthBackground;
		std::vector<Vertex_PCT> healthPercentBar;
		float displaceAmountTopHeight;
		float displaceAmountBottomHeight = startMessageBoxHeight;
		displaceAmountTopHeight = displaceAmountBottomHeight + 0.1f;

		healthBackground.push_back(Vertex_PCT(Vector3(0.f, displaceAmountBottomHeight), Rgba(0.5f, 0.5f, 0.5f, 1.f)));
		healthBackground.push_back(Vertex_PCT(Vector3(WindowWidth, displaceAmountBottomHeight), Rgba(0.5f, 0.5f, 0.5f, 1.f)));
		healthBackground.push_back(Vertex_PCT(Vector3(WindowWidth, displaceAmountTopHeight), Rgba(0.5f, 0.5f, 0.5f, 1.f)));
		healthBackground.push_back(Vertex_PCT(Vector3(0.f, displaceAmountTopHeight), Rgba(0.5f, 0.5f, 0.5f, 1.f)));

		healthPercentBar.push_back(Vertex_PCT(Vector3(0.f, displaceAmountBottomHeight), Rgba(1.f, 0.1f, 0.1f, 1.f)));
		healthPercentBar.push_back(Vertex_PCT(Vector3(WindowWidth * healthPerc, displaceAmountBottomHeight), Rgba(1.f, 0.1f, 0.1f, 1.f)));
		healthPercentBar.push_back(Vertex_PCT(Vector3(WindowWidth * healthPerc, displaceAmountTopHeight), Rgba(1.f, 0.1f, 0.1f, 1.f)));
		healthPercentBar.push_back(Vertex_PCT(Vector3(0.f, displaceAmountTopHeight), Rgba(1.f, 0.1f, 0.1f, 1.f)));

		if (m_player->GetItemMenuActive() == true)
		{
			healthBackground.push_back(Vertex_PCT(Vector3(inventoryStartX, 0.f), Rgba(1.f, 1.f, 1.f, 1.f)));
			healthBackground.push_back(Vertex_PCT(Vector3(WindowWidth, 0.f), Rgba(1.f, 1.f, 1.f, 1.f)));
			healthBackground.push_back(Vertex_PCT(Vector3(WindowWidth, displaceAmountBottomHeight), Rgba(1.f, 1.f, 1.f, 1.f)));
			healthBackground.push_back(Vertex_PCT(Vector3(inventoryStartX, displaceAmountBottomHeight), Rgba(1.f, 1.f, 1.f, 1.f)));
		}

		g_Renderer->DrawVertexArray(healthBackground, PRIMITIVE_QUADS, nullptr);
		g_Renderer->DrawVertexArray(healthPercentBar, PRIMITIVE_QUADS, nullptr);

		if (m_player != nullptr)
		{
			m_player->RenderInventory(Vector3(inventoryStartX, displaceAmountBottomHeight), m_font, m_messageFontSize);
		}
	}

	if ((m_player != nullptr && m_player->GetIsAlive() == false) || (m_createdPlayerOnce == true && m_player == nullptr))
	{
		g_Renderer->DrawVertexArray(m_WholeWindowBlackBoxFadeOnDeath, PRIMITIVE_QUADS, nullptr);
		m_font->RenderText(Vector3(WindowWidth / 2.f, WindowHeight / 2.f), "You are dead.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f),
			Rgba(1.f, 1.f, 1.f, 1.f), Font_Center_Aligned);
	}
}
void TheGame::PausedRender() const
{
	SetUp2DView(Rgba(0.5f, 0.5f, 0.5f, 1.f));
	if (m_font != nullptr)
	{
		m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight) - 1.f, 0.f), "Pause Menu.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);

		m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight / 2.f), 0.f), "Press \'ESCAPE\' to return to Playing Phase.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight / 2.f) - 1.f, 0.f), "Press \'M\' to return to the Main Menu.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
		m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight / 2.f) - 2.f, 0.f), "Press \'Q\' to Quit.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f), Font_Center_Aligned);
	}
}
void TheGame::ConfirmExitRender() const
{
	SetUp2DView(Rgba(0.1f, 0.1f, 0.1f, 1.f));
	if (m_font != nullptr)
	{
		m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight) - 1.f, 0.f), "Do you wish to Quit?", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(1.0f, 1.f, 1.0f, 1.f), Font_Center_Aligned);

		m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight / 2.f), 0.f), "\'Y\'es? or \'N\'o?", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(1.f, 1.f, 1.f, 1.f), Font_Center_Aligned);
	}
}
void TheGame::ShutdownRender() const
{
	SetUp2DView(Rgba(1.f, 1.f, 1.f, 1.f));
	m_font->RenderText(Vector3((WindowWidth / 2.f), (WindowHeight)-1.f, 0.f), "Shutting Down...", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(1.f, 1.f, 1.f, 1.f), Font_Center_Aligned);

}
void TheGame::ContinueRender() const
{
	SetUp2DView(Rgba(0.f, 0.f, 0.f, 1.f));
	m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 1.f), "Continue Menu", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(1.f, 1.f, 1.f, 1.f), Font_Center_Aligned);
	m_font->RenderText(Vector3((WindowWidth / 2.f), WindowHeight - 2.f), "Press \'Escape\' to go back.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(1.f, 1.f, 1.f, 1.f), Font_Center_Aligned);
	std::vector<std::string> saveFileLocs = EnvironmentGenerationProcess::GetSaveFileLocations();
	Vector3 startLoc = Vector3((WindowWidth / 2.f), WindowHeight - 3.f);
	if (saveFileLocs.size() <= 0)
	{
		m_font->RenderText(startLoc, "You do not have any save files to load.", m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(1.f, 1.f, 1.f, 1.f), Font_Center_Aligned);
	}
	else
	{
		Vector3 amountToDisplace = Vector3(0.f, 1.f);
		Vector3 curLoc = startLoc;
		for (size_t i = 0; i < saveFileLocs.size(); i++)
		{
			std::string message = "Press \'" + std::to_string(i) + "\' to load " + saveFileLocs.at(i);
			m_font->RenderText(curLoc, message, m_fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(1.f, 1.f, 1.f, 1.f), Font_Center_Aligned);
			curLoc -= amountToDisplace;
		}
	}
}

//-------------------------------------------------------------------------
//Monsters/NPCs


void TheGame::EraseNPC(Agent* npc)
{
	for (std::multimap<float, Agent*>::iterator it = m_turnOrderMap.begin(); it != m_turnOrderMap.end(); it++)
	{
		if (it->second != nullptr && it->second == npc)
		{
			m_turnOrderMap.erase(it);
		}
	}

	Agent::ClearAgentNonPlayerPointer(npc);
}


//-------------------------------------------------------------------------
//StartUpPhases

void TheGame::LoadNumberKeyValues()
{
	for (int i = 0; i < numberOfNumberKeys; i++)
	{
		numberKeysHeldDown[i] = false;
	}
	numberKeyValues[0] = '0';
	numberKeyValues[1] = '1';
	numberKeyValues[2] = '2';
	numberKeyValues[3] = '3';
	numberKeyValues[4] = '4';
	numberKeyValues[5] = '5';
	numberKeyValues[6] = '6';
	numberKeyValues[7] = '7';
	numberKeyValues[8] = '8';
	numberKeyValues[9] = '9';
}