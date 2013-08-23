// Copyright (c) 2013- PPSSPP Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0 or later versions.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official git repository and contact information can be found at
// https://github.com/hrydgard/ppsspp and http://www.ppsspp.org/.

#include "base/functional.h"
#include "ui/view.h"
#include "ui/ui_screen.h"
#include "ui/ui_context.h"
#include "../Core/CwCheat.h"
#include "UI/MiscScreens.h"
using namespace UI;

class CwCheatScreen : public UIDialogScreenWithBackground {
public:
	CwCheatScreen() {}
	std::vector<std::string> CreateCodeList();
	std::ifstream is;
	std::ofstream os;
	void processFileOn(std::string activatedCheat);
	void processFileOff(std::string deactivatedCheat);
	const char * name;
	std::string activatedCheat, deactivatedCheat;
protected:
	virtual void CreateViews();

private:
	UI::EventReturn OnCheckBox(UI::EventParams &params);
	bool enableCheat [64];
	std::vector<std::string> cheatList, formattedList;
	std::vector<int> locations;
	
};

class CheatCheckBox : public ClickableItem, public CwCheatScreen {
public:
	CheatCheckBox(bool *toggle, const std::string &text, const std::string &smallText = "", LayoutParams *layoutParams = 0)
		: ClickableItem(layoutParams), toggle_(toggle), text_(text) {
			OnClick.Handle(this, &CheatCheckBox::OnClicked);
	}

	virtual void Draw(UIContext &dc);

	EventReturn OnClicked(EventParams &e) {
		if (toggle_) {
			*toggle_ = !(*toggle_);
		}
		if (*toggle_ == true)
		{
			activatedCheat = text_;
			processFileOn(activatedCheat);
		}
		if (*toggle_ == false)
		{
			deactivatedCheat = text_;
			processFileOff(deactivatedCheat);
		}
		return EVENT_DONE;
	}
private:
	bool *toggle_;
	std::string text_;
	std::string smallText_;

};