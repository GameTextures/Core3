/*
Copyright (C) 2007 <SWGEmu>
 
This File is part of Core3.
 
This program is free software; you can redistribute 
it and/or modify it under the terms of the GNU Lesser 
General Public License as published by the Free Software
Foundation; either version 2 of the License, 
or (at your option) any later version.
 
This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU Lesser General Public License for
more details.
 
You should have received a copy of the GNU Lesser General 
Public License along with this program; if not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 
Linking Engine3 statically or dynamically with other modules 
is making a combined work based on Engine3. 
Thus, the terms and conditions of the GNU Lesser General Public License 
cover the whole combination.
 
In addition, as a special exception, the copyright holders of Engine3 
give you permission to combine Engine3 program with free software 
programs or libraries that are released under the GNU LGPL and with 
code included in the standard release of Core3 under the GNU LGPL 
license (or modified versions of such code, with unchanged license). 
You may copy and distribute such a system following the terms of the 
GNU LGPL for Engine3 and the licenses of the other code concerned, 
provided that you include the source code of that other code when 
and as the GNU LGPL requires distribution of source code.
 
Note that people who make modified versions of Engine3 are not obligated 
to grant this special exception for their modified versions; 
it is their choice whether to do so. The GNU Lesser General Public License 
gives permission to release a modified version without this exception; 
this exception also makes it possible to release a modified version 
which carries forward this exception.
*/

#include "../../objects/player/Races.h"
#include "../../objects/terrain/Terrain.h"

#include "../skills/SkillManager.h"

#include "../../objects/creature/skills/SkillList.h"

#include "../../objects/player/PlayerImplementation.h"
#include "../../objects/player/PlayerObjectImplementation.h"

#include "../../objects/tangible/weapons/Weapon.h"
#include "../../objects/tangible/weapons/JediWeapon.h"

#include "../../../ServerCore.h"

#include "../../objects.h"

#include "../../Zone.h"

#include "PlayerManager.h"
#include "PlayerManagerImplementation.h"

#include "PlayerMapImplementation.h"

#include "../guild/GuildManager.h"
#include "../planet/PlanetManager.h"
#include "../../../chat/ChatManager.h"

#include "PlayerHAM.h"

PlayerManagerImplementation::PlayerManagerImplementation(ItemManager* mgr, ZoneProcessServerImplementation* srv) : PlayerManagerServant() {
	PlayerMapImplementation* mapImpl = new PlayerMapImplementation(3000);
	playerMap = (PlayerMap*) mapImpl->deploy("PlayerMap");
	
	itemManager = mgr;
	
	server = srv;
}

PlayerManagerImplementation::~PlayerManagerImplementation() {
	//delete playerMap;
}

bool PlayerManagerImplementation::create(Player* player, uint32 sessionkey) {	
	int accountID = sessionkey;
	int galaxyID = 2;
	
	string surname;
	
	if ((player->getFirstName().size() + 1) < player->getCharacterName().size())
			surname = player->getCharacterName().substring(player->getFirstName().size() + 1, player->getCharacterName().size()).c_str();
		else
			surname = "";
	
	player->setZoneIndex(8);

	player->initializePosition(96.0f, 0, -5334.0f);
		
	player->randomizePosition(128);
	
	int race = Races::getRaceID(player->getRaceFileName());
	int gender = 0;
	int creditsCash = 100000;
	int creditsBank = 100000;
	
	string bio = player->getBiography().c_str();
	string info = "";
	
	string apperance;
	BinaryData cust(player->getCharacterApperance());
	cust.encode(apperance);
	
	string hairdata;
	BinaryData hair(player->getHairData());
	hair.encode(hairdata);
	
	int hamValues[9];
	
	string prof = player->getStartingProfession();
	
	if (prof == "artisan")
		memcpy(hamValues, professionHams[0], sizeof(hamValues));
	else if (prof == "brawler")
		memcpy(hamValues, professionHams[1], sizeof(hamValues));
	else if (prof == "entertainer")
		memcpy(hamValues, professionHams[2], sizeof(hamValues));
	else if (prof == "marksman")
		memcpy(hamValues, professionHams[3], sizeof(hamValues));
	else if (prof == "medic")
		memcpy(hamValues, professionHams[4], sizeof(hamValues));
	else if (prof == "scout")
		memcpy(hamValues, professionHams[5], sizeof(hamValues));
	else {
		memcpy(hamValues, professionHams[6], sizeof(hamValues));
	}
	
	// Add the race mods
	int hamMods[9];
	memcpy(hamMods, raceHamMods[race % 10], sizeof(hamMods));
		
	for (int i = 0; i < 9; i++)
		hamValues[i] += hamMods[i];

	try {
		stringstream query;
    	query << "INSERT INTO `characters` "
        	  << "(`account_id`,`galaxy_id`,`firstname`,`surname`,"
	          << "`appearance`,`professions`,`race`,`gender`,`lots`,"
    	      << "`credits_inv`,`credits_bank`,`guild`,`x`,`y`,`z`,`zoneid`,`planet_id`,"
	          << "`lfg`,`helper`,`roleplayer`,`faction_id`,`archived`,`scale`,`biography`,"
	          << "`infofield`,`hair`,`hairData`,`playermodel`,`CRC`,`Title`,"
	          << "`health`,`strength`,`constitution`,`action`,`quickness`,"
	          << "`stamina`,`mind`,`focus`,`willpower`"
	          << ") VALUES ("
    	      << accountID << "," << galaxyID << ",'" 
	          << player->getFirstName() << "','" << surname << "','" 
	          << apperance.substr(0, apperance.size() - 1) << "','" 
	          << player->getStartingProfession() << "'," <<  race << "," << gender << ",10," 
    	      << creditsCash << "," << creditsBank << ",0," 
        	  << player->getPositionX() << "," << player->getPositionY() << "," 
	          << player->getPositionZ() << "," << player->getZoneIndex() << "," << 0//planetID 
    	      << ",0,0,0,0,0," << player->getHeight() << ","
    	      << "'" << bio << "','" << info << "','" 
    	      << player->getHairObject() << "','" << hairdata.substr(0, hairdata.size() - 1) << "','', '0','',"
    	      << hamValues[0] << "," << hamValues[1] << "," << hamValues[2] << "," 
    	      << hamValues[3] << "," << hamValues[4] << "," << hamValues[5] << ","
    	      << hamValues[6] << "," << hamValues[7] << "," << hamValues[8] << ")";
	
		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
	
		player->setCharacterID(res->getLastAffectedRow());
		
		playerMap->put(player->getFirstName(), player);
		
		delete res;
	} catch (DatabaseException& e) {
		return false;
	}

	return true;
}

bool PlayerManagerImplementation::validateName(string& cname) {
	if (cname.size() < 1)
		return false;
	
	string name = cname;
	String::toLower(name);
	try {
		string query = "SELECT * FROM characters WHERE lower(firstname) = \'" + name + "\'";

		ResultSet* res = ServerDatabase::instance()->executeQuery(query);
		bool nameExists = res->next();		
		
		delete res;
		
		bool valid = true;
		
		if (strchr(name.c_str(),'#') || strchr(name.c_str(),';') || strchr(name.c_str(),'!') || strchr(name.c_str(),'@') || strchr(name.c_str(),'$') || 
			strchr(name.c_str(),'%') || strchr(name.c_str(),'^') || strchr(name.c_str(),'&') || strchr(name.c_str(),'*') || strchr(name.c_str(),'{') || 
			strchr(name.c_str(),'}') || strchr(name.c_str(),'(') || strchr(name.c_str(),')') || strchr(name.c_str(),'+') || strchr(name.c_str(),'=') || 
			strchr(name.c_str(),'~') || strchr(name.c_str(),'`') || strchr(name.c_str(),'[') || strchr(name.c_str(),']') || strchr(name.c_str(),'/') 
			|| strchr(name.c_str(),'>') || strchr(name.c_str(),'<') || strchr(name.c_str(),'?') || strchr(name.c_str(),'/') || strchr(name.c_str(),'|') 
			|| strchr(name.c_str(),'`')) {
			valid = false;
		}

		return !nameExists && valid;
	} catch (DatabaseException& e) {
		return false;
	}
}

Player* PlayerManagerImplementation::load(uint64 charid) {
	PlayerImplementation* playerImpl = new PlayerImplementation(charid);
	playerImpl->setZoneProcessServer(server);
	
	Player* player = loadFromDatabase(playerImpl);
	
	if (player == NULL)
		return NULL;
			
	playerMap->put(player->getFirstName(), player);
	
	return player;
}

Player* PlayerManagerImplementation::loadFromDatabase(PlayerImplementation* player) {
	ResultSet* character;
	Player* playerRes = NULL;
		
	stringstream query;
	query << "SELECT * FROM characters WHERE character_id = " << player->getCharacterID();

	character = ServerDatabase::instance()->executeQuery(query);
		
	if (!character->next()) {
		stringstream msg;
		msg << "unknown character ID" << player->getCharacterID();
			
		throw Exception(msg.str());
	}

	player->setFirstName(character->getString(3));
	string surname = character->getString(4);
	
	string orbname = "Player" + player->getFirstName();
	playerRes = (Player*) ObjectRequestBroker::instance()->deploy(orbname, player);

	PlayerObjectImplementation* playerObjImpl = new PlayerObjectImplementation(playerRes);
	PlayerObject* playerObject = (PlayerObject*) playerObjImpl->deploy();
	
	player->setPlayerObject(playerObject);

	if (surname.size() != 0)
		player->setCharacterName(player->getFirstName() + " " + surname);
	else
		player->setCharacterName(player->getFirstName());
		
	String::toLower(player->getFirstName());
	
	Zone* zne = server->getZoneServer()->getZone(character->getInt(16));
	
	player->setZoneIndex(character->getInt(16));
	player->setTerrainName(Terrain::getTerrainName(player->getZoneIndex()));
	player->initializePosition(character->getFloat(13), character->getFloat(15), character->getFloat(14));
	
	Guild* guild = guildManager->getGuild(character->getUnsignedInt(12));
	if (guild != NULL)
		player->setGuild(guild);
	
	string apperance = character->getString(5);
	BinaryData cust(apperance);
	string custStr;
	cust.decode(custStr);
	player->setCharacterApperance(custStr);
	
	string hairData = character->getString(28);
	BinaryData hair(hairData);
	string hData;
	hair.decode(hData);
	player->setHairData(hData);

	int raceID = character->getInt(7);
	player->setObjectCRC(Races::getRaceCRC(raceID));
	player->setRaceName(Races::getRace(raceID));		
	player->setSpeciesName(Races::getSpecies(raceID));		
	player->setHairObject(character->getString(27));
	//player->hairData = character->getString(28);

	player->setFaction(character->getUnsignedInt(21));

	player->setHeight(character->getFloat(23));
	
	player->setCashCredits(character->getInt(10));
	player->setBankCredits(character->getInt(11));
	
	//location = character->getString(5);		
	player->setStartingProfession(character->getString(6));

	player->setItemShift(character->getUnsignedInt(34));
	
	player->setHeight(character->getFloat(23));
	
	string bio = character->getString(24); 
	player->setBiography(bio);
	
	string title = character->getString(32);
	player->getPlayerObject()->setTitle(title);
	
	player->setBaseHealth(character->getInt(35));
	player->setBaseStrength(character->getInt(36));
	player->setBaseConstitution(character->getInt(37));
	player->setBaseAction(character->getInt(38));
	player->setBaseQuickness(character->getInt(39));
	player->setBaseStamina(character->getInt(40));
	player->setBaseMind(character->getInt(41));
	player->setBaseFocus(character->getInt(42));
	player->setBaseWillpower(character->getInt(43));
	
	// On login have no buffs applied
	player->setHealthMax(character->getInt(35));
	player->setStrengthMax(character->getInt(36));
	player->setConstitutionMax(character->getInt(37));
	player->setActionMax(character->getInt(38));
	player->setQuicknessMax(character->getInt(39));
	player->setStaminaMax(character->getInt(40));
	player->setMindMax(character->getInt(41));
	player->setFocusMax(character->getInt(42));
	player->setWillpowerMax(character->getInt(43));
	
	// And stats at maximum
	player->setHealth(character->getInt(35));
	player->setStrength(character->getInt(36));
	player->setConstitution(character->getInt(37));
	player->setAction(character->getInt(38));
	player->setQuickness(character->getInt(39));
	player->setStamina(character->getInt(40));
	player->setMind(character->getInt(41));
	player->setFocus(character->getInt(42));
	player->setWillpower(character->getInt(43));
	player->setHealthMax(character->getInt(35));
	player->setStrengthMax(character->getInt(36));
	player->setConstitutionMax(character->getInt(37));
	player->setActionMax(character->getInt(38));
	player->setQuicknessMax(character->getInt(39));
	player->setStaminaMax(character->getInt(40));
	player->setMindMax(character->getInt(41));
	player->setFocusMax(character->getInt(42));
	player->setWillpowerMax(character->getInt(43));
	
	player->loadProfessions();
	
	PlanetManager* planetManager = zne->getPlanetManager();
	SceneObject* parent = planetManager->getCell(character->getUnsignedLong(33));
	
	if (parent != NULL) {
		player->setParent(parent);
		player->setBuilding((BuildingObject*) parent->getParent());
	}
	
	loadWaypoints(player);
	
	delete character;
	return playerRes;
}

void PlayerManagerImplementation::loadWaypoints(PlayerImplementation* player) {
	/*PlayerObject* play = player->getPlayerObject();
		
	if (play == NULL)
		return;
			
	try {
		stringstream query;
		query << "SELECT * FROM waypoints WHERE owner_id = '" << player->getCharacterID() <<"';";
					   
		ResultSet* res = ServerDatabase::instance()->executeStatement(query.str());

		while (res->next()) {
			uint64 waypointId = res->getUnsignedLong(0);
			string wpName = res->getString(2);
			float x = res->getFloat(3);
			float y = res->getFloat(4);
			string planetName = res->getString(5);
			bool active = res->getInt(6);
			
			Waypoint* wp = new Waypoint(waypointId, player->getCharacterID());
			wp->setWaypointID(waypointId);
			wp->setWaypointName(wpName);
			wp->setPosition(x, 0.0f, y);
			wp->setPlanetName(planetName);
			wp->setActiveStatus(active);

			play->waypointMap.put(waypointId, wp);
		}
	
		delete res;
	} catch(DatabaseException& e) {
		cout << e.getMessage() << "\n";
	}*/
}

void PlayerManagerImplementation::unload(Player* player) {
	string biography = player->getBiography().c_str();
	MySqlDatabase::escapeString(biography);
	
	stringstream query;
	query << "UPDATE characters SET x=" << player->getPositionX() << ",y=" << player->getPositionY() 
		  << ",z=" << player->getPositionZ() 
          << ",zoneid=" << player->getZoneIndex()
          << ",faction_id=" << player->getFaction() 
          << ",biography=\'" << biography << "\'" 
          << ",Title=" << "'" << player->getPlayerObject()->getCurrentTitle() << "'"
          << ",Guild=" << "'" << player->getGuildID() << "'"
           << ",credits_inv=" << "'" << player->getCashCredits() << "'"
          << ",credits_bank=" << "'" << player->getBankCredits() << "'"
          << ",parentid=" << "'" << player->getParentID() << "'"
          << ",itemShift=" << player->getItemShift()
          << " WHERE character_id=" << player->getCharacterID() << ";";   
       
               
	ServerDatabase::instance()->executeStatement(query);
	
	player->saveProfessions();	
	
	playerMap->remove(player->getFirstName());
}

void PlayerManagerImplementation::doBankTip(Player* sender, Player* receiver, int tipAmount, bool updateTipTo) {
	//Pre: sender wlocked
	if (!sender->verifyBankCredits(tipAmount)) {
		sender->sendSystemMessage("You lack the required funds to do that. (Bank Tip.)");
		return;
	}
	
	try {
		receiver->wlock(sender);
		
		receiver->addBankCredits(tipAmount);
		sender->subtractBankCredits(tipAmount);

		sender->sendSystemMessage("Your bank tip will be transferred soon.");

		//Now we send the tipper an email.

		//This will be the same in both emails, so only declare it once.
		string mailSender;
		mailSender = "System";

		string charNameSender = sender->getFirstName();

		unicode subjectSender("SENT BANK TIP");
		unicode bodySender("SENT BANK TIP");

		ChatManager* chatManager = server->getChatManager();
		chatManager->sendMail(mailSender, subjectSender, bodySender, charNameSender);

		if (updateTipTo == true) {
			//This is where we notify the other player with mail + sys message.
			//But we have to make sure they are online first. Passed in the method.
			string charNameReceiver = receiver->getFirstName();

			unicode subjectReceiver("RECEIVED BANK TIP");
			unicode bodyReceiver("RECEIVED BANK TIP");

			//Email the player you bank tipped.
			chatManager->sendMail(mailSender, subjectReceiver, bodyReceiver, charNameReceiver);
		}
		
		receiver->unlock();
	} catch (...) {
		cout << "Unreported exception caught in PlayerManagerImplementation::doBankTip\n";
		receiver->unlock();
	}
}

void PlayerManagerImplementation::doCashTip(Player* sender, Player* receiver, int tipAmount, bool updateTipTo) {
	// Pre: sender wlocked
	if (!sender->verifyCashCredits(tipAmount)) {
		sender->sendSystemMessage("You lack the required funds to do that. (Cash Tip.)");
		return;
	}
	
	try {
		receiver->wlock(sender);
		
		receiver->addCashCredits(tipAmount);
		
		sender->subtractCashCredits(tipAmount);
		sender->sendSystemMessage("You have successfully cash tipped.");

		if (updateTipTo == true) {
			//This is where we notify the other player with mail + sys message.
			//But we have to make sure they are online first. Passed in the method.
			receiver->sendSystemMessage("You have been tipped.");
		}
		
		receiver->unlock();
	} catch (...) {
		cout << "Unreported exception caught in PlayerManagerImplementation::doCashTip\n";
		receiver->unlock();
	}
}

void PlayerManagerImplementation::modifyOfflineBank(Player* sender, string receiverName, int creditAmount) {
	//First we need to get the current bank credits.
	String::toLower(receiverName);
	MySqlDatabase::escapeString(receiverName);
	
	stringstream query;
	query << "SELECT * FROM characters WHERE lower(firstname) = '" << receiverName << "'";

	ResultSet* character = ServerDatabase::instance()->executeQuery(query);

	if (!character->next()) {
		delete character;
		stringstream msg;
		msg << "unknown character name" << receiverName;
		throw Exception(msg.str());
	}

	//Grab the current credits.
	int currentBankCredits = character->getInt(11);

	//we can dump it now.
	delete character;

	//Our new credits amount.
	int newBankCredits = currentBankCredits + creditAmount;

	//Now we need to update the db.
	stringstream query2;
	query2 << "UPDATE characters SET credits_bank=" << newBankCredits 
		   << " WHERE lower(firstname)='" << receiverName << "';";   

	ServerDatabase::instance()->executeStatement(query2);

	//now we need to modify the online tippers credits.
	sender->subtractBankCredits(creditAmount);
}
