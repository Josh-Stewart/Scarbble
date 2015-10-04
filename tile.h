// tile.h

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
	CTile is derived from CObject (its the only class that is in my game)
	this is because CTile are put into CObList's
	CBoard and CPlayer use CObLists, but they dont have to be derived from CObject to do so
*/

#ifndef TILE_HEADER
#define TILE_HEADER

#include "mfcalleg.h"
#include <afxcoll.h>

class CTile : public CObject{

public:

	CTile();

	void SetBlank(char newletter);
	int GetValue(){return value;}
	int GetIndex(){return index;}
	
	CTile(char newletter);
	
	~CTile();

private:
		
		int index; 
		/*
			this is the index that tells the renderer what tile sprite to render
			this is better than having two BITMAP pointers because tile doesnt need
			to know about BITMAP structures and the renderer decides to render 
			big or small tile sprites depending on position

			index is used alot in place of the 'letter' variable, because index determines what
			is displayed on the screen. letter is never referenced. If this was a text based app
			then letter would be used. We'll keep it here for old times sake :)
		*/

		char letter; //character on the tile 
					//this is not used (see above)
		
		
		int value;  //score value of tile (determined by tiles letter)
	
};


#endif  //TILE_HEADER