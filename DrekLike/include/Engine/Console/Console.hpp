#pragma once
#include "Engine/Math/Vertex_PCT.hpp"
#include "Engine/Font/Font.hpp"
#include "Engine/Console/Command.hpp"
#include "Engine/Console/RegisterCommandHelper.hpp"
#include <vector>
#include <map>

/*
	CONSOLE COMMANDS!
	Register commands.

	void ShowHelp(Command &args)
	{
		//do stuff.
		int arg0
		char const *arg1;
		if(args->get_next_int(&arg0, 0) && args->get_next_string(&arg1, nullptr))
		{
			//do something with args.
		}
		else
		{
			ConsolePrintf(cConsoleColor_Warning, "help follows format: help <int> <string>");
		}
		Rgba color;
		args->get_next_color(&color, cColor_White);
	}
	typdef void (*console_command_cb)( Command& );

	void Console::RegisterCommand(const char* command_name, console_command_cb callback)
	{
		//HashMap<std::string, console_command_cb> _callbacks;
	}
	void SomeSystem::Init()
	{
		Console::RegisterCommand("help", ShowHelp);

		Console::RunCommand("help");
		Console::RunCommand("help clear");
	}

	//////////////////////////////////////////////////////////////////////////
	static HashMap<std::string, console_command_cb> gCommands;
	void RegisterCommand(std::string name, console_command_cb cb);
	{
		gCommands.add(name, cb);
	}
	void RunCommand(std::string command_line)
	{
		Command com(command_line);
		console_command_cb out_command;
		if(gCommands.find(com->get_command_name(), & out_command))
		{
			out_command(com);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	//MACROS
	class RegisterCommandHelper
	{
		public:
		RegisterCommandHelper(char const *name, console_command_cb cb)
		{
			Console::RegisterCommand(name, cb);
		}
	};
	static RegisterCommandHelper _helpHelper("help", ShowHelp);

	#define CONSOLE_COMMAND( name ) void ConsoleCommand_ ## name ## ( Command &args); \
	static RegisterCommandHelper RegistrationHelper_ ## name ## ( #name, ConsoleCommand_ ## name ## ); \
	void ConsoleCommand_ ## name ##(command &args)

	CONSOLE_COMMAND( Help )
	{
		//implement help
	}
	CONSOLE_COMMAND( Clear )
	{
		//args
		//implement clear
	}

	// Becomes
	void ConsoleCommand_Clear( Command & );
	static RegisterCommandHelper RegistrationHelper_Clear("Clear", ConsoleCommand_Clear);
	void ConsoleCommand_Clear(Command &args)
	{

	}

*/

#ifndef CONSOLE_HPP
#define CONSOLE_HPP
//-----------------------------------------------------
class Console;

//-----------------------------------------------------
extern Console* g_Console;

//-----------------------------------------------------
typedef void(*console_command_cb)(Command&);

struct SpecialLine
{
	SpecialLine(const std::string& text = "", const Rgba& color = Rgba(1.f,1.f,1.f,1.f), const bool& drawTextInBox = false, const Rgba& boxRGBA = Rgba(0.f,0.f,0.f,1.f));
	Rgba lineColor;
	std::string line;
	bool drawInBox;
	Rgba boxColor;
};



enum ConsoleImportanceLevel
{
	INVALID_IMPORTANCE = -1,
	UNIMPORTANT,
	WARNING,
	URGENT
};
class Console
{
public:
	Console(Font* fontToUse = nullptr, const bool& responsibleForDeletingFont = false);
	~Console();

	void Update(float deltaSeconds);
	void Render() const;

	void ChangeFont(Font* fontToUse);
	void SetResponsibleForDeletingFont(const bool& responsible) { m_responsible = responsible; };
	void ConsolePrint(const std::string& input, const Rgba& color = Rgba(1.f, 1.f, 1.f, 0.f), const bool& backgroundBox = false, const Rgba& backgroundColor = Rgba(0.f,0.f,0.f,1.f));
	void ConsolePrintf(const ConsoleImportanceLevel& importanceLevel, const std::string& consoleMessage);
	const bool RegisterCommand(const std::string& command_name, console_command_cb callback);
	const bool RunCommand(const std::string& command_line);

	void setConsoleActive(const bool& active) { consoleActive = active; };
	const bool isConsoleActive() const { return consoleActive; };

	const bool isQuitting() const;

	void clear();
	void quit() { quitApplication = true; };
	void help(const Command& args);
private:


	const bool guaranteeKeySpecialCase(const unsigned char& input) const;
	void handleKeyPressEventsCorrectly(const unsigned char& input);
	void handleShiftCaseChanges(size_t& endVal);
	void testAllKeysForPushedDown();
	void UpdateCursor(const float& deltaSeconds);

	void RenderCommandPromptLines() const;
	void RenderCurrentlyBeingTypedText() const;
	void RenderCursor() const;

	bool consoleActive;
	bool m_responsible;
	bool cursorOn;
	int cursorPosition;
	float m_fontSize;
	float m_secondsSinceLastToggledCursor;
	int currentFirstViewable;
	size_t maxNumLines;
	Vector2 assumedAspects;
	Vector2 commandPromptBoxStart;
	Vector2 commandPromptBoxEnd;
	Vector2 commandLineStart;
	Vector2 inputBoxStart;
	Vector2 inputBoxLineStart;
	Rgba currentPushColor;
	Font* m_consoleFont;
	std::vector<Vertex_PCT> overlayVertices;
	std::vector<SpecialLine> commandPromptLines;
	std::vector<bool> m_whatKeysHeldDown;
	std::string currentlyBeingTyped;

	bool quitApplication;
	static std::map<std::string, console_command_cb> m_commands;

	static const float s_secondsBetweenToggleCursorView;
	static const Rgba s_defaultColor;
	static const std::string s_brokenBar;
	static const Vector3 s_rightDirection;
	static const Vector3 s_upDirection;
	//static std::map<std::string, console_command_cb> gCommands;
};
#endif // !CONSOLE_HPP
