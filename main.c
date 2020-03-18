#include "U8glib.h"

#define PL_STEP 3
#define PL_STEPY 3
#define ENLIMIT 2
#define BIRDDX 3
#define BIRDDY 2
#define FRTOG 3
#define SPAUSE 25
#define BULLSTEP 5

char pltog = 0;

bool status = 1;
char encount = 0;

char shootPause = 0;

U8GLIB_ST7920_128X64_4X u8g(10);


#define BALL_CONT 10

struct {
  char st;
  int x;
  int y;
  char dx;
  char ftog;
  char i;
  
  } bullets[3] = {0};

char pl_i = 0;
int plx = -3, ply = 64 - 16;

char pldx = 0, pldy = 0;

char plgc = 0;

bool jumping = 0;

#define PLG 6

int offset = 0;

#define ZONES 3

#define WIDTH (ZONES * 128)

char zone = 0;

struct hole {
    int start_x;
    int finish_x;
    
    };

    struct cloudstrc
    {
      int x;
      int y;
      
      };


struct hole holes0[] = {{16, 32}, {48, 72}};

struct hole holes1[] = {{16, 48}, {56, 80}};

struct hole holes2[] = {{24, 48}, {48, 80}};

struct cloudstrc clouds1[] = {{36, 5}, {96, 8}};

struct cloudstrc clouds2[] = {{36, 5}, {96, 8}};

struct cloudstrc clouds3[] = {{28, 8}, {80, 3}};

struct {
    char st;
    int x;
    int y;
    char type;
    char fc;
    char fi;
    char dx;
    char dy;
  } enemies[ENLIMIT] = {0};

struct
{
  hole* holes;
  char num_holes;
  cloudstrc* clouds;
  char num_clouds;
  
  } zones[] = {
      {holes0, 2, clouds1, 2}, {holes1, 2, clouds2, 2}, {holes2, 2, clouds3, 2}
    
    };


const uint8_t ball[] U8G_PROGMEM = {
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  };

const uint8_t bullet[][3] U8G_PROGMEM = {
  
  0b000,
  0b111,
  0b000, 

  0b001,
  0b111,
  0b001,
  
  };

const uint8_t cloud[] U8G_PROGMEM ={

0b10011100, 0b01111011,
0b11111110, 0b11111111,
0b11111111, 0b11111111,
0b11111111, 0b00111111,

0b10111100, 0b00111001

 
  };


const uint8_t bird[][6] U8G_PROGMEM ={


0b10010000,
0b11011000,
0b01111110,
0b11111101,
0b00101000,
0b00000000,


0b01001000,
0b01101100,
0b01111110,
0b11111101,
0b00010100,
0b00000000,


};


const uint8_t grass[] U8G_PROGMEM ={
  
0b10101010,   
0b01010101, 
0b11111111,   
  };

const uint8_t pl_right[][8] U8G_PROGMEM = {

  0b00011000, 
  0b00101110, 
  0b00111110, 
  0b01111111, 
  0b11011101, 
  0b00011100, 
  0b00110010, 
  0b00100001,

0b00011000, 
  0b00101110, 
  0b00111110, 
  0b01111111, 
  0b11011101, 
  0b00011100, 
  0b00010010, 
  0b00010010,

  
/*
   0xff, 
0xff, 
0xff, 
0xff, 
0xff, 
0xff, 0xff, 0xff, */

  };


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
// assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  randomSeed(analogRead(0));

}

void dispClouds()
{
  for(char i = 0; i < zones[zone].num_clouds; i++)
  {
      u8g.drawXBMP(zones[zone].clouds[i].x - zone * 128 - offset, zones[zone].clouds[i].y, 16, 5, cloud);
    
    }

  if(offset % 128)
  {

  for(char i = 0; i < zones[zone + 1].num_clouds; i++)
  {
      u8g.drawXBMP(zones[zone + 1].clouds[i].x - (zone + 1) * 128 - offset, zones[zone + 1].clouds[i].y, 16, 5, cloud);
    
    }

  } 
  }

void dispGrass(int x1, int x2)
{

  int cur_x = x1;

  do
  {
    u8g.drawXBMP(cur_x, 64 - 3, 8, 3, grass);
      cur_x += 8;
   
    }
    while(cur_x < x2);
  
  }


void dispEnemies()
{
    for(char i = 0; i < ENLIMIT; i++)
    {
      if(enemies[i].st)
      {
          if(enemies[i].st == 2)
             u8g.drawXBMP(enemies[i].x - offset, enemies[i].y, 5, 5, ball);
          else if(enemies[i].type == 1) 
            u8g.drawXBMP(enemies[i].x - offset, enemies[i].y, 8, 6, bird[enemies[i].fi]);


if(++enemies[i].fc >= FRTOG)
        {
            enemies[i].fi ^= 1;
            enemies[i].fc = 0;
          }

        }

        
      
      }
  }


void disp()
{
  if(!status)
  {
 
    u8g.firstPage();  
    do {


      u8g.setFont(u8g_font_unifont);
  
  u8g.drawStr( 8, 20, "Game over");
    } while( u8g.nextPage() );

    
    return;
    }

  
  u8g.firstPage();  
  do {

  int last_p = 0;

  if(offset % 8 != 0)
    last_p = -(offset % 8);

  for(char i = 0; i < zones[zone].num_holes; i++)
  {
  

    
      dispGrass(last_p,  zones[zone].holes[i].start_x + zone * 128 - offset);
      last_p = zones[zone].holes[i].finish_x + zone * 128 - offset;

      
    }
    if(offset % 128)
    {
      
  for(char i = 0; i < zones[zone + 1].num_holes; i++)
  {
  

    
      dispGrass(last_p,  zones[zone + 1].holes[i].start_x + (zone + 1) * 128 - offset);
      last_p = zones[zone + 1].holes[i].finish_x + (zone + 1) * 128 - offset;

      
    }
      
      }

    if(last_p < 128)
      dispGrass(last_p, 128);

dispClouds();
dispEnemies();



  u8g.drawXBMP(plx - offset, ply, 8, 8, pl_right[pl_i]);

  for(char i = 0; i < 3; i++)
  {
      if(bullets[i].st)
      {

        if(++bullets[i].ftog >= FRTOG)
        {
            bullets[i].ftog = 0;
            bullets[i].i ^= 1;
          
          }
        
        u8g.drawXBMP(bullets[i].x - offset, bullets[i].y, 3, 3, bullet[bullets[i].i]);
      
      
      }
    }

     
  } while( u8g.nextPage() );
  
}



unsigned int deepness(int x, int y)
{
    if(y <= 64 - 3)
    {
      for(char i = 0; i < zones[zone].num_holes; i++)
      {
          if(plx > zone * 128 + zones[zone].holes[i].start_x && plx + 8 < zone * 128 + zones[zone].holes[i].finish_x)
            return 100;

          
        }

  if(offset % 128)
  {


   for(char i = 0; i < zones[zone + 1].num_holes; i++)
      {
          if(plx > (zone + 1) * 128 + zones[zone + 1].holes[i].start_x && plx + 8 < (zone + 1) * 128 + zones[zone + 1].holes[i].finish_x)
            return 100;

          
        }
    
    }
      
      return 64 -3;
    }
     else
      return 100;
  }

void addEnemy()
{
  
  for(char i = 0; i < ENLIMIT; i++)
  {
      if(!enemies[i].st)
      {
        
          enemies[i].x = offset + 128;
          enemies[i].y = random(0, 50);
          enemies[i].dx = -BIRDDX;
          enemies[i].dy = 0;
          enemies[i].st = 1;
          enemies[i].type = 1;

          encount++;
          break;
        }
    }
  }


void enemyAction()
{
  
    for(char i = 0 ; i < ENLIMIT; i++)
    {
        if(enemies[i].st)
        {
            if(enemies[i].st == 2 && --enemies[i].fc <= 0)
            {
              enemies[i].st = 0;
              continue;
            }
          
            if(enemies[i].type == 1)
            {
                enemies[i].dy = random(-BIRDDY, BIRDDY + 1);

                if(enemies[i].dy < 0 && enemies[i].y < 0 || enemies[i].dy > 0 && enemies[i].y >= 64)
                  enemies[i].dy = -enemies[i].dy;
              
                enemies[i].x += enemies[i].dx;
                enemies[i].y += enemies[i].dy;

               
              }


               if(enemies[i].x <= offset -8 || enemies[i].y <= -6 || enemies[i].y >= 64)
               {
                  enemies[i].st = 0;
                  encount--;
               }
          
          }
      
      }
  }


bool intersects(int x1, int y1, char w1, char h1, int x2, int y2, char w2, char h2)
{
    if((x1 <= x2 && x2 <= x1 + w1 || x2 <= x1 && x1 <= x2 + w2) && (y1 <= y2 && y2 <= y1 + h1 || y2 <= y1 && y1 <= y2 + h2))
      return true;
    else
      return false;  
  
  }

void loop() {
  // put your main code here, to run repeatedly:

  if(!status)
    return;

  if(!digitalRead(2))
  {
    pldx = -PL_STEP;
     
    }
    else if(!digitalRead(3))
  {
  pldx = PL_STEP;
    
     
    }
    else if(!jumping)
      pldx = 0;

    if(!digitalRead(5) && shootPause <= 0)
    {
      for(char i = 0; i < 3; i++)
      {
          if(!bullets[i].st)
          {
              bullets[i].st = 1;
              bullets[i].x = plx + 8;
              bullets[i].y = ply + 2;
              bullets[i].dx = BULLSTEP;
              shootPause = SPAUSE;
              break;
            
            }
        
        }
      
      } 


if((plx += pldx) < 0)
      plx = 0;


  

  if(plx >= WIDTH - 8)
    plx = WIDTH - 8;

  if(plx - offset >= 64 && pldx > 0 || plx - offset < 64 && pldx < 0)
    offset += pldx;

   if(offset < 0)
    offset = 0;
   else if(offset > WIDTH - 128)
    offset = WIDTH - 128;

    if(!digitalRead(4))
     {
      pldy = -PL_STEPY;
      jumping = true;
     }

  int deepness1 = deepness(plx, ply + 8);

  if(ply + 8 < deepness1 && !jumping)
  {
    jumping = true;
    plgc = 0;
    pldy = 1;
  }

  ply += pldy;

    if(jumping && deepness1 != deepness(plx, ply + 8))
    {
        ply = deepness1 - 8;
        pldy = 0;
        jumping = false;
       }
       else if(ply > 64)
        status = false;



  if(jumping)
  {
      if(++plgc >= PLG)
      {
          pldy++;
          plgc = 0;
        }

      
    }
      
    zone = offset / 128;

  if(shootPause > 0)
    shootPause--;


  for(char i = 0; i < 3; i++)
  {
      if(bullets[i].st)
      {
          bullets[i].x += bullets[i].dx;
        
          if(bullets[i].x + 3 < offset || bullets[i].x > offset + 128)
          {
              bullets[i].st = 0;
              continue;
            
            }


            for(char j = 0; j < ENLIMIT; j++)
            {
              if(enemies[j].st == 1 && intersects(bullets[i].x, bullets[i].y, 3, 3, enemies[j].x, enemies[j].y, 8, 8))
              {
                  enemies[j].st = 2;
                  bullets[i].st = 0;
                  enemies[j].fc = BALL_CONT;
                  encount--;
                  break;
                
                }
            }
        
        }
    }


    if(!encount) 
      addEnemy();

  enemyAction();
    
  disp();

  if(++pltog >= FRTOG)
  {
    pl_i ^= 1;
    pltog = 0;
  }
  delay(50);
}

