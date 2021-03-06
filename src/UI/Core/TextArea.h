#pragma once

#include "Control.h"

#include "NAS2D/NAS2D.h"

#include <string>


class TextArea : public Control
{
public:
	TextArea();
	virtual ~TextArea();

	void textColor(int r, int g, int b, int a = 255) { mTextColor(r, g, b, a); }

	void font(const std::string&, size_t);

	void update();

private:
	typedef std::vector<std::string> StringList;

private:
	virtual void onSizeChanged();
	virtual void onTextChanged();
	virtual void onFontChanged();

	virtual void draw();
	void processString();

private:
	size_t		mNumLines = 0;

	StringList	mFormattedList;

	NAS2D::Color_4ub	mTextColor = NAS2D::COLOR_WHITE;

	NAS2D::Font*		mFont = nullptr;
};
