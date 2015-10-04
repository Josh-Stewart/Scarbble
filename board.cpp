//board.cpp

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

#include "board.h"


CBoard::CBoard()
{
	xmax = ymax = 15;
	firstword = FALSE;

	//create the initial tile set and put them in the tilebag list

    char tile_letters[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	int numtiles[26]      =  {9,  2,  2,  4,  12, 2,  3,  2,  9,  1,  1,  4,  2,  6,  8,  2,  1,  6,  4,  6,  4,  2,  2,  1,  2,  1};
	
	int i; //outer loop counter
	int j; //inner loop counter

	for(i=0;i<26;i++) {                     //for each letter
		for(j=0;j<numtiles[i];j++) {        //make tile j times
			tilebag.AddHead(new CTile(tile_letters[i]));
		}
	}

	//special case for blanks
	//makes 2 blank tiles
	tilebag.AddHead(new CTile());
	tilebag.AddHead(new CTile());


	//set the squares array to NULL
	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			squares[i][j] = NULL;
		}
	}
	
}

//returns NULL if there are no tiles left in bag
CObject* CBoard::GetRandomTile() {

	CObject *tmp;
	POSITION pos;
	srand(time(NULL));//used to when getting a random number between 0 and number elements in tilebag
	int limit = tilebag.GetCount();
	//limit++; //evil bug of random crashies = squashed!
	int random;

	//special cases where random breaks
	if (limit == 0) //fix
		return NULL;
	
	if (limit == 1) {
		pos = tilebag.FindIndex(0);
		tmp = tilebag.GetAt(pos);
		tilebag.RemoveAt(pos);
		return tmp;
	}
	//////////////////////////////////
	
	random = rand()%limit;
	//the general random workies case
	if( ( pos = tilebag.FindIndex( random )) != NULL ) {
		
		tmp = tilebag.GetAt(pos);
		tilebag.RemoveAt(pos);

	} 
	else {
		tmp = NULL;
	}

	
	return tmp;
}

//returns a copy of the tile at specified position (used in drawing)
CObject* CBoard::GetBoardTile(int x, int y) {

	return squares[x][y];
}

///////////////////////////
// polymorphism at work :) 
//////////////////////////

//return a tile to the tilebag (swapping)
void CBoard::AddTile(CObject *tile) {

	tilebag.AddTail(tile);

}

//add tile to a square[x][y]
void CBoard::AddTile(CObject *tile, int x, int y) {
	
	squares[x][y] = tile;

}

///////////////////////////

// checks a squares index position to see if tile is there
bool CBoard::IsEmpty(int x, int y) {
	
	//make sure player score functions dont request tiles out of board
	if (x < 0 || x >= xmax || y < 0 || y >= ymax)
		return TRUE;
	
	if (squares[x][y] == NULL) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

//returns value of tile at position
int CBoard::GetScore(int x, int y) {

	CObject *object;
	CTile *tile;

	//make sure player score functions dont request tiles out of board
	if (x < 0 || x >= xmax || y < 0 || y >= ymax)
		return 0;

	if (squares[x][y] == NULL) {
		return 0;
	}
	else {
		//read out tile->value
		object = squares[x][y];
		tile = (CTile *)object;
		return tile->GetValue();
	}
	

}

CBoard::~CBoard()
{

	//traverse squares and tilebag to delete tiles
	CObject *object;
	CTile *tile;
	POSITION pos;
	int i,j;

	//remove tiles in the tilebag
	for( pos = tilebag.GetHeadPosition(); pos != NULL; ) {	
		
		object = tilebag.GetNext(pos);
		tile = (CTile*) object;
		
		delete tile;
	}

	//remove tiles in squares[][]
	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			object = squares[i][j] = NULL;
			tile = (CTile*) object;

			delete tile;			
		}
	}

	

}

