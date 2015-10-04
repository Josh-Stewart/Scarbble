//tile.cpp

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

#include "tile.h"

CTile::CTile()
{
    value = 0; //blank tile	
	index = 0; //sprite array index (for GUI)
}

CTile::CTile(char newletter)
{

	switch(newletter) {
	case 'a' : value = 1;  index = 1;  break;
	case 'b' : value = 3;  index = 2;  break;
	case 'c' : value = 3;  index = 3;  break;
	case 'd' : value = 2;  index = 4;  break;
	case 'e' : value = 1;  index = 5;  break;
	case 'f' : value = 4;  index = 6;  break;
	case 'g' : value = 2;  index = 7;  break;
	case 'h' : value = 4;  index = 8;  break;
	case 'i' : value = 1;  index = 9;  break;
	case 'j' : value = 8;  index = 10; break;
	case 'k' : value = 5;  index = 11; break;
	case 'l' : value = 1;  index = 12; break;
	case 'm' : value = 3;  index = 13; break;
	case 'n' : value = 1;  index = 14; break;
	case 'o' : value = 1;  index = 15; break;
	case 'p' : value = 3;  index = 16; break;
	case 'q' : value = 10; index = 17; break;
	case 'r' : value = 1;  index = 18; break;
	case 's' : value = 1;  index = 19; break;
	case 't' : value = 1;  index = 20; break;
	case 'u' : value = 1;  index = 21; break;
	case 'v' : value = 4;  index = 22; break;
	case 'w' : value = 4;  index = 23; break;
	case 'x' : value = 8;  index = 24; break;
	case 'y' : value = 4;  index = 25; break;
	case 'z' : value = 10; index = 26; break;
	}
	
	//letter = newletter;

}

void CTile::SetBlank(char newletter) {
	
	//same as the constructor except that the value stays the same
	
	switch(newletter) {
	case 'a' : index = 1;  break;
	case 'b' : index = 2;  break;
	case 'c' : index = 3;  break;
	case 'd' : index = 4;  break;
	case 'e' : index = 5;  break;
	case 'f' : index = 6;  break;
	case 'g' : index = 7;  break;
	case 'h' : index = 8;  break;
	case 'i' : index = 9;  break;
	case 'j' : index = 10; break;
	case 'k' : index = 11; break;
	case 'l' : index = 12; break;
	case 'm' : index = 13; break;
	case 'n' : index = 14; break;
	case 'o' : index = 15; break;
	case 'p' : index = 16; break;
	case 'q' : index = 17; break;
	case 'r' : index = 18; break;
	case 's' : index = 19; break;
	case 't' : index = 20; break;
	case 'u' : index = 21; break;
	case 'v' : index = 22; break;
	case 'w' : index = 23; break;
	case 'x' : index = 24; break;
	case 'y' : index = 25; break;
	case 'z' : index = 26; break;
	}

	//letter = newletter;

}

CTile::~CTile()
{

}


