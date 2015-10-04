//game.cpp

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

#include "game.h"
#include "data.h"
#include "tile.h"
#include "mfcalleg.h"

#include <afxcoll.h>
#include <stdio.h>

CGame::CGame()
{
    	
}

int CGame::SetupGame(char *argv[]) {

	int bpp = 32; //bits per pixel (truecolor)
	
	//buffers for filename manipulation
	char fullpath[255];
	char filename[10]; 

	int ret; //catches return values for processing
	int i;  //universal loop counter

	//initialise allegro stuff
	allegro_init(); 
	install_keyboard(); 
	install_mouse(); 
	install_timer();

	set_color_depth(bpp);
	
	/* Lets play the color depth game!  tries 32, 24, then 16 bit color modes */
	// set resolution to play intro movie
	ret = set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
	/* did the video mode set properly? */
	if (ret != 0) {
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Error setting %d bit graphics mode\n%s\nLets try another color depth!", bpp, allegro_error);
		
		bpp = 24;
		set_color_depth(bpp);
		
		// set resolution
		ret = set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
		/* did the video mode set properly? */
		if (ret != 0) {
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Error setting %d bit graphics mode\n%s\nLets try another color depth!", bpp, allegro_error);

			bpp = 16;
			set_color_depth(bpp);

			// set resolution
			ret = set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
			/* did the video mode set properly? */
			if (ret != 0) {
				set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
				allegro_message("Error setting %d bit graphics mode\n%s\nIm all out of options. Exiting", bpp, allegro_error);
				return 1;
			}
		}
	}
	
	// Load data from data.dat into memory
	textout_centre(screen,font,"NOW LOADING",SCREEN_W/2,SCREEN_H/2,makecol(255,255,255));

	sprintf(filename,"data.dat");
	replace_filename(fullpath, argv[0], filename, sizeof(fullpath));
    data = load_datafile(fullpath);
	if (!data) { 
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Cant load file %s for some reason.\nIt should be in the same directory as the .exe\n"
		"If you have deleted the debug directory to mark this then\ncopy all files from the \"required files\""
		"directory into the directory with the .exe\n\nThanks. Scarbble will now crash horribly", filename);
		return 1;
	}

	//make smaller copy of all tiles
	for (i=0;i<27;i++) {
		smalltiles[i] = create_bitmap(29,29);
		rotate_scaled_sprite(smalltiles[i], (WINDOWS_BITMAP *)data[i].dat, 0,0, 0, ftofix(0.8));
	}
		
	PlayIntro();

	AddPlayers();

	//stuff that cant be done in the players constructor
	InitialisePlayers();
	
	//set video mode to play game
	ret = set_gfx_mode(GFX_AUTODETECT, 800, 600, 0, 0);
	/* did the video mode set properly? */
	if (ret != 0) {
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Error setting %d bit graphics mode\n%s\n", bpp, allegro_error);
		return 1;
	}
	
	//initialise the 'background' screen buffer
	background = create_bitmap(SCREEN_W, SCREEN_H);
	clear(background); //clear to black
	boardcolor = makecol(206,206,90);

	show_mouse(screen);
		
	return 0;
}

void CGame::PlayTurn() {

	int x,y;

	scare_mouse();
	DrawBoard();
	DrawPlayer();
	DrawScores();
	DrawCursor();

	blit(background, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	unscare_mouse();

	while (!players[currplayer].Done()) {

		if (mouse_b & 1) {

			x = mouse_x;
			y = mouse_y;
			players[currplayer].HandleMouse(x,y);
			rest (200); //rest for a bit so it doesnt keep catching mouse clicks
			
			scare_mouse();
			DrawBoard();
			DrawScores(); //shouldnt have to redraw this, but i dont have time to do a decent redraw management system
			if (!players[currplayer].Done())
				DrawPlayer(); //only redraw if not at end of turn
			DrawCursor();
	
			blit(background, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
			unscare_mouse();
		}

		
	}//end while

	players[currplayer].ResetTurnComplete();
		

	//update currplayer

	int oldcurrplayer;
	oldcurrplayer = currplayer;

	do {
		if (currplayer == (numplayers-1)) {  //if currplayer is last in line
			currplayer = 0;
		}
		else {
			currplayer++;
		}

	//stop increment currplayer if the curr isnt playing AND we have gone through the whole list already
	} while (!players[currplayer].stillplaying && currplayer != oldcurrplayer);


}

bool CGame::GameEnd() {

	//returns true if all players are finished

	int i;

	for (i=0;i<numplayers;i++) {
		if (players[i].stillplaying)
			return FALSE;
	}

	return TRUE;
}


CGame::~CGame()
{
	int i;

	//destroy bitmaps
	destroy_bitmap(background);

	for (i=0;i<27;i++) {
		destroy_bitmap(smalltiles[i]);
	}
	
	//unload data
	unload_datafile(data);

}

/*
	PRIVATE MEMBER FUNCTIONS
*/

void CGame::AddPlayers(){

	int i; //loop counter
	srand(time(NULL)); // seed random for colors
	int limit = 255;   //used to when getting a random number between 0 and 255-1

	set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

	//display copyright crap
	printf("Scarbble v1, Copyright (C) 2001 Joshua Stewart\n"
		   "Scarbble comes with ABSOLUTELY NO WARRANTY;\n"
		   "This is free software, and you are welcome to\n"
		   "redistribute it under certain conditions.\n\n");


	printf("\n\nPlease enter the number of players [2-4]: ");
	fflush(stdin);
	scanf("%d",&numplayers);

	//check valid number of players
	while (numplayers < 2 || numplayers > 4) {
	
		printf("\n\tScarbble requires between 2 to 4 players.."); 
		printf("\n\nPlease enter the number of players [2-4]: ");
		fflush(stdin); //dodgy as hell.. you shouldnt fflush input streams..my bad :( 
		scanf("%d",&numplayers);
	}

	for (i=0; i<numplayers;i++) {
		printf("\n\n\tPlease enter player %d's name: ",i+1);
		fflush(stdin);
		scanf("%21[^\n]",players[i].name);
		
		// generate a random color for the player
		// we cant do this in the player constructor because allegro isnt initialised at that point
		players[i].color = makecol(rand()%limit,rand()%limit,rand()%limit); 
	}

}

void CGame::InitialisePlayers() {
	
	//give each player 7 tiles + give them board pointers
	//also choose which player to go first
	//this is stuff we cant do in the players constructor

	int i,j; //loop counters
	int min = 27;
	CBoard *boardpointer = &board;
	CObject *tmp;
	CTile *tile;

	for (i=0;i<numplayers;i++) {
		//pass each player a board pointer
		players[i].SetPBoard(boardpointer); 
		
		//every players first drawn tile is compared with other players first drawn. closest to 'A' goes first
		tmp = board.GetRandomTile();
		tile = (CTile *)tmp;
		if (tile->GetIndex() < min && tile->GetIndex() != 0)
				currplayer = i;
				min = tile->GetIndex();
		players[i].AddTile(tmp);

		//give em 6 more tiles
		for (j=0;j<6;j++) {
			tmp = board.GetRandomTile();
			players[i].AddTile(tmp);
		}
	}	

}

/* 
  GUI PRIVATE MEMBER FUNCTIONS

  below are the gui member functions.. they draw stuff to the screen

*/


//fill *background with static elements
//so we dotn have to redraw them all the time :)
void CGame::DrawBoard(){
	
	int i,j;

	clear(background);

	//main board
	rectfill(background, 1, 1, 569, 569, boardcolor);
	
	//lines on board
	for (i=1;i<15;i++) {
		
		vline(background,(i*38),1 ,570, 0);
	}
	
	for (i=1;i<15;i++) {
		
		hline(background,1, (i*38),570, 0);
	}

	//center marker
	draw_sprite(background, (WINDOWS_BITMAP *)data[ZSTAR_BMP].dat, 267,267);

	//draw board tiles
	int xpos = 2;
	int ypos = 2;
	
	CObject *object; //object pointer
	CTile *tile; //tile pointer

	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			if (!board.IsEmpty(i,j)) {

				object = board.GetBoardTile(i,j);
				tile = (CTile*) object; 
				draw_sprite(background, (WINDOWS_BITMAP *)data[tile->GetIndex()].dat,xpos,ypos);
			}
			
			ypos += 38; //updated ypos
		}

		ypos = 2; //reset ypos
		xpos += 38; //update xpos
	}

	//draw playerboard tiles
	xpos = 2;
	ypos = 2; //reset these
	
	for (i=0;i<15;i++) {
		for (j=0;j<15;j++) {
			if (players[currplayer].playerboard[i][j] != NULL) {

				object = players[currplayer].playerboard[i][j];
				tile = (CTile*) object; 
				draw_sprite(background, (WINDOWS_BITMAP *)data[tile->GetIndex()].dat,xpos,ypos);
			}
			
			ypos += 38; //updated ypos
		}

		ypos = 2; //reset ypos
		xpos += 38; //update xpos
	}


}

void CGame::DrawScores(){

	//read + display scores from each player

	int i;
	for (i=0;i<numplayers;i++) {
		textprintf(background,font, 580, ((i+1)*20), players[i].color, "%02d - %s",players[i].score,players[i].name);
	}
	//display player names and scores.. color coded
	rect(background,570,0,SCREEN_W-1,100,players[currplayer].color);

	draw_sprite(background, (WINDOWS_BITMAP *)data[ZEXIT_BMP].dat, SCREEN_W-21,1);

	//draw lines through players name that are out of the game
	for (i=0;i<numplayers;i++) {
		if (!players[i].stillplaying) {
			hline(background,580, ((i+1)*20)+4, 790, players[i].color);
		}
	}
}


//changes the mouse cursor to the tile when you click on a tile
//also changes the actual mouse xy position to the center of the tile
void CGame::DrawCursor() {
	
	CObject *object;
	CTile *tile;

	if (players[currplayer].cursor == NULL) {
		set_mouse_sprite(NULL);
		set_mouse_sprite_focus(0,0);
	}
	else{
		object = players[currplayer].cursor;
		tile = (CTile*) object;
		set_mouse_sprite((WINDOWS_BITMAP *)data[tile->GetIndex()].dat);
		set_mouse_sprite_focus(18,18);
	}
}


void CGame::DrawPlayer() {
	
	// draw players name and tile rack and temp tile rack
	// and buttons to do stuff
	// all color coded
	
	int i; //used to calculate rack position offset when drawing sprite
	CObject *object; //object pointer
	POSITION pos; //CObList position
	CTile *tile; //tile pointer

	//boarder around board
	rect(background,0,0,570,570,players[currplayer].color);
	
	//player hud
	rect(background,570,101,SCREEN_W-1,SCREEN_H-1,players[currplayer].color);
	
	//player name ("rub out" old one. cheap and nasty style)
	textout_centre(background,font,"Current player is:",685,105,players[currplayer].color);
	textout(background,font,".                          .",571,125,players[currplayer].color);
	textprintf_centre(background,font,685,125,players[currplayer].color,"%s",players[currplayer].name);
	
	//rack 1
	rectfill(background,580,150,790,180,players[currplayer].color);
	//rack2
	rectfill(background,580,210,790,240,players[currplayer].color);

	// render rack1 tiles
	i = 1;
	if (!players[currplayer].rack.IsEmpty()) {
		for( pos = players[currplayer].rack.GetHeadPosition(); pos != NULL; i++) {	

			object = players[currplayer].rack.GetNext(pos);
			tile = (CTile *)object;

			draw_sprite(background, smalltiles[tile->GetIndex()], (551+(i*30)),151);
		}
	}

	//render rack2 tiles
	i = 1;
	if (!players[currplayer].tmprack.IsEmpty()) {
		for( pos = players[currplayer].tmprack.GetHeadPosition(); pos != NULL; i++) {	
		
			object = players[currplayer].tmprack.GetNext(pos);
			tile = (CTile *)object;
		
			draw_sprite(background, smalltiles[tile->GetIndex()], (551+(i*30)),211);
		}

	}

	//draw buttons + spam banner :)
	draw_sprite(background, (WINDOWS_BITMAP *)data[ZRESET_BMP].dat, 580,260);
	draw_sprite(background, (WINDOWS_BITMAP *)data[ZSWAP_BMP].dat, 690,260);
	draw_sprite(background, (WINDOWS_BITMAP *)data[ZSPAM_BMP].dat, 571,310);	
	draw_sprite(background, (WINDOWS_BITMAP *)data[ZDONE_BMP].dat, 635,530);

}


//play intro movie etc
void CGame::PlayIntro() {   
	
	//play FLC from memory
	play_memory_fli(data[ZANIMATION].dat,screen, 0, NULL);

	// display flashing prompt

	text_mode(-1); //text with transparent background

	//crude by effective blinking text :)
	while (!keypressed()) {
		
		textout_centre(screen,font,"INSERT COIN(S)..",320,400,makecol(0,0,0));
		rest(600);
		textout_centre(screen,font,"INSERT COIN(S)..",320,400,makecol(206,206,90)); 
		rest(300);
	}

	clear_keybuf();
	
}

//the final score screen
void CGame::ShowResults() {

	int i;
	int finalscore;
	WINDOWS_BITMAP *text;

	text = create_bitmap(220, 130);
	clear_to_color(text,makecol(255,0,255));
	

	scare_mouse();

	//background image
	blit((WINDOWS_BITMAP *)data[ZSCORE_BMP].dat, background,0,0,0,0,SCREEN_W,SCREEN_H);
	
	//print text on temp image and resize
	text_mode(-1);

	for (i=0;i<numplayers;i++) {
		
		finalscore = players[i].CalcFinalScore();
		textprintf_centre(text,font,110,(i*40),players[i].color,"%d - %s",finalscore,players[i].name);
	}
	
	masked_stretch_blit(text,background,0,0,220,130,210,190,390,250);

	blit(background,screen,0,0,0,0,SCREEN_W,SCREEN_H);

	clear_keybuf();
	readkey();

	destroy_bitmap(text);

}