/*  Arduinote / microNote / uNote
 *
 * 	2015-01-06 yorgle@gmail.com
 *
 *	a simple slideshow presenter using TVOut
 *
 * 2015-01-09  1.1  Serial interface added with screendump and terminal display of text
 *
 * 2015-01-06  1.0  Initial release
 *
 */

#include <TVout.h>
#include <avr/pgmspace.h>

#define VERSA	" uNote v1.1 2015/1/9"
#define VERSB	"  yorgle@gmail.com"

#define kPageDown	A0
#define kPageUp		A1

TVout TV;
unsigned char x, y;

char buffer[26];

void setup()  {
  x = 0;
  y = 0;
  TV.start_render(_NTSC);	//for devices with only 1k sram(m168) use TV.begin(_NTSC,128,56)
  TV.clear_screen();

  // buttons for forward/reverse
  pinMode( kPageDown, INPUT );
  pinMode( kPageUp, INPUT );
  
  Serial.begin( 115200 );
  loadString( 42 );
  Serial.println( buffer );
  loadString( 43 );
  Serial.println( buffer );
}

// the page to display
int page = 0;

// number of presses of each
int pressedPageDown = 0;
int pressedPageUp = 0;


// current state, last state, release count
int a, la, b, lb, r;

// pagemove
void previousPage( void )
{
  page--;
  r++;
}

void nextPage( void )
{
  page++;
  r++;
}


// handle reading the buttons
void handleButtons() {
  a = digitalRead( kPageDown );
  if ( a != la && a == HIGH ) {
    pressedPageDown++;
    previousPage();
  }
  la = a;

  b = digitalRead( kPageUp );
  if ( b != lb && b == HIGH) {
    pressedPageUp++;
    nextPage();
  }
  lb = b;
}

// Slide content (internal hardcoded, canned for now)
// item 0 per page is drawn at x=0
// items afterwards are drawn with 
// underscore as first character puts a line before it
//                         "01234567890123456789"
prog_char s000[] PROGMEM = "";
prog_char s001[] PROGMEM = "   Scott Stuff";
prog_char s002[] PROGMEM = "  ";
prog_char s003[] PROGMEM = "  Scott Lawrence";
prog_char s004[] PROGMEM = "    2015-01-06";

prog_char s100[] PROGMEM = "Sinclair ZX81";
prog_char s101[] PROGMEM = "_- Video Out Mod";
prog_char s102[] PROGMEM = "- Keyboard Repair";
prog_char s103[] PROGMEM = "- Internal 16k RAM";
prog_char s104[] PROGMEM = "_+ Lit Power Switch";
prog_char s105[] PROGMEM = "+ Basic Video Out";
prog_char s106[] PROGMEM = "+ USB for 5v Power";
prog_char s107[] PROGMEM = "+ Board wedge";

prog_char s200[] PROGMEM = "Apple ][c";
prog_char s201[] PROGMEM = "_- RAM chip swap";
prog_char s202[] PROGMEM = "- RAM replace?";

prog_char s300[] PROGMEM = "Doorduino 2000";
prog_char s301[] PROGMEM = "_+ Basic design";
prog_char s302[] PROGMEM = "+ Web UI";
prog_char s303[] PROGMEM = "+ Kinda open door";
prog_char s304[] PROGMEM = "_- Outside door";
prog_char s305[] PROGMEM = "- Intercom lifter";

prog_char s400[] PROGMEM = "TV Out";
prog_char s401[] PROGMEM = "_+ Composite Cable";
prog_char s402[] PROGMEM = "+ Learn TV Library";
prog_char s403[] PROGMEM = "+ ArduiNote (11k)";
prog_char s404[] PROGMEM = "_- Homebrew Computer";

prog_char s900[] PROGMEM = "Into currently";
prog_char s901[] PROGMEM = "_- Canto Ostinato";
prog_char s902[] PROGMEM = "   Simeon ten Holt";
prog_char s903[] PROGMEM = "- RLUG (Lego)";
prog_char s904[] PROGMEM = "- Floorplan Rework";
prog_char s905[] PROGMEM = "- Portable Computer";

prog_char uEnd[] PROGMEM = "End.";
prog_char uVersA[] PROGMEM = VERSA;
prog_char uVersB[] PROGMEM = VERSB;

// slide arrangement of strings
PROGMEM const char * string_list[] =
{
 /* 0: title slide  */
 s001, s002, s002, s003, s004, s000,
 /* 6: ZX */
 s100, s101, s102, s103, s104, s105, s106, s107, s000,
 /* 15: Apple IIc */
 s200, s201, s202, s000,
 /* 19: Doorduino */
 s300, s301, s302, s303, s304, s305, s000,
 /* 26: Arduinote */
 s400, s401, s402, s403, s404, s000, s000,
 /* 33: into */
 s900, s901, s902, s903, s904, s905, s000,
 /* end */
 s000,

 /* 39: utility strings */
 uEnd,
 uVersA,
 uVersB,
};

// indexes for each slide - could make it more dynamic using a scanner
char slideidx[] = {
  0, 6, 15, 19, 26, 33, -1
};

// this pulls content out of PROGMEM for general use
void loadString( int idx )
{
  strcpy_P( buffer, (char*)pgm_read_word(&(string_list[idx])));
}

void dumpScreenSerial( char * one, char * zero )
{
  int w = TV.hres();
  int h = TV.vres();
  
  Serial.print( w, DEC );
  Serial.print( F(", ") );
  Serial.println( h, DEC );
  
  // we'll do our own thing here instead of get_pixel to speed it up
  for( int y=0 ; y<h ; y++ ) {
    int offsy = y * w/8; // row offset into the buffer
    
    for( int x=0 ; x<w ; x=x+8 ) {
      int offs = offsy + (x/8); // row + col offset into the buffer
      int b = TV.screen[ offs ]; // get the byte there
      for( int m=0x080 ; m ; m>>=1 ) // iterate over the bits
      {
        Serial.print( (b&m)==m? one : zero ); // and print out each
      }
      
      /* the simpler, slower way
      TV.get_pixel( x, y );
      Serial.print( (b)?one:zero );
      */
    }
    Serial.println();
  }
}

void drawPage( int page )
{
  static int lastPage = -999;
  
  // find page
  int x = 0;
  while( slideidx[x] != -1 && x != page ) x++;

  if ( page != lastPage ) {
    sprintf( buffer, "(%d)", page );
    Serial.println();
    Serial.println();
    Serial.println( buffer );
  }
  
  // not found. print END.
  if( slideidx[x] == -1 ) {
    // center "END"
    loadString( 41 );
    TV.print_str( (TV.horz_res()/2)-(4*5) , (TV.vert_res() /2)-8, buffer );
    if( page != lastPage ) {
      Serial.println( buffer );
    }
        
    // version info at the bottom
    loadString( 42 );
    TV.print_str( 0 , TV.vert_res() - 18, buffer );
    loadString( 43 );
    TV.print_str( 0 , TV.vert_res() - 9, buffer );
    
    lastPage = page;
    return;
  }
  
  // get our starting index
  int strid = slideidx[x];
  
  // for each content line
  int l = 0;
  int drawline = 0;
  // title page offset
  if( page == 0 ) {
    l = 5;
  }

  // prime with the first string
  loadString( strid );
  
  // for each row in this slide
  while( buffer[0] != '\0' ) {
    
    // check for underscore (hint to draw hline below this text)
    drawline = 0;
    if( buffer[0] == '_' ) {
      drawline = 1;
    }
    
    if( drawline ) {
      // and draw it...
      int ypos = (8*l) + 3;
      TV.draw_line(0, ypos, TV.horz_res()-1, ypos, 1);
      l++;
      if( page != lastPage ) Serial.println( F("----------------------") );
    }

    // output our text
    TV.print_str( (l==0)? 0 : (2 * 5), l * 8, &buffer[drawline] );
    if( page != lastPage ) {
      if( l>0 ) Serial.print( "  " );
      Serial.println( &buffer[drawline] );
    }
    l++; // next screen line
    
    strid++; // next display line
    
    // prime for the next string
    loadString( strid );
  }
  
  if( page != 0 ) {
    // print out the page number
    sprintf( buffer, "%d", page );
    TV.print_str( 0, TV.vert_res()-8, buffer );
  }
  
  //if( lastPage != page )
  //{
  //  dumpScreenSerial("#"," ");
  //}
  
  lastPage = page;
}

void handleSerial( void )
{
  if( Serial.available() )
  {
    switch( Serial.read() ) {
      case( 'n' ): nextPage(); break;
      case( 'p' ): previousPage(); break;
      case( 'd' ): dumpScreenSerial( "#", " " ); break;
      case( 'b' ):
        Serial.println( F("P1") );
        Serial.print( F("# Page ") );
        Serial.print( page, DEC );
        Serial.print( F("  "));
        loadString( 42 );
        Serial.print( buffer );
        Serial.print( F("  "));
        loadString( 43 );
        Serial.println( buffer );
        dumpScreenSerial( "1 ", "0 " );
        break;
        
      case( 'h' ):
      case( '?' ):
        Serial.println( F( "n .. next" ));
        Serial.println( F( "p .. previous" ));
        Serial.println( F( "d .. dump" ));
        Serial.println( F( "b .. PBM" ));
        break;
      default:
        break;
      }
  }
}

void loop() {
  // check for advance/prevance button presses
  handleButtons();
  
  // check for serial input
  handleSerial();
  
  // if either button is down, clear the screen with white.
  if ( a == 0 || b == 0 ) {
    TV.fill( 1 ); // this can be removed.
    return;
  }
  
  // if we're just transitioning to the next slide (button released)
  // then clear the screen with black
  if ( r != 0 ) {
    TV.fill( 0 );
    r = 0;
  }

  // bounds checking for pages
  if ( page < 0 ) page = 0;
  if ( page > 10 ) page = 10;

  drawPage( page );
}
