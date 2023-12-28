#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

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
	bool hasNeopronouns = Mod::get()->getSettingValue<bool>("neopronouns");
	if (levelHash(levelID) < 20 && hasNeopronouns)
		return neopronounList[index];
	else
		return pronounList[index];
};


class $modify( LevelInfoLayer ) {
 	bool init( GJGameLevel* p0, bool p1 ) {
		if ( !LevelInfoLayer::init( p0, p1 ) )
			return false;
		
		CCLabelBMFont* label = CCLabelBMFont::create( getPronoun( p0->m_levelID ), "bigFont.fnt" );
		label->setScale( 0.4 );
		label->setAnchorPoint( { 0.5, 0.5 } );
		label->setPosition( { CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height - 36 } );
		this->addChild( label, 100 );


		// For each all children and find the one with 7 or more children
		auto children = CCArrayExt<CCNode>(this->getChildren());
		for (auto child : children) {
			if (auto menu = typeinfo_cast<CCMenu*>(child)) {
				if ( menu->getChildrenCount() >= 7 ) {
					// Move "By <username>" down a bit
					auto author = as<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(0));
					author->setPosition( { author->getPositionX(), author->getPositionY() - 7 } );
				}
			}
		}

		return true;
	} 
};
