#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>

#include <random>

using namespace geode::prelude;

int randomSeededNumber(int levelID) {
  std::mt19937 rng(levelID);
  std::uniform_int_distribution<int> random(0, 100);

  return random(rng);
}

constexpr std::array<std::string, 5> pronounList = {
    "He/Him",
    "She/Her",
    "They/Them",
    "It/Its",
	"Any/All"
};

constexpr std::array<std::string, 4> neopronounList = {
    "Ze/Hir",
    "Xe/Xem",
    "E/Eir",
    "Per/Per",
};

constexpr std::string getPronoun(int levelID) {
	// Due to the fact that neo-pronouns are less common, we'll make them less likely to appear.
	if (randomSeededNumber(levelID) < 20 && Mod::get()->getSettingValue<bool>("neopronouns")) {
		return neopronounList[randomSeededNumber(levelID) % neopronounList.size()];
	}

	return pronounList[randomSeededNumber(levelID) % pronounList.size()];
};


Result<CCLabelBMFont*> getLabelFromText(CCNode* node, std::string text) {
	auto children = CCArrayExt<CCNode*>(node->getChildren());
	for (auto child : children) {
		if (auto label = safe_cast<CCLabelBMFont*>(child)) {
			auto labelString = std::string(label->getString());
			if (!labelString.compare(text)) {
				return Ok(label);
			}
		}
	}
	return Err("Label not found");
}

class $modify(LevelInfoLayer) {
	bool init(GJGameLevel* level) {
		if (!LevelInfoLayer::init(level)) {
			return false;
		}
		auto levelNameResult = getLabelFromText(this, level->m_levelName);
		
		if (levelNameResult.isErr()) {
			return false;
		}

		auto levelNameLabel = levelNameResult.unwrap();

		std::string pronoun = getPronoun(level->m_levelID);
		
		auto pronounLabel = CCLabelBMFont::create(pronoun.c_str(), "bigFont.fnt");
		pronounLabel->setScale(0.4);
		pronounLabel->setPosition(levelNameLabel->getPosition() + ccp(0, -levelNameLabel->getScaledContentSize().height / 2 - 4));
		pronounLabel->setID("pronoun-label");
		this->addChild(pronounLabel);

		// Move "By <username>" down a bit
		auto creatorLabel = this->getChildByID("creator-info-menu")->getChildByID("creator-name");
		creatorLabel->setPosition(creatorLabel->getPosition() + ccp(0, -7));

		return true;
	}
};


class $modify(LevelCell) {
	void loadFromLevel(GJGameLevel* level) {
		// call original method
		LevelCell::loadFromLevel(level);

		std::string pronoun = getPronoun(level->m_levelID);

		auto pronounLabel = CCLabelBMFont::create(std::format("({})", pronoun).c_str(), "chatFont.fnt");

		pronounLabel->setScale(0.3);
		pronounLabel->setColor({ 0, 0, 0 });
		pronounLabel->setOpacity(100);
		pronounLabel->setPosition(this->m_backgroundLayer->getContentSize() - pronounLabel->getScaledContentSize());
		pronounLabel->setID("pronoun-label");
		m_mainLayer->addChild(pronounLabel);
    }
};
