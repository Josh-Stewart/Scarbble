//mfcalleg.h

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
  This is a quick hack by me to make allegro play nicely with MFC
  This should be included before any mfc headers

  -Josh
*/

#ifndef MFCALLEG_HEADER
#define MFCALLEG_HEADER

//this fixes some warnings when using this header in a few different .cpp files
//because these are redefined by MFC headers in other .cpp files
#undef BITMAP
#undef TRACE
#undef ASSERT

//redefine magic
#define BITMAP WINDOWS_BITMAP

//#define WIN32_LEAN_AND_MEAN 
//not a good idea cos we are using mfc/cpp, but prolly wont break anything

#include "allegro.h"

//undefine stuff that gives us trouble
//dont worry MFC will redefine these if we need them
#undef BITMAP
#undef TRACE
#undef ASSERT


#endif // MFCALLEG_HEADER