#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Input.hpp"
#include "Engine/Core/Camera3D.hpp"
#include "Engine/Math/ExtendedMath.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Debug/DebugRenderring.hpp"
#include "Engine/Font/Font.hpp"
#include "Game/TempGameStates.hpp"
#include "Game/GameEntity/AgentType/Player.hpp"
#include "Game/Pathfinding/AStarAlgo.hpp"
#include "Game/GameEntity/AgentType/NPC/NPC.hpp"

#ifndef THEGAME_HPP
#define THEGAME_HPP
//-----------------------------------------------------
class TheGame;

//-----------------------------------------------------
extern TheGame* g_TheGame;


//-----------------------------------------------
class TheGame
{
public:
	TheGame();
	~TheGame();
	void InitializeTileDefinitions();
	void Update(const float& deltaSeconds, const bool& focus);
	void Render() const;
	void SetFont(Font* font);
	void CameraControls(const float& deltaSeconds);
	void XYZPlaneMovement(const float& deltaSeconds);
	void SetUp3DView() const;
	void SetUp2DView(const Rgba& screenColor) const;
	void DrawDebugAxisLines() const;
	bool isQuitting();
	void ClearTheGame();
protected:
	void UpdateKeysHeldDown();
	void ChangeStates(const TempGameStates& state);
	void StateChangeTest(bool& keyHeld, unsigned char keyVal, const TempGameStates& changeToThisState);
	const bool SaveAll();
	const bool LoadAll(const std::string& gamefileLoc);

	//StateUpdates
	void StartUpUpdate(const float& deltaSeconds);
	void MainMenuUpdate(const float& deltaSeconds);
	void PickMapUpdate(const float& deltaSeconds);
	void GenerationUpdate(const float& deltaSeconds);
	void PlayingUpdate(const float& deltaSeconds);
	void PlayingInventoryUpdate(const float& deltaSeconds);
	void PausedUpdate(const float& deltaSeconds);
	void ConfirmExitUpdate(const float& deltaSeconds);
	void ShutdownUpdate(const float& deltaSeconds);
	void ContinueUpdate(const float& deltaSeconds);

	//StateRenders
	void StartUpRender() const;
	void MainMenuRender() const;
	void PickMapRender() const;
	void GenerationRender() const;
	void PlayingRender() const;
	void PausedRender() const;
	void ConfirmExitRender() const;
	void ShutdownRender() const;
	void ContinueRender() const;

	//StartUpPhases
	void LoadNumberKeyValues();

	//Monsters/NPCs
	void EraseNPC(Agent* npc);

	//Variables:
	float WindowWidth;
	float WindowHeight;
	float secondsSinceLastStateChange;
	float timeSecondsSinceLastStep;
	float timeSecondsBetweenStepsIfRealTime;
	float simulationClock;
	float simulationDelta;
	float inventoryStartX;

	static const int numberOfNumberKeys = 10;
	bool numberKeysHeldDown[numberOfNumberKeys];
	unsigned char numberKeyValues[numberOfNumberKeys];

	bool stateFinished;
	bool stateChanging;
	bool stateEnding;
	bool stateStarting;
	bool m_mIsHeldDown;
	bool m_nIsHeldDown;
	bool m_qIsHeldDown;
	bool m_pIsHeldDown;
	bool m_yIsHeldDown;
	bool m_sIsHeldDown;
	bool m_iIsHeldDown;
	bool m_cIsHeldDown;
	bool m_gIsHeldDown;
	bool m_tabIsHeldDown;
	bool m_escapeIsHeldDown;
	bool m_returnIsHeldDown;
	bool m_spaceIsHeldDown;
	bool m_quitNow;
	bool m_focused;
	bool m_automaticGeneration;
	bool m_playerAutoUsing;
	bool m_justSavedGame;

	int numberOfMapGenerators;
	int mapTypeSelected;
	int itemsToSpawn;
	int npcsToSpawn;
	const int numOfMessageLines = 12;
	std::vector<Vertex_PCT> m_messageBoxBackground;
	std::vector<Vertex_PCT> m_WholeWindowBlackBoxFadeOnDeath;
	const float m_fontSize = 0.01f;
	const float m_messageFontSize = 0.005f;
	const float m_messageBarHeight = 2.5f;
	float startMessageBoxHeight = 0.0f;
	float secondsSincePlayerDied = 0.0f;
	const float secondsBeforeFullyFadeToBlackSincePlayerDied = 5.f;
	const float secondsAfterFadeToBlackToFadeInYouAreDeadText = 2.f;
	int stepsSinceLastCheckedSpawn;
	int stepsBeforeCheckSpawn;
	Rgba YouAreDeadColor;
	Rgba m_playingBackgroundColor;


	static const IntVector2 s_centralMousePosition;
	static const Vector2 s_mouseYawPitchSensitivity;
	Camera3D m_mainCamera;
	Vector3 cameraVelocity;
	TempGameStates previousState;
	TempGameStates currentState;
	TempGameStates stateToChangeTo;
	IntVector2 numTiles = IntVector2(20, 20);
	std::multimap<float, Agent*> m_turnOrderMap; //can have any number of Agents with same key.
	SpriteSheet* m_gameSpriteSheet;


	std::string m_mapName = "";
	Player* m_player;
	bool m_createdPlayerOnce;
	Font* m_font;


	std::vector<IntVector2> m_aStarClosedList;
	std::vector<IntVector2> m_aStarOpenList;
	IntVector2 m_aStarStart;
	IntVector2 m_aStarEnd;
};

#endif //THEGAME_HPP