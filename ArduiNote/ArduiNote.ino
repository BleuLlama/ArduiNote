/*  Arduinote
 *
 * 	2015-01-06 yorgle@gmail.com
 *
 *	a simple slideshow presenter using TVOut
 *
 *  This software is distributed under the MIT License
 *
 * 2015-01-09  1.2  Updated the demo presentation to be about ArduiNote
 *                  Updated for Arduino 1.5.8
 *
 * 2015-01-09  1.1  Serial interface added with screendump and terminal display of text
 *
 * 2015-01-06  1.0  Initial release
 *
 */

#include <TVout.h>
#include <avr/pgmspace.h>

#define VERSIONA " uNote v1.2 2015/1/9"
#define VERSIONB "  yorgle@gmail.com"

#define kPageDown	A0
#define kPageUp		A1

TVout TV;
unsigned char x, y;

char buffer[26];

void setup()  {
  x = 0;
  y = 0;
  TV.start_render(_NTSC );
  //TV.begin(_NTSC, 128,56 );	//for devices with only 1k sram(m168)
  TV.clear_screen();

  // buttons for forward/reverse
  pinMode( kPageDown, INPUT );
  pinMode( kPageUp, INPUT );
  
  Serial.begin( 115200 );
  loadString( 1 );
  Serial.println( buffer );
  loadString( 2 );
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
const char PROGMEM s000[] PROGMEM = "    ArduiNote";
const char PROGMEM s001[] PROGMEM = "  ";
const char PROGMEM s002[] PROGMEM = "  Scott Lawrence";
const char PROGMEM s003[] PROGMEM = " yorgle@gmail.com";
const char PROGMEM s004[] PROGMEM = "    2015-01-09";

const char PROGMEM s200[] PROGMEM = "Overview";
const char PROGMEM s201[] PROGMEM = "_* KeyNote from AVR";
const char PROGMEM s202[] PROGMEM = "* Self Contained ";
const char PROGMEM s203[] PROGMEM = "* Simple Circuit";
const char PROGMEM s204[] PROGMEM = "* Just add power";

const char PROGMEM s300[] PROGMEM = "Hardware";
const char PROGMEM s301[] PROGMEM = "_* Arduino (AT328+)";
const char PROGMEM s302[] PROGMEM = "* Two buttons A0, A1";
const char PROGMEM s303[] PROGMEM = "* LED for power";
const char PROGMEM s304[] PROGMEM = "* Composite Plug";
const char PROGMEM s305[] PROGMEM = "* 5 resistors";

const char PROGMEM s400[] PROGMEM = "Features";
const char PROGMEM s401[] PROGMEM = "_* Simple use";
const char PROGMEM s402[] PROGMEM = "* Simple hookup";
const char PROGMEM s403[] PROGMEM = "* Serial interface";
const char PROGMEM s404[] PROGMEM = "* Screendumps";

const char PROGMEM s500[] PROGMEM = "Limitations";
const char PROGMEM s501[] PROGMEM = "_* Hardcoded text";
const char PROGMEM s502[] PROGMEM = "* Program Memory";
const char PROGMEM s503[] PROGMEM = "* Source Editing";
const char PROGMEM s504[] PROGMEM = "* Cumbersome Editing";

const char PROGMEM s600[] PROGMEM = "Next Version";
const char PROGMEM s601[] PROGMEM = "_* SD for Slides";
const char PROGMEM s602[] PROGMEM = "* SD for BMPs";
const char PROGMEM s603[] PROGMEM = "* SD for Animations";

/* utility strings */
const char PROGMEM uEndSlide[] PROGMEM = "";
const char PROGMEM uEnd[] PROGMEM = "End.";
const char PROGMEM uVersionA[] PROGMEM = VERSIONA;
const char PROGMEM uVersionB[] PROGMEM = VERSIONB;

// slide arrangement of strings
PROGMEM const char * const string_list[] =
{
  /* 0: Utility */
  uEnd, uVersionA, uVersionB, 
  
  /* Slides  */
  /* 3 */
  s000, s001, s002, s003, s004, uEndSlide,
  /* 9 */
  s200, s201, s202, s203, s204, uEndSlide,
  /* 15 */
  s300, s301, s302, s303, s304, s305, uEndSlide,
  /* 22 */
  s400, s401, s402, s403, s404, uEndSlide,
  /* 28 */
  s500, s501, s502, s503, s504, uEndSlide,
  /* 34 */
  s600, s601, s602, s603, uEndSlide,

  /* utility strings */
  uEnd
};

// indexes for each slide - could make it more dynamic using a scanner
// end with a -1
char slideidx[] = {
  3, 9, 15, 22, 28, 34, 
  -1
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
    loadString( 0 );
    TV.print_str( (TV.horz_res()/2)-(4*5) , (TV.vert_res() /2)-8, buffer );
    if( page != lastPage ) {
      Serial.println( buffer );
    }
        
    // version info at the bottom
    loadString( 1 );
    TV.print_str( 0 , TV.vert_res() - 18, buffer );
    loadString( 2 );
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
        loadString( 1 );
        Serial.print( buffer );
        Serial.print( F("  "));
        loadString( 2 );
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
