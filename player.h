// player.h

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

#ifndef PLAYER_HEADER
#define PLAYER_HEADER

#include "tile.h"
#include "board.h"
#include <afxcoll.h>


const int MAX_NAME_SIZE = 21;


class CPlayer {
	
public:

	char name[MAX_NAME_SIZE];
	int color;
	int score;
	bool stillplaying;
	
	/* TILE CONTAINERS */
	/*
		these are are all included when rendering frames
		these are all pointers to tiles because the tile instance doesnt move in memory, 
		but pointers to it are moved around all over the place (moving a pointer is quicker
		than moving a slab of memory)
		Why are they not private? well CGame has all the drawing functions in it, and they need to
		access these lists of tile pointers alot. I could move all the drawing out into CPlayer and
		CBoard.. but it wouldnt be pretty :)
	*/
	CObList rack;			// the main rack of tiles.
	CObList tmprack;		// this is the rack below the main rack in the players view
	
	CObject *playerboard[15][15];	
	// stores tiles draged onto board during a players turn
	// they are moved to the real board at the end of the turn.
	// its to keep track of which tiles on the board still belong to a player during their turn.
	// this isnt a linked list because to render it would require the tiles to know their x,y positions 
	
	CObject *cursor;	// holds tile currently attached to the cursor, that isnt on rack or board
	

	CPlayer();
	
	void SetPBoard(CBoard *boardpointer); //used to give each player a pointer to the board
	void AddTile(CObject *tile); //add tiles to the player (usually from the tilebag)
	int CalcFinalScore();

	//access to the players turncomplete flag
	bool Done(){return turncomplete;}
	void ResetTurnComplete();

	//GUI MEMBER FUNCTIONS
	void HandleMouse(int x, int y); //after a click, the xy are passed and it determines what to do.

	~CPlayer();


private:

	bool turncomplete; //updated when a player finishes his turn
	
	CBoard* pboard; 
	//holds a pointer to the board so that CPlayer functions can call public member functions
	//on board. This way play functions can add tiles to the board and get tiles from the tilebag
	//by themselves. If this wasnt here all the code to manage distribution of tiles to board 
	//and players would have to be done in CGame. I think CGame has enough to worry about already :)

	void ProcessPlayerBoard(); //at the end of the turn process any tiles the player moved to the board
	void ProcessBlank(CObject *tile, int i, int j); //prompts the user for the letter to set the blank to
	
	void Reset(); //puts all the players tiles back on his first rack
	void Swap(); //grabs tiles from rack 2 and swaps them with tiles from the tilebag

	//mouse bounding box checking + position reporting
	//called when processing a mouse click to find out what the user is doing
	void CalcSquarePos(int &x, int &y);//what square is the user over?
	void CalcRackPos(int &x); //what rack position is the user over?
	bool InsideBoard(int x, int y);//these all return true if the mouse was inside that object 
	bool InsideRack1(int x, int y);
	bool InsideRack2(int x, int y);
	bool InsideReset(int x, int y);
	bool InsideSwap(int x, int y);
	bool InsideDone(int x, int y);
	bool InsideExit(int x, int y);

};

	
#endif