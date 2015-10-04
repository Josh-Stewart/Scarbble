//player.cpp

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

#include "player.h"
#include "board.h"


CPlayer::CPlayer() {
	
	int i,j;

	score = 0;
	stillplaying = TRUE;
	turncomplete = FALSE;

	//set the playerboard array to NULL
	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			playerboard[i][j] = NULL;
		}
	}

	cursor = NULL;

}

void CPlayer::SetPBoard(CBoard *boardpointer) {

	pboard = boardpointer;
}


void CPlayer::AddTile(CObject *tile) {
	
	rack.AddTail(tile);
}

// collect all the tiles and put them in rack1
void CPlayer::Reset() {

	//reset rack2 if not empty
	if (!tmprack.IsEmpty()) {
		CObList* listpointer = &tmprack;
		rack.AddTail(listpointer);
		tmprack.RemoveAll(); //clean out tmprack pointers
	}
	

	//reset playerboard
	int i,j; //loop counters	
	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			if (playerboard[i][j] != NULL) {
				
				rack.AddTail(playerboard[i][j]);
				
				playerboard[i][j] = NULL;
			}
		}
	}	
	
	//reset cursor
	if (cursor != NULL) {
		rack.AddTail(cursor);
		cursor = NULL;
	}
	
}

void CPlayer::Swap() {

	//take all tiles from rack2 put them in tilebag.. and get the same amount of tiles back
	
	int count;
	POSITION pos;
	CObject *tmp;
	
	count = tmprack.GetCount();
	
	for( pos = tmprack.GetHeadPosition(); pos != NULL; ) {		
	
		pboard->AddTile(tmprack.GetNext(pos));
	}

	tmprack.RemoveAll();

	for (int i=0;i<count;i++) {
		
		tmp = pboard->GetRandomTile();
		if  (tmp != NULL) //check return value
			tmprack.AddTail(tmp);
	}

}

void CPlayer::ResetTurnComplete(){

	turncomplete = FALSE;

}

void CPlayer::ProcessPlayerBoard() {
	
	/* 
		This is where we take tiles from the playerboard[][] and stick them onto the real board
		As we do this we process blank tiles and calculate the score for the turn.

		_Scoring:
		We move all tiles going to the board into an array along with relevent position data
		Then for each element of that array we look for orthoginally connected tiles on the real board
		If we find a tile then we add it to another list of tiles (no dupes)

		Soo at the end we have a list of all tiles going to the board _AND_ a list of all tiles
		orthoginally connected to the tiles going to the board. 
		
		We just add up the scores of all those tiles and we have the score for that turn. *phew* 		
	*/

	//this is just a little struct to hold a tile and its position information (see comment above)
	struct TileIndex {
		int x,y;
		int score; //used by stack B
		CObject *tile; //used by stack A
	};
	

	int i,j; //loop counters
	int x,y; //position indexes
	bool endthread; //flag to say "there are no more orthogonally connected tiles"
	CObject *tmp; //for adding new tiles at end
	
	int bcount=0; //how many elements in btiles[]
	TileIndex btiles[100]; //tiles from board.squares that are to be added to the score
	
	int count=0; //how many elements in pending[]
	TileIndex pending[7]; //7 possible tiles to add
	
	//traverse playerboard and fill pending[]
	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			if (playerboard[i][j] != NULL) {

				//check for blanks and processes em
				ProcessBlank(playerboard[i][j],i,j);
				
				//add to pending[]
				pending[count].tile = playerboard[i][j]; //save tile to pending
				pending[count].x = i; //save x pos to pending
				pending[count].y = j; //save y pos to pending
				playerboard[i][j] = NULL; //clear board at pos
				count++;

			}
		}
	}

	//start the scoring.. filling btiles[] with tiles that are already on the board
	//and should be included in the players score, based on whats in pending[]

	for (i=0;i<count;i++) {  //for each element in pending[]
		
		x = pending[i].x;
		y = pending[i].y;
		endthread = FALSE;

		while (!pboard->IsEmpty(x+1,y)) {
			
			//traverse B
			for (j=0;j<bcount;j++) {
				if (btiles[j].x == x+1 && btiles[j].y == y) { //if offset is in B already
					endthread = TRUE;
				}
			}
			
			if (!endthread) {
				//add tile to B
				btiles[bcount].x = x+1;
				btiles[bcount].y = y;
				btiles[bcount].score = pboard->GetScore(x+1,y); //check return maybe
				bcount++;
			}

			x++;
		}

		x = pending[i].x;
		endthread = FALSE;
		
		while (!pboard->IsEmpty(x-1,y)) {
		
			//traverse B
			for (j=0;j<bcount;j++) {
				if (btiles[j].x == x-1 && btiles[j].y == y) { //if offset is in B already
					endthread = TRUE;
				}
			}
			
			if (!endthread) {
				//add tile to B
				btiles[bcount].x = x-1;
				btiles[bcount].y = y;
				btiles[bcount].score = pboard->GetScore(x-1,y); //check return maybe
				bcount++;
			}

			x--;
		}
		
		x = pending[i].x;
		endthread = FALSE;

		while (!pboard->IsEmpty(x,y+1)) {
		
			//traverse B
			for (j=0;j<bcount;j++) {
				if (btiles[j].x == x && btiles[j].y+1 == y) { //if offset is in B already
					endthread = TRUE;
				}
			}
			
			if (!endthread) {
				//add tile to B
				btiles[bcount].x = x;
				btiles[bcount].y = y+1;
				btiles[bcount].score = pboard->GetScore(x,y+1); //check return maybe
				bcount++;
			}

			y++;
		}

		y = pending[i].y;
		endthread = FALSE;

		while (!pboard->IsEmpty(x,y-1)) {
		
			//traverse B
			for (j=0;j<bcount;j++) {
				if (btiles[j].x == x && btiles[j].y-1 == y) { //if offset is in B already
					endthread = TRUE;
				}
			}
			
			if (!endthread) {
				//add tile to B
				btiles[bcount].x = x;
				btiles[bcount].y = y-1;
				btiles[bcount].score = pboard->GetScore(x,y-1); //check return maybe
				bcount++;
			}

			y--;
		}
	}

	/* OK.. *phew*  at this point we have an array of pending tiles ready to be moved to the board
	   and an array of position and score data.. 

	   we need to add up scores from pending[] and btiles[].. then move pending[] tiles onto the 
	   real board and give the player some new tiles.
	*/

	//traverse pending[]
	for (i=0;i<count;i++) {
	
		//move to board
		pboard->AddTile(pending[i].tile,pending[i].x,pending[i].y);
		
		//add to score
		//pending didnt store scores.. its easier to get them after i add them to the board
		score += pboard->GetScore(pending[i].x,pending[i].y);

		//get new tiles
		tmp = pboard->GetRandomTile();
		if  (tmp != NULL) { //check return value
			rack.AddTail(tmp);
		}
	}
	
	//traverse btiles[]
	for (i=0;i<bcount;i++) {
		//unlike pending, we store the scores for btiles
		score += btiles[i].score;
	}

	// my god what a mission.. this scoring system is the biggest mindbreaker. 
	// i just hope it covers all the scoring possibilities 
	// (UPDATE: i did find one really odd instance where it doesnt add one tiles score..
	// ..i decided against spending a day debugging it)
	
	//double score for first word
	if (!pboard->firstword) {  //first word hasnt been placed yet
		pboard->firstword = TRUE;
		score = score * 2;
	}

	// NOTE: i asume you get the 50 bonus after the double score for first word using 7 tiles

	//50 point bonus for all 7 tiles used in a turn
	if (count == 7) {
		score = score + 50;
	}

}

void CPlayer::ProcessBlank(CObject *tile, int i, int j) {
	
	CTile *tmp;
	int store;
	char c;
	
	tmp = (CTile *)tile;
	
	if (tmp->GetIndex() == 0) { //if blank
		//prompt for blank letter at [i,j]
		do {
			textprintf(screen,font,5,580,color,"Press the letter you want the blank at [%d,%d] to be (then ENTER):",i,j);
			rectfill(screen, (i*38)+1, (j*38)+1, (i*38)+37, (j*38)+37, color);
			
			store = readkey();
		
			if ((store>>8) != KEY_ENTER) {
				//set char
				c = (store & 0xff); //convert store to ascii char 
				//display key
				textout(screen,font," ",530,580,color); //dodgy erase last char printed
				textprintf(screen,font,530,580,color,"%c",c);
			}

		} while ((store>>8) != KEY_ENTER);
		
		tmp->SetBlank(c);

	}	
}

int CPlayer::CalcFinalScore() {

	//take returns the score, minus the tiles left on the rack
	int finalscore;
	POSITION pos;
	CObject *object;
	CTile *tile;
	
	finalscore = score;

	for( pos = rack.GetHeadPosition(); pos != NULL; ) {		

		object = rack.GetNext(pos);
		tile = (CTile*) object;
		finalscore = finalscore - tile->GetValue(); 
	}

	return finalscore;
}

//gets an X,Y from the mouse and figures out what users wants to do
void CPlayer::HandleMouse(int x, int y) {

	POSITION pos;
	
	//if something is attached to the cursor
	if (cursor == NULL) {
		
		if (InsideBoard(x,y)) {

			CalcSquarePos(x,y);
			if (playerboard[x][y] != NULL) {
				cursor = playerboard[x][y];
				playerboard[x][y] = NULL;
			}
		}
		if (InsideRack1(x,y)) {
			CalcRackPos(x);
			if( ( pos = rack.FindIndex( x )) != NULL ) {
				
				cursor = rack.GetAt( pos );
				rack.RemoveAt(pos);
			}
		}

		if (InsideRack2(x,y)) {
			CalcRackPos(x);
			if( ( pos = tmprack.FindIndex( x )) != NULL ) {
				
				cursor = tmprack.GetAt( pos );
				tmprack.RemoveAt(pos);
			}
		}

		if (InsideReset(x,y)) {
			Reset();
		}

		if (InsideSwap(x,y)) {
			Swap();
			Reset();
			turncomplete = TRUE;
		}

		if (InsideDone(x,y)) {
			//player turn finished
			ProcessPlayerBoard();
			Reset();
			turncomplete = TRUE;
		}

		if (InsideExit(x,y)) {
			stillplaying = FALSE;
			Reset();
			turncomplete = TRUE;
		}

	}

	else { //if there is a tile on the cursor
		
		if (InsideBoard(x,y)) {
			CalcSquarePos(x,y);
			if (playerboard[x][y] == NULL) {
				if (pboard->IsEmpty(x,y)) {
					playerboard[x][y] = cursor;
					cursor = NULL;
				}
			}

		}
		if (InsideRack1(x,y)) {
			CalcRackPos(x);
			if (rack.GetCount() < 7) { //less than 7 tiles in rack
				rack.AddTail(cursor);
				cursor = NULL;
			}
		}

		if (InsideRack2(x,y)) {
			CalcRackPos(x);
			if (tmprack.GetCount() < 7) { //less than 7 tiles in tmprack
				tmprack.AddTail(cursor);
				cursor = NULL;
			}
		}

		if (InsideReset(x,y)) {
			Reset();
		}

		if (InsideSwap(x,y)) {
			Swap();
			Reset();
			turncomplete = TRUE;
		}

		if (InsideDone(x,y)) {
			//player turn finished
			ProcessPlayerBoard();
			Reset();
			turncomplete = TRUE;
		}

		if (InsideExit(x,y)) {
			stillplaying = FALSE;
			Reset();
			turncomplete = TRUE;
		}

	}//end else

}

/* Bounding Box Checking Functions */

bool CPlayer::InsideBoard(int x, int y) {

	if (x < 570 && y < 570)
		return TRUE;

	return FALSE;
}

bool CPlayer::InsideRack1(int x, int y) {

	if (x >= 580 && x <= 790 && y >= 150 && y <= 180)
		return TRUE;

	return FALSE;
}

bool CPlayer::InsideRack2(int x, int y) {

	if (x >= 580 && x <= 790 && y >= 210 && y <= 240)
		return TRUE;

	return FALSE;
}

bool CPlayer::InsideReset(int x, int y) {

	if (x >= 580 && x <= 680 && y >= 260 && y <= 310)
		return TRUE;

	return FALSE;
}

bool CPlayer::InsideSwap(int x, int y) {

	if (x >= 690 && x <= 790 && y >= 260 && y <= 310)
		return TRUE;

	return FALSE;
}

bool CPlayer::InsideDone(int x, int y) {

	if (x >= 635 && x <= 735 && y >= 530 && y <= 580)
		return TRUE;

	return FALSE;
}

bool CPlayer::InsideExit(int x, int y) {

	if (x >= 779 && x <= 799 && y >= 1 && y <= 21)
		return TRUE;

	return FALSE;
}

void CPlayer::CalcSquarePos(int &x, int &y) {

	/* this little function gets the mouse position and determines what square on the board it is over
	   why didnt i pay more attention in high school maths class!! :(

	   why did i do it this way instead of having 16 if statements, or select? Because i was tired and this 
	   was the first method that occured to me. Also i like punishment. This way is slighly more portable in that
	   wether its 7 or 300 squares on the board, it still takes the same amount of time to calclate.

	   to find the board position of x, you divide it by 38 (square length) and add a small offset that is determined by x 
	*/

	float tmp;
	float slope = (1 / -570); 
	float offset = (slope * x) + 1;

	tmp = (x / 38) + offset;
	x = tmp;
	x--;  //array starts at 0

	//re calculate offset for the y axis
	offset = (slope * y) + 1;

	tmp = (y / 38) + offset;
	y = tmp;
	y--; //array starts at 0
}

void CPlayer::CalcRackPos(int &x) {

	// same thing as the square position thing.. exept this one caters to both the racks and only needs the x value
	// since only time this is called is after the InsideRack bounding function returns true.
	
	x = x - 580;

	float tmp;
	float slope = (1 / -570); 
	float offset = (slope * x) + 1;

	tmp = (x / 30) + offset;
	x = tmp;

	x--; //coblist index starts at 0

}


CPlayer::~CPlayer()
{

	int i,j;
	CObject *object;
	CTile *tile;
	POSITION pos;

	for( pos = rack.GetHeadPosition(); pos != NULL; ) {		
	
		object = rack.GetNext(pos);
		tile = (CTile*) object;
		delete tile;
	}

	//these should be empty anyway.. but we'll check
	if (cursor != NULL)
		delete cursor;

	for( pos = tmprack.GetHeadPosition(); pos != NULL; ) {		
	
		object = tmprack.GetNext(pos);
		tile = (CTile*) object;
		delete tile;
	}

	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			if (playerboard[i][j] != NULL)
				delete playerboard[i][j];
		}
	}


}


