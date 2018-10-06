#pragma once

#include <NAS2D/NAS2D.h>

class MapViewState;

class GameState : public NAS2D::State
{

public:
	GameState();
	virtual ~GameState();

	void mapviewstate(MapViewState*);

	virtual void initialize() final;
	virtual State* update() final;

private:
	void onMouseMotion(int x, int y, int relX, int relY);
	void onMouseDown(NAS2D::EventHandler::MouseButton, int x, int y);
	void onMouseUp(NAS2D::EventHandler::MouseButton, int x, int y);

	void fadeComplete();
	void musicComplete();

	void quitEvent();
	void showReportsUi();
	void hideReportsUi();

private:
	NAS2D::State*	mReturnState = this;
};
