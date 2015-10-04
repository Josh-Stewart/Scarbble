//game.h

/*
	<begin copyright stuff>

	Copyright 2001 Joshua Stewart

	This file is part of Scarbble.

    Scarbble is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Scarbble is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Scarbble; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	<end copyright stuff>
*/

/*
	The game class is like the overseeer of the game. 
	It contains the board and players.
	It has all the drawing functions (that are called after each mouse click is processed)
	It catches mouse clicks and sends them to player to be processed
	It sets the next player
	It doesnt keep score (thats the players job)
*/

#ifndef GAME_HEADER
#define GAME_HEADER

#include "player.h"
#include "board.h"

class CGame{

public:
	
	CGame();
	
	/*	SetupGame() initialises allegro graphics libraries 
		plays starting animation
		loads tiles into memory
		returns 1 if error occurs somewhere
	*/
	int SetupGame(char *argv[]);

	//controls input from the user and rendering during a players turn
	//removes users that are finished
	void PlayTurn();

	//after each players turn this is called to determine if the game has finished
	bool GameEnd();
	
	//shows final results screen
	void ShowResults();

	~CGame();

private:

	CBoard board;		//the board
	CPlayer players[4]; //the players
	
	int numplayers; //number of players (1-4)
	int currplayer; //current players number (NOTE: possible value range 0-3)
	int boardcolor; //board color (pretty useless since user cant set it)
	
	///////////////////
	//private methods
	///////////////////
	
	//prompts for number of players and asks for player names
	void AddPlayers();

	void InitialisePlayers();

	////////////////////////////////////////
	//----------  GUI STUFF  -------------//
	////////////////////////////////////////
	
	// GUI variables
	// bitmaps
	WINDOWS_BITMAP *background; //we draw to this before blitting to screen
	WINDOWS_BITMAP *smalltiles[27]; //scaled down tiles (0.8 scale)

	//data file pointer (see data.h)
	DATAFILE *data; //all our tiles, buttons, animation frames etc are in here
	
	//////////////////////
	//GUI private methods
	//////////////////////

	//rendering functions
	void DrawCursor();
	void DrawBoard(); 
	void DrawScores();
	void DrawPlayer();
	void PlayIntro();

};


#endif  //GAME_HEADER