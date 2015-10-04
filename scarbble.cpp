//scarbble.cpp

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
	
// STARTING-ENTRY POINT

#define USE_CONSOLE

#include "game.h"

int main(int argc, char *argv[])
{

	CGame game;
		
	//need to pass argv for the full path where the .exe is run to open data files successfully
	if (game.SetupGame(argv)) {
		return 1; //something went wrong in setup.. time to die
				  //SetupGame should have already displayed some error message
	}
	
	do {

		game.PlayTurn();
	
	} while (!game.GameEnd());
	
	game.ShowResults();

	//printf("\n\n\n\t[normal exit]\n\nVersion Dump: %s\n\n",allegro_id); //debug
	
	return 0;
}


END_OF_MAIN();