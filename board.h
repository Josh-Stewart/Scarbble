//board.h

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

#ifndef BOARD_HEADER
#define BOARD_HEADER

#include "tile.h"
#include <afxcoll.h>


class CBoard {
public:	
		
		bool firstword; // flag to give first player double score 
	
		CBoard();

		CObject* GetRandomTile(); //from tilebag
		CObject* GetBoardTile(int x, int y); //returns copy of a tile at position (for drawing)

		void AddTile(CObject *tile); // add tile to tilebag (swaps)
		void AddTile(CObject *tile, int x, int y); // add tile to squares
		
		bool IsEmpty(int x, int y); // check if squares[x][y] exists
		int GetScore(int x, int y); //checks squares position for tile and returns score or 0 if no tile
		

		~CBoard ();

private:
		 
		int xmax;
		int ymax; 
		
		/* 
			Why 2D array instead of CObList, you might ask. Here are some reasons
		   
			 tiles dont have to store x,y
			 you dont have to traverse entire list to check if a given x,y is empty
			 it is a more logical representation of real life board data
		*/	   

		CObject *squares[15][15];
		CObList tilebag; 
		
};


#endif // BOARD_HEADER