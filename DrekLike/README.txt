SD 4 - RogueLike Project - DrekLike
- Created By Alexander Baird
----------------------------------------------------------

- Controls
	- From the Main Menu State Screen:
		* Q - Change state to ConfirmExit
		* C - Change State to Continue
		* N - Change state to Map Selection
	- From the Map Selection State Screen:
		* 0 - Change state to Generation (with respect to Cavern Generator)
		* 1 - Change state to Generation (with respect to Cellular Automation Map Generator)
		* 2 - Change state to Generation (with respect to From Data Generator)
		* 3 - Change state to Generation (with respect to Dungeon Map Generator)
		* 4 - Change state to Generation (with respect to Mixed Map)
		* 5 - Change state to Generation (with respect to Cellular Automation Map with low stone percentage chance)
		* (other unlisted numbers) - Change state to Generation (Do no special map generation)
		* Tab - Toggle Autogeneration on or off.
		* Escape - Change state to Main Menu
	- From the Generation State Screen (if autogeneration off and generation complete):
		* Escape - Change state to PickMap
		* Space - Redo Generation operations
		* Return - Change State to Playing
	- From the Playing State Screen:
		* h,j,k,l - (left, down, up, right)
		* y,u,b,n (nw,ne,sw,se)
		* m - toggle show all map
		* p - game randomly picks goal tile, if not already selected; if goal tile selected,
 uses A* to calculate path one step at a time from player to goal tile
		* i - Open Player inventory (activates Playing-Inventory state)
		* q - Quaff/Drink a random potion from player inventory.
		* a - Toggle on/off Features in the game that are on the same tile as or adjacent to player.
		* Escape - Change State to Pause
	- From the Playing-Inventory State
		* VK_Enter - Close Player Inventory (Return to Playing State)
	- From the Paused State Screen:
		* Escape - Change State to Playing
		* M - Change State to Main Menu
		* Q - Change State to ConfirmExit
	- From the ConfirmExit State Screen:
		* N - Return to previously visited State
		* Y - Change State to Shutdown (shutdown state will auto close the game)
	- From the Continue State Screen:
		* ESC - go back to main menu screen
		* 0-9 - if applicable, load a given save from the Saves folder, and immediatly go to playing state.
---------------------------------------------------------------

---------------------------------------------------------------
Assignment #1 : Procedural Generation
	-> Gameplay State Machine
		- Main Menu
			- N for New Game functionallity exists
			- Q for Quit functionallity exists
		- Map Selection
			- Functionallity for map 1 (Cavern Generator) exists
			- Functionallity for map 2 (Cellular Automata Generator) exists
			- Functionallity for map 3 (From Data Generator) exists
			- Functionallity for map 4 (Dungeon Generator) exists
			- Functionallity for map 5 (Mixed Map Generators) exists
			- Functionallity for Map 6 (Plains Generator) exists
			- Functionallity for returning to main menu exists
			- Functionallity for toggle auto-generate exists
		- Generating Level
			-If AutomaticGeneration enabled, loads and then goes straight to playing
			-If AutomaticGeneration disabled
				*Space bar allows for extra passes through generation
				*Esc returns to Map Selection WHEN map generation complete.
				* Enter switches state to playing
		- Playing
			- Pressing Esc pauses the game.
		- Pause
			- pressing M switches state back to main menu
			- pressing Q brings up Confirm Exit state
			- Going to the menu deletes the map
			- pressing Escape returns player to State_Playing
		- ConfirmExit
			- Pressing Y deletes the map (if it exists) and closes the game.
			- Pressing N returns to previous state.

	-> Procedural Map Generation System
		- Framework
			* Can Register Generator Builders which all Generators get plugged into and built out of,
regardless of type.
			* Cellular Automata Generator - Produces 60% stone, and 40% tiles for the map
			* Dungeon Generator - Generators a random count of Rooms, and connects them all via right angle tunnels
			* Custom Generator: Cavern Generator - Generates random tunnels of varying sizes.
			* From Data Generator: Read in xml to customly plug in rooms or tiles of varying types.
			* XML Loading Generation: Works perfectly; can write XML code to mix and match the previously mentioned generators.
	-> Environments
			* There is support for all Map Generators to customize the generator's actions for creating map chunks
This is done via XML files.
			* There are at least 5 XML files, 3 of which related to the single generators, one of which is a mixed file,
and the fifth is another one only relating to one generator, but with tweaked values.
-----------------------------------------------------------------------


-----------------------------------------------------------------------
Assignment #2: Player, Pathfinding, FOV
	-> Turn-based framework with a player and player movement
		- Entity System:
			* There is an Entity class in the engine, a GameEntity over in the game along with
an Agent, and a Player.
		- Turn-based Update:
			* Player movesbased on input along the map grid.
		- Player Movement:
			* player moves based on h,j,k,l, y,u,b,n keys
			* player is represented by the @ char
	-> Field of View and Fog of War
		- FOV Algorithm:
			*there are several FOV algorithms in the project, the presently used is the FloodFill version.
		- Blocked Map Areas are hidden.
		- FOG of war hides areas the player has not yet seen.
		- M key shows entire map regardless of blocked areas or Fog of War.
	-> Pathfinding
		- A* Algorithm:
			* A* algorithm exists and is used to find the best path to a given point in the map
			* Once A* hits the finish, it WILL NOT add more closed/open nodes.
		- Single Stepping:
			* upon pressing P, a point is randomly selected in the map as a goal tile, and then
with each progressive P, the A* algorithm collects tiles for it's calculations.
			*NOTE: Upon moving the character, the A* calculations will be cleared.
		-Representation:
			* A green square represents the current goal tile
			* blue tiles are in A*'s open list
			* red tiles are in the closed list.
-----------------------------------------------------------------------

-----------------------------------------------------------------------
Assignment #3 - NPCs, AI, and Combat
	->Data-Driven NPCs
		- A factory based system for adding new npcs exists; uses NPC XML data files to create
templates
		- 
	->Entity Detection
		- Entities are hidden from the player unless within view of the player, unless set full map to render regardless of visibility.
		- NPCs hang onto a bunch of pointers of Agents they can see.
	->AI Behaviors
		- Self-registering C++ framework exists for adding new AI Behaviors.
		- Wander Behavior exists, NPC wanders about aimlessly, and occassionally rest, baced on data file input.
		- Flee Behavior exists, on NPC going below a certain percentage, it will attempt to run away; though may scream.
		- Patrol Behavior exists, NPCs will patrol to a defined number of points from the XML file, points will be within a range also defined in the file
(note if they feel trapped, they will try to retaliate/MeleeAttack instead of flee)
		- MeleeAttack Behavior exists, NPC will decide whether or not to attack based on whether it hates the people within range of it enough.
		- Chase Behavior exists, NPC will chase its most hated target down.
	->Combat
		- Combat System Exists, Player and NPCs can do combat.
		- NPCs will attack the Agent they hate most.
	->Death
		- NPCs can die
		- Player can die
	->Status Bar
		- Status bar exists, and is along the bottom of the Message box, it will fill or unfill from red to dark gray depending on player's current health versus max health.
	->Message Bar
		- Message box exists, messages can be added.
		- Messages are added on combat interaction, showing how much damage was done.
	->Factions
		- Faction Framework exists, It loads all xml files out of the faction folder before loading any npcs.
		- Factions allow for NPC fighting NPCs.
	->Fade to Black (extra Credit)
		- When player dies, screen will slowly fade to black, and show "You are dead on the screen."

-----------------------------------------------------------------------

-----------------------------------------------------------------------
Assignment 4 - Save/Load, Items/ Features
	-> Save and Load
		- the option for "C to continue" was added to the main menu.
		- From the Continue Screen, if there is a save file in the Saves Folder,
One can press the numbers keys to load from 10 different save files stored in the folder.
		- During the playing state, can Save the entire game to an xml file located in 
"Data/Saves/SaveGame.Save.xml"; the game will notify the player of a successful save. (feel free to go into the save folder and rename it, if you wish; as long as it is in the folder, the game will find it).
		- Note also the map is saved specifically by it's tileType enum and whether or not its been seen.
		- Save files are destroyed on load.
		- Upon saving the game, the rest of your controls are locked up except for the escape button, which will return the player to the main menu.
	-> Items
		- Framework
			* When Player moves onto tile with atleast one item on it,
 it will spit out a bunch of messages saying what items are in the tile.
			* There exists an ItemFactory framework that enables spawning in items to the map.
		- Potions 
			* 2 types Potions exist (both are healing, but one is instantaneous healing, while the other is healing over time)
			* use Q to Quaff potions.
			* Slow Healing Potion will only display healing messages after each turn the player uses (until status effect is dead)
			* The other potion in the game is also a healing potion but heals more instantenously. it will display a message on the same turn used, guaranteed.
		- Weapons
			* 5 types Weapons exist
			* Picking up a weapon off the floor will auto equip it.
		- Armor
			* 5 types pieces of armor exist (rings are considered a part of armor for this)
			* Pick up a piece of armor off the floor will auto equip it.
		-Pickup Item Behavior
			*There exists a pick up item behavior
		-NPC Blueprint Inventory
			*As long as the item names listed in the inventory actually exist in the ItemFactory lists, then there is a random chance that
an NPC with that item defined in it's inventory will spawn with it; if its an equipment item, it will automatically be equipped.
	-> Inventory
		- There is an inventory class that keeps track of both equipped and regular inventory items for a given agent.
		- Items in the inventory that are equipped still take up space in the inventory (they will just not be listed under inventory)
		- Player can open up the inventory with the I button, and close it with Return
		- All Agents (Player and NPC alike) have an inventory.
	-> Features
		- Framework exists, can create data driven features and define a glyph for them.
		- Doors appear into the world via the dungeon map generator IF its told to generate doors (see TestRoom XML for an example).
		- Player can toggle on and off features with the A button.
	-> New terrain Types
		- To see the new tile types, please use map 6, aka TestRoom.
		- Flying Saves against slow and hurting effects, unless they have to do with going through walls.
		- Player, by default is hurt by fire, slowed by water, and blocked by walls and not flying.
		- Water Tiles exist
			* is just a blue '~'
			* entities passing through the water that are slowed by the water will get their movement cost multiplied by 2.
		- Lava Tiles exist
			* is a red '~'
			* entities passing through the lava that are slowed by the lava will get their movement cost multiplied by 2.
			* Entities that step onto Lava tiles, that can get hurt by fire, will get hurt by the fire (for 5 damage).
	-> Updated Systems
		- Pathfinding
			* Entities, if blocked by certain types of tiles will not enter them.
			* Entities, if slowed or hurt by certain types of tiles, should be reluctant to enter them.
		- Field of View
			* now checks to see if a feature is blocking line of sight.
			* now keeps track of what features are visible to agents and updates them based on that.
	-> Cohesive Feel
		*If number of NPCs + Player Count = less than initial number of npcs spawned in plus player; then every 5 turns the game should spawn in one npc randomly.
		* Some NPCs spawn with items in their inventory which they will automatically equip if it is equipable.
	-> Extra Credit
		* Turn order is saved.

__________________________________________________________
FINAL
__________________________________________________________
	- Castle Generator
		* Generates the entrance, moat, castle, gatehouse, Towers, and Rooms consistently. (Should be scaleable in size, but due to time constraints, may not be thoroughly tested).
		* Note, there is a solid line of air tiles from the entrance to the gatehouse.
		* specific Enemy placement is not possible due to how my generators are set up.
	- NPCs
		* Increased cap number of spawns to 50 in hopes that randomization will spawn the specific entities wished for.
		*Commented out all other blueprints outside of the scaredy cat NPC and the ones created for this final.
		* Reasoning behind keeping the scaredy cat NPC:
			-> it only wanders and runs away in fear; its also really weak.
		* Fire Eel Exists
		* Dark Elf Wizard exists
		* Dark Elf Warrior exists (and has an easter egg pointing towards an old WoW commercial)
		* Dwarf Soldier exists.
		* Note also that lower case letters are 1 off from where they are suppose to be right now. So in the xml sheet, the glyphs will be set to the letter before the one intended.
	- Behaviors
		* Wander
			* If Entity Rests, then will regain set amount of HP
		* Chase
			* has it able to allow the chase behavior to define someone to chase.
		* The new behaviors - Ran out of time; was starting on UseSiegeWeapon just as everything else got called.
			* New behaviors refers to UseSiegeWeapon and PsychicBlast
			* UseSiegeWeapon will spew out a bunch of warnings saying certain variables weren't used; ignore these.
	- Player
		*Press G to toggle God mode.
		*Press Period to rest and regain one hp, if currently below max health.
		*Player is represented by the @ char.
		*Since made the ring of levitation work, not going to make player blocked from moving on lava tiles,
instead just going to give player ability to move over them and get hurt (won't get hurt if levitating).
	- Features
		* Siege Weapons exist
		* Right now only the player can interact with it. (It just displays a message that says that player shot into the lava)
		* Ran out of time to implement UseSiegeWeapon behavior
		* The variables are there for being able to create UseSiegeWeapon.
	- Items
		*Ring of levitation actually makes wearer levitate.
		* I set the ring of levitation to be a darker color than white, as it was blending into the floor.