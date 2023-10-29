#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CustomListView.hpp>
#include <UIBuilder.hpp>

#include <random>

using namespace geode::prelude;

// https://crypto.stackexchange.com/a/16231
uint32_t levelHash(uint32_t x) {
    int n = 12;
    do // repeat for n from 12 downto 1
        x = ((x>>8)^x)*0x6B+n;
    while( --n!=0 );
    return x % 100;
}

char const* pronounList[] = {
    "He/Him",
    "She/Her",
    "They/Them",
    "It/Its",
    "He/They",
    "She/They",
	"Any/All"
};

char const* neopronounList[] = {
    "Ze/Hir",
    "Ze/Zir",
    "Xe/Xem",
    "E/Eir",
    "Ey/Em",
    "Per/Per",
    "Fae/Faer"
};

char const* getPronoun(int levelID) {
	size_t index = levelHash(levelID) % (sizeof(pronounList) / sizeof(char*));

	// Due to the fact that neo-pronouns are less common, we'll make them less likely to appear.
	if (levelHash(levelID) < 20 && Mod::get()->getSettingValue<bool>("neopronouns"))
		return neopronounList[index];
	else
		return pronounList[index];
};

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level) {
		if (!LevelInfoLayer::init(level))
			return false;

		Build<CCLabelBMFont>::create(getPronoun(level->m_levelID), "bigFont.fnt")
			.scale(0.4)
			.center()
			.posY(CCDirector::sharedDirector()->getWinSize().height - 36)
			.id("pronoun-label")
			.parent(this);

		// Move "By <username>" down a bit
		Build(this->getChildByIDRecursive("creator-name")).move(0, -7);

		return true;
	}
};

class $modify(CustomListView) {
	void loadCell(TableViewCell* tvc, int i) {
		CustomListView::loadCell(tvc, i);

		if (auto lc = typeinfo_cast<LevelCell*>(tvc)) {
			auto pronoun = fmt::format("({})", getPronoun(lc->m_level->m_levelID));

			Build<CCLabelBMFont>::create(pronoun.c_str(), "chatFont.fnt")
				.scale(0.5)
				.color(0, 0, 0)
				.opacity(100)
				.pos(lc->m_backgroundLayer->getContentSize() - ccp(10, 3))
				.anchorPoint({1, 1})
				.id("pronoun-label")
				.parent(lc->m_mainLayer);
		}
	}
};
