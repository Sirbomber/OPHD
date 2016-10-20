// ==================================================================================
// = This file implements the UI and UI Event Handlers used by GameState. I separated
// = it into its own file because the GameState.cpp file was starting to get a little
// = out of control.
// ==================================================================================

#include "GameState.h"
#include "GameStateHelper.h"

#include "PlanetSelectState.h"

#include "../Constants.h"

#include "../RobotTranslator.h"
#include "../StructureTranslator.h"

using namespace constants;

NAS2D::Rectangle_2d	BOTTOM_UI_AREA;

/**
* Sets up the user interface elements
*/
void GameState::initUi()
{
	EventHandler& e = Utility<EventHandler>::get();
	Renderer& r = Utility<Renderer>::get();

	mDiggerDirection.directionSelected().Connect(this, &GameState::diggerSelectionDialog);
	mDiggerDirection.hide();
	mDiggerDirection.position(r.screenCenterX() - mDiggerDirection.width() / 2, r.height() / 2 - 125);

	mTileInspector.position(r.screenCenterX() - mTileInspector.width() / 2, r.height() / 2 - 175);
	mTileInspector.hide();

	mStructureInspector.position(r.screenCenterX() - mStructureInspector.width() / 2, r.height() / 2 - 175);
	mStructureInspector.hide();

	mFactoryProduction.position(r.screenCenterX() - mFactoryProduction.width() / 2, 175);
	mFactoryProduction.hide();

	mFileIoDialog.position(r.screenCenterX() - mFileIoDialog.width() / 2, 50);
	mFileIoDialog.setMode(FileIo::FILE_SAVE);
	mFileIoDialog.fileOperation().Connect(this, &GameState::fileIoAction);
	mFileIoDialog.anchored(true);
	mFileIoDialog.hide();

	mPopulationPanel.position(580, constants::RESOURCE_ICON_SIZE + 4 + constants::MARGIN_TIGHT);
	mPopulationPanel.font(mTinyFont);
	mPopulationPanel.population(&mPopulation);
	mPopulationPanel.morale(&mCurrentMorale);
	mPopulationPanel.old_morale(&mPreviousMorale);

	mGameOverDialog.position(r.screenCenterX() - mGameOverDialog.width() / 2, r.screenCenterY() - mGameOverDialog.height() / 2 - 100);
	mGameOverDialog.returnToMainMenu().Connect(this, &GameState::btnGameOverClicked);
	mGameOverDialog.hide();

	mAnnouncement.position(r.screenCenterX() - mGameOverDialog.width() / 2, r.screenCenterY() - mGameOverDialog.height() / 2 - 100);
	mAnnouncement.hide();

	mWindowStack.addWindow(&mTileInspector);
	mWindowStack.addWindow(&mStructureInspector);
	mWindowStack.addWindow(&mFactoryProduction);
	mWindowStack.addWindow(&mDiggerDirection);
	mWindowStack.addWindow(&mAnnouncement);


	// Bottom UI
	BOTTOM_UI_AREA(0, r.height() - constants::BOTTOM_UI_HEIGHT, r.width(), constants::BOTTOM_UI_HEIGHT);

	mResourceInfoBox(mMiniMapBoundingBox.x() + mMiniMapBoundingBox.w() + constants::MARGIN * 2 + constants::MINI_MAP_BUTTON_SIZE, mMiniMapBoundingBox.y(), constants::RESOURCE_BOX_WIDTH, mMiniMapBoundingBox.h());

	// X Position of the main UI buttons
	int posX = r.width() - 34;

	// Mini Map
	mMiniMapBoundingBox(r.width() - mMapDisplay.width() - constants::MARGIN, BOTTOM_UI_AREA.y() + constants::MARGIN, mMapDisplay.width(), mMapDisplay.height());

	// BUTTONS
	// System
	mBtnTurns.image("ui/icons/turns.png");
	mBtnTurns.size(constants::MAIN_BUTTON_SIZE);
	mBtnTurns.position(mMiniMapBoundingBox.x() - constants::MAIN_BUTTON_SIZE - constants::MARGIN_TIGHT, r.height() - constants::MARGIN - MAIN_BUTTON_SIZE);
	mBtnTurns.click().Connect(this, &GameState::btnTurnsClicked);
	mBtnTurns.enabled(false);

	mBtnToggleHeightmap.image("ui/icons/height.png");
	mBtnToggleHeightmap.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleHeightmap.position(mBtnTurns.positionX(), mMiniMapBoundingBox.y());
	mBtnToggleHeightmap.type(Button::BUTTON_TOGGLE);

	mBtnToggleConnectedness.image("ui/icons/connection.png");
	mBtnToggleConnectedness.size(constants::MAIN_BUTTON_SIZE);
	mBtnToggleConnectedness.position(mBtnTurns.positionX(), mMiniMapBoundingBox.y() + constants::MAIN_BUTTON_SIZE + constants::MARGIN_TIGHT);
	mBtnToggleConnectedness.type(Button::BUTTON_TOGGLE);
	mBtnToggleConnectedness.click().Connect(this, &GameState::btnToggleConnectednessClicked);

	// Menus
	mRobots.font(mTinyFont);
	mRobots.sheetPath("ui/robots.png");
	mRobots.position(mBtnTurns.positionX() - constants::MARGIN_TIGHT - 52, BOTTOM_UI_AREA.y() + MARGIN);
	mRobots.size(52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mRobots.iconSize(46);
	mRobots.iconMargin(constants::MARGIN_TIGHT);
	mRobots.showTooltip(true);
	mRobots.selectionChanged().Connect(this, &GameState::robotsSelectionChanged);

	mConnections.font(mTinyFont);
	mConnections.sheetPath("ui/structures.png");
	mConnections.position(mRobots.positionX() - constants::MARGIN_TIGHT - 52, BOTTOM_UI_AREA.y() + MARGIN);
	mConnections.size(52, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mConnections.iconSize(46);
	mConnections.iconMargin(constants::MARGIN_TIGHT);
	mConnections.selectionChanged().Connect(this, &GameState::connectionsSelectionChanged);
	mConnections.sorted(false);

	mStructures.font(mTinyFont);
	mStructures.sheetPath("ui/structures.png");
	mStructures.position(constants::MARGIN , BOTTOM_UI_AREA.y() + MARGIN);
	mStructures.size( mConnections.positionX() -  constants::MARGIN - constants::MARGIN_TIGHT, BOTTOM_UI_HEIGHT - constants::MARGIN * 2);
	mStructures.iconSize(46);
	mStructures.iconMargin(constants::MARGIN_TIGHT);
	mStructures.showTooltip(true);
	mStructures.selectionChanged().Connect(this, &GameState::structuresSelectionChanged);
	

	// Initial Structures
	mStructures.addItem(constants::SEED_LANDER, 0);
}


/**
 * Hides ALL UI elements.
 */
void GameState::hideUi()
{
	mBtnTurns;

	mBtnToggleHeightmap.hide();
	mBtnToggleConnectedness.hide();

	mStructures.hide();
	mRobots.hide();
	mConnections.hide();

	mWindowStack.hide();
}


/**
 * Hides all non-essential UI elements.
 */
void GameState::resetUi()
{
	clearMode();

	clearSelections();

	mDiggerDirection.hide();
	mFactoryProduction.hide();
	mStructureInspector.hide();
	mTileInspector.hide();
}


void GameState::clearSelections()
{
	mStructures.clearSelection();
	mConnections.clearSelection();
	mRobots.clearSelection();
}


/**
* Adds selection options to the Structure Menu
*/
void GameState::populateStructureMenu()
{
	mStructures.dropAllItems();
	mConnections.dropAllItems();

	// Above Ground structures only
	if (mStructureManager.count() == 0)
	{
		if(mTileMap->currentDepth() == constants::DEPTH_SURFACE)
			mStructures.addItem(constants::SEED_LANDER, 0);
	}
	else if (mTileMap->currentDepth() == constants::DEPTH_SURFACE)
	{
		mStructures.addItem(constants::AGRIDOME, 5);
		mStructures.addItem(constants::CHAP, 3);
		mStructures.addItem(constants::FUSION_REACTOR, 21);
		mStructures.addItem(constants::HOT_LABORATORY, 18);
		mStructures.addItem(constants::ROBOT_COMMAND, 14);
		mStructures.addItem(constants::SURFACE_POLICE, 23);
		mStructures.addItem(constants::SMELTER, 4);
		mStructures.addItem(constants::SOLAR_PLANT, 10);
		mStructures.addItem(constants::STORAGE_TANKS, 8);
		mStructures.addItem(constants::SURFACE_FACTORY, 11);
		mStructures.addItem(constants::WAREHOUSE, 9);

		mConnections.addItem(constants::AG_TUBE_INTERSECTION, 110);
		mConnections.addItem(constants::AG_TUBE_RIGHT, 112);
		mConnections.addItem(constants::AG_TUBE_LEFT, 111);


		// Special case code, not thrilled with this
		if(mLandersColonist > 0)
			mStructures.addItem(constants::COLONIST_LANDER, 2);
	}
	else
	{
		mStructures.addItem(constants::LABORATORY, 58);
		mStructures.addItem(constants::PARK, 75);
		mStructures.addItem(constants::UNDERGROUND_POLICE, 61);
		mStructures.addItem(constants::RECREATION_CENTER, 73);
		mStructures.addItem(constants::RESIDENCE, 55);
		mStructures.addItem(constants::UNDERGROUND_FACTORY, 69);
		mStructures.addItem(constants::COMMERCIAL, 66);
		mStructures.addItem(constants::RED_LIGHT_DISTRICT, 76);
		mStructures.addItem(constants::UNIVERSITY, 63);

		mConnections.addItem(constants::UG_TUBE_INTERSECTION, 113);
		mConnections.addItem(constants::UG_TUBE_RIGHT, 115);
		mConnections.addItem(constants::UG_TUBE_LEFT, 114);
	}
}


/**
* Updates and draws the UI.
*/
void GameState::drawUI()
{
	Renderer& r = Utility<Renderer>::get();

	// Bottom UI
	r.drawBoxFilled(BOTTOM_UI_AREA, 39, 39, 39);
	r.drawBox(BOTTOM_UI_AREA, 21, 21, 21);
	r.drawLine(BOTTOM_UI_AREA.x() + 1, BOTTOM_UI_AREA.y(), BOTTOM_UI_AREA.x() + BOTTOM_UI_AREA.w() - 2, BOTTOM_UI_AREA.y(), 56, 56, 56);

	drawMiniMap();
	drawResourceInfo();
	drawNavInfo();
	drawRobotInfo();

	// Buttons
	mBtnToggleHeightmap.update();
	mBtnToggleConnectedness.update();

	mBtnTurns.update();
	
	// Menus
	mRobots.update();
	mStructures.update();
	mConnections.update();

	// Windows
	mFileIoDialog.update();
	mWindowStack.update();

	// Always draw last
	mPointers[mCurrentPointer].draw(mMousePosition.x(), mMousePosition.y());
}


/**
 * Handles clicks of the Connectedness Overlay button.
 */
void GameState::btnToggleConnectednessClicked()
{
	mTileMap->toggleShowConnections();
}


/**
* Currently uses a text comparison function. Not inherently bad but
* should really be turned into a key/value pair table for easier lookups.
*/
void GameState::structuresSelectionChanged(const std::string& _s)
{
	// clear the others selectors 
	mConnections.clearSelection();
	mRobots.clearSelection();

	// set the new structure Id
	setStructureID(StructureTranslator::translateFromString(_s), INSERT_STRUCTURE);
}


/**
* Handler for the Tubes Pallette dialog.
*/
void GameState::connectionsSelectionChanged(const std::string& _s)
{
	// clear the others selectors 
	mRobots.clearSelection();
	mStructures.clearSelection();
	
	// set the new structure Id
	setStructureID(SID_TUBE, INSERT_TUBE);
}


/**
* Handles clicks of the Robot Selection Menu.
*/
void GameState::robotsSelectionChanged(const std::string& _s)
{
	// clear the others selectors
	mConnections.clearSelection();
	mStructures.clearSelection();

	mCurrentRobot = RobotTranslator::translateFromString(_s);

	if(mCurrentRobot == ROBOT_NONE)
	{
		clearMode();
		return;
	}

	mInsertMode = INSERT_ROBOT;
	mCurrentPointer = POINTER_PLACE_TILE;
}


void GameState::diggerSelectionDialog(DiggerDirection::DiggerSelection _sel, Tile* _t)
{
	// Don't dig beyond the dig depth of the planet.
	if (mTileMap->currentDepth() == mTileMap->maxDepth() && _sel == DiggerDirection::SEL_DOWN)
	{
		mAiVoiceNotifier.notify(AiVoiceNotifier::MAX_DIGGING_DEPTH_REACHED);
		cout << "GameState::diggerSelectionDialog(): Already at the maximum digging depth." << endl;
		return;
	}

	// Before doing anything, if we're going down and the depth is not the surface,
	// the assumption is that we've already checked and determined that there's an air shaft
	// so clear it from the tile, disconnect the tile and run a connectedness search.
	if (_t->depth() > 0 && _sel == DiggerDirection::SEL_DOWN)
	{
		mStructureManager.removeStructure(_t->structure());
		mStructureManager.disconnectAll();
		_t->deleteThing();
		_t->connected(false);
		checkConnectedness();
	}

	// Assumes a digger is available.
	Robodigger* r = mRobotPool.getDigger();
	r->startTask(_t->index() + 5); // FIXME: Magic Number
	insertRobotIntoTable(mRobotList, r, _t);


	if (_sel == DiggerDirection::SEL_DOWN)
	{
		r->direction(DIR_DOWN);
	}
	else if (_sel == DiggerDirection::SEL_NORTH)
	{
		r->direction(DIR_NORTH);
		mTileMap->getTile(_t->x(), _t->y() - 1, _t->depth())->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_SOUTH)
	{
		r->direction(DIR_SOUTH);
		mTileMap->getTile(_t->x(), _t->y() + 1, _t->depth())->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_EAST)
	{
		r->direction(DIR_EAST);
		mTileMap->getTile(_t->x() + 1, _t->y(), _t->depth())->excavated(true);
	}
	else if (_sel == DiggerDirection::SEL_WEST)
	{
		r->direction(DIR_WEST);
		mTileMap->getTile(_t->x() - 1, _t->y(), _t->depth())->excavated(true);
	}


	if (!mRobotPool.robotAvailable(ROBOT_DIGGER))
	{
		mRobots.removeItem(constants::ROBODIGGER);
		clearMode();
	}

	mDiggerDirection.visible(false);
}


void GameState::btnGameOverClicked()
{
	mReturnState = new PlanetSelectState();
	Utility<Renderer>::get().fadeOut(constants::FADE_SPEED);
}


void GameState::fileIoAction(const std::string& _file, FileIo::FileOperation _op)
{
	_op == FileIo::FILE_LOAD ? load(constants::SAVE_GAME_PATH + _file + ".xml") : save(constants::SAVE_GAME_PATH + _file + ".xml");

	mFileIoDialog.hide();
}


int moraleChange(StructureManager& _sm, Structure::StructureType _type)
{
	int count = 0;
	for (auto it = _sm.structureList(_type).begin(); it != _sm.structureList(_type).end(); ++it)
		if ((*it)->operational())
			++count;
		else if ((*it)->disabled())
			--count;

	return count;
}


int pullFood(ResourcePool& _rp, int amount)
{
	if (amount <= _rp.food())
	{
		_rp.food(_rp.food() - amount);
		return amount;
	}
	else
	{
		int ret = _rp.food();
		_rp.food(0);
		return ret;
	}
}


/**
 * Turns button clicked.
 */
void GameState::btnTurnsClicked()
{
	clearMode();

	mStructureManager.disconnectAll();
	checkConnectedness();
	mStructureManager.update(mPlayerResources);

	mPreviousMorale = mCurrentMorale;

	// FOOD CONSUMPTION
	int food_consumed = mPopulation.update(mCurrentMorale, foodInStorage());
	StructureManager::StructureList &foodproducers = mStructureManager.structureList(Structure::STRUCTURE_FOOD_PRODUCTION);
	int remainder = food_consumed;	
	
	if (mPlayerResources.food() > 0)
		remainder -= pullFood(mPlayerResources, remainder);
	
	for (size_t i = 0; i < foodproducers.size(); ++i)
	{
		if (remainder <= 0)
			break;

		remainder -= pullFood(foodproducers[i]->storage(), remainder);
	}
	
	// MORALE
	// Positive Effects
	mCurrentMorale += mPopulation.birthCount();
	mCurrentMorale += mStructureManager.getCountInState(Structure::STRUCTURE_PARK, Structure::OPERATIONAL);
	mCurrentMorale += mStructureManager.getCountInState(Structure::STRUCTURE_RECREATION_CENTER, Structure::OPERATIONAL);

	int food_production = mStructureManager.getCountInState(Structure::STRUCTURE_FOOD_PRODUCTION, Structure::OPERATIONAL);
	food_production > 0 ? mCurrentMorale += food_production : mCurrentMorale -= 5;

	//mCurrentMorale += mStructureManager.getCountInState(Structure::STRUCTURE_COMMERCIAL, Structure::OPERATIONAL);

	// Negative Effects
	mCurrentMorale -= mPopulation.deathCount();
	mCurrentMorale -= mStructureManager.disabled();
	mCurrentMorale -= mStructureManager.destroyed();

	mCurrentMorale = clamp(mCurrentMorale, 0, 1000);


	// Update storage capacity
	mPlayerResources.capacity(totalStorage(mStructureManager.structureList(Structure::STRUCTURE_STORAGE)));
	
	ResourcePool truck;
	truck.capacity(100);

	auto mines = mStructureManager.structureList(Structure::STRUCTURE_MINE);
	auto smelters = mStructureManager.structureList(Structure::STRUCTURE_SMELTER);

	// Move ore from mines to smelters
	for (size_t m = 0; m < mines.size(); ++m)
	{
		if (mines[m]->disabled() || mines[m]->destroyed())
			continue; // consider a different control path.

		truck.commonMetalsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_COMMON_METALS_ORE, 25));
		truck.commonMineralsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_COMMON_MINERALS_ORE, 25));
		truck.rareMetalsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_RARE_METALS_ORE, 25));
		truck.rareMineralsOre(mines[m]->storage().pullResource(ResourcePool::RESOURCE_RARE_MINERALS_ORE, 25));

		for (size_t s = 0; s < smelters.size(); ++s)
		{
			if(smelters[s]->operational())
				smelters[s]->production().pushResources(truck);
		}

		if (!truck.empty())
			mines[m]->storage().pushResources(truck);
	}

	// Move refined resources from smelters to storage tanks
	for (size_t s = 0; s < smelters.size(); ++s)
	{
		if (smelters[s]->disabled() || smelters[s]->destroyed())
			continue; // consider a different control path.

		truck.commonMetals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_COMMON_METALS, 25));
		truck.commonMinerals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_COMMON_MINERALS, 25));
		truck.rareMetals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_RARE_METALS, 25));
		truck.rareMinerals(smelters[s]->storage().pullResource(ResourcePool::RESOURCE_RARE_MINERALS, 25));

		mPlayerResources.pushResources(truck);

		if (!truck.empty())
		{
			smelters[s]->storage().pushResources(truck);
			break;	// we're at max capacity in our storage, dump what's left in the smelter it came from and barf.
		}
	}

	updateRobots();

	Structure* cc = mTileMap->getTile(mCCLocation.x(), mCCLocation.y(), TileMap::LEVEL_SURFACE)->structure();
	if (cc->state() == Structure::OPERATIONAL)
	{
		populateStructureMenu();
	}

	mTurnCount++;


	// Check for colony ship deorbiting; if any colonists are remaining, kill
	// them and reduce morale by an appropriate amount.
	if (mTurnCount == COLONY_SHIP_ORBIT_TIME)
	{
		if (mLandersColonist > 0)
		{
			mCurrentMorale -= (mLandersColonist * 50) * 6; // TODO: apply a modifier to multiplier based on difficulty level.
			if (mCurrentMorale < 0)
				mCurrentMorale = 0;

			mLandersColonist = 0;

			populateStructureMenu();

			mWindowStack.bringToFront(&mAnnouncement);
			mAnnouncement.announcement(MajorEventAnnouncement::ANNOUNCEMENT_COLONY_SHIP_CRASH_WITH_COLONISTS);
			mAnnouncement.show();
		}
		else
		{
			mWindowStack.bringToFront(&mAnnouncement);
			mAnnouncement.announcement(MajorEventAnnouncement::ANNOUNCEMENT_COLONY_SHIP_CRASH);
			mAnnouncement.show();
		}
	}


	// Check for Game Over conditions
	if (mPopulation.size() < 1 && mLandersColonist == 0)
	{
		hideUi();
		mGameOverDialog.show();
	}
}
