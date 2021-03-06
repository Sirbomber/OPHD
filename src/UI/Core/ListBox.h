#pragma once

#include "NAS2D/NAS2D.h"

#include <map>
#include <string>
#include <algorithm>

#include "UIContainer.h"
#include "Slider.h"

#include "../../Constants/UiConstants.h"

/**
 * Implements a ListBox control.
 */
class ListBox: public UIContainer
{
public:
	typedef NAS2D::Signals::Signal0<void> SelectionChangedCallback;

	struct ListBoxItem
	{
		ListBoxItem(const std::string& text, int tag) : Text(text), Tag(tag) {}

		bool operator==(const std::string& rhs) { return Text == rhs; }
		bool operator<(const ListBoxItem& lhs) { return Text < lhs.Text; }

		std::string		Text;			/**< Text of the ListBoxItem. */
		int				Tag = 0;		/**< User defined int data attached to the item. */
	};

	using ListBoxItems = std::vector<ListBoxItem>;

public:
	ListBox();
	virtual ~ListBox();
	
	void sorted(bool _b) { mSorted = _b; }
	bool sorted(bool) const { return mSorted; }

	void sort() { if (mSorted) { std::sort(mItems.begin(), mItems.end()); } }

	void textColor(const NAS2D::Color_4ub& color)	{ mText = color; }
	void selectColor(const NAS2D::Color_4ub& color)	{ mHighlightBg = color; }

	void addItem(const std::string& item, int tag = 0);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();

	int count() const { return mItems.size(); }
	int lineHeight() const { return mLineHeight; }

	void setSelectionByName(const std::string& item);

	int currentSelection() const { return mCurrentSelection; }
	void currentSelection(int selection) { mCurrentSelection = selection; mSelectionChanged(); }
	void clearSelection() { mCurrentSelection = constants::NO_SELECTION; }

	int currentHighlight() const { return mCurrentHighlight; }

	const std::string& selectionText() const;
	int selectionTag() const;

	void update();

	bool empty() const;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);
	void onMouseWheel(int x, int y);
	virtual void slideChanged(double _position);

	virtual void visibilityChanged(bool visible) final;

private:
	virtual void onSizeChanged() final;

	void _updateItemDisplay();
	void _init();

private:
	int							mCurrentHighlight = constants::NO_SELECTION;	/**< Currently highlighted selection index. */
	int							mCurrentSelection = 0;							/**< Current selection index. */
	int							mCurrentOffset = 0;								/**< Current selection index. */

	int							mItemWidth = 0;									/**< Width of items. */
	int							mLineHeight = 0;								/**< Height of an item line. */
	int							mLineCount = 0;									/**< Number of lines that can be displayed. */

	ListBoxItems				mItems;											/**< List of items preserved in the order in which they're added. */

	NAS2D::Color_4ub			mText = NAS2D::COLOR_WHITE;						/**< Text Color */
	NAS2D::Color_4ub			mHighlightBg = NAS2D::COLOR_GREEN;				/**< Highlight Background color. */
	NAS2D::Color_4ub			mHighlightText = NAS2D::COLOR_WHITE;			/**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback	mSelectionChanged;								/**< Callback for selection changed callback. */
	Slider						mSlider;										/**<  */
	
	bool						mSorted = false;								/**< Flag indicating that all Items should be sorted. */
};
