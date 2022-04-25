/*******************************************************************************************
*
*   HACKAGAME
*   Copyright (c) 2021-2022 Guillaume Lozenguez - Institut Mines-Telecom
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <pthread.h>
#include <netinet/in.h> 

#include "hackagames.h"
#include "hackagames-interface.h"

//-----------------------------------//
//--   Constructor / Destructor    --//
//-----------------------------------//

void Interface_construct(Interface * self, Organism* tabletop, int frameWidth, int frameHeight, float scale)
{
    int name_size= max( strlen(tabletop->name)+16, 32 );
    self->name= malloc( sizeof(char)*name_size );
    self->name[0]= '\0';
    self->name[31]= '\0';
    strcpy( self->name, tabletop->name );
    self->camera.x= 0.f;
    self->camera.y= 0.f;
    self->frameWidth= frameWidth;
    self->frameHeight= frameHeight;
    self->screenCenter.x= self->frameWidth/2.f;
    self->screenCenter.y= self->frameHeight/2.f;
    self->scale= scale; // meter per pixel
    self->tabletop= tabletop;
    self->cursor.x= 0.f;
    self->cursor.y= 0.f;
    self->isReady= false;
    self->attributMask_size= -1;
    self->attributMask= malloc( sizeof(int) );

    printf("NEW INTERFACE: scale: %f\n", self->scale);
}

Interface * Interface_new(Organism* tabletop, int frameWidth, int frameHeight, float scale)
{
    Interface * self = malloc( sizeof(Interface) );
    Interface_construct( self, tabletop, frameWidth, frameHeight, scale);
    return self;
}
Interface * Interface_newBasic()
{
    return Interface_new(NULL, 800, 600, 100.f );
}

void Interface_distroy( Interface * self )
{
    free(self->attributMask);
    self->frameWidth= 0;
    self->frameHeight= 0;
}

void Interface_delete( Interface * self )
{
    free( self );
}

// Raylib IHM Thread
void* void_InterfaceLoop(void* void_interface)
{
    Interface* self= (Interface*)void_interface;

    // Raylib Initialization
    //----------------------
    InitWindow(self->frameWidth, self->frameHeight, self->name);
    SetTargetFPS(60);
    
    while ( self->isReady && !WindowShouldClose() )
    {
        Interface_control(self);
        Interface_draw(self);
        TakeScreenshot("risky-screenshot.png");
    }
    
    // proper closing
    //---------------
    CloseWindow();   // Close window and OpenGL context
    self->isReady= false;

    return (void*)(self);
}

void Interface_startIHM(Interface * self)
{
    puts("HackaGames: Launch IHM Thread");
    self->isReady= true;

    int rc = pthread_create( 
                &(self->thread ),
                NULL,
                void_InterfaceLoop,
                (void*)(self)
            );
    if(rc)
    {
        printf("Error:unable to create IHM thread, %d\n", rc);
        self->isReady= false;
    }
}

void Interface_stopIHM(Interface * self)
{
    self->isReady= false;
    void* ret;
    pthread_join(self->thread, &ret);
}

bool Interface_IHMIsOpen(Interface* self)
{
    return self->isReady;
}

// To String
char* Interface_str(Interface * self, char* buffer)
{
    sprintf( buffer, "Interface camera(x-%f, y-%f, scale-%f)\n",
            self->camera.x,
            self->camera.y,
            self->scale);
    return buffer;
}


// Configuration
int* Interface_setAttributMask_ofSize(Interface* self, int* mask, int mask_size)
{
    free(self->attributMask);
    self->attributMask= malloc( sizeof(int) * max(mask_size, 1) );
    self->attributMask_size= max(mask_size, 0);
    for( int i= 0 ; i < self->attributMask_size ; ++i )
        self->attributMask[i]= mask[i];
    return self->attributMask;
}

// Rendering
void Interface_draw(Interface * self)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    Interface_drawTabletop(self, self->tabletop);
    //Interface_drawBasis(self);

    EndDrawing();
}

// Rendering
void Interface_drawTabletop(Interface * self, Organism * aTtop)
{
    // Draw connectivity:
    for(int i= 0 ; i < aTtop->size ; ++i )
    {
        Organism * cell= aTtop->cells[i];
        int cardinality= aTtop->links[i][0];
        for(int ii= 1 ; ii <= cardinality ; ++ii )
        {
            Interface_drawEdge( self, cell, aTtop->cells[ aTtop->links[i][ii] ] );
        }
    }

    // Draw the nodes
    for(int i= 0 ; i < aTtop->size ; ++i )
    {
        Interface_drawCell( self, aTtop->cells[i] );
    }
}

void Interface_drawBasis(Interface * self)
{
    Float2 ref= {0.f, 0.f};
    Vector2 screen00= Interface_pixelFromPosition(self, ref );
    ref.x= 1.f;
    Vector2 screen10= Interface_pixelFromPosition(self, ref );
    ref.x= 0.f;
    ref.y= 1.f;
    Vector2 screen01= Interface_pixelFromPosition(self, ref );

    DrawCircleV( screen00, 4, BLUE );   
    DrawLineV( screen00, screen10, RED );
    DrawLineV( screen00, screen01, BLUE );
}

void Interface_drawCell(Interface * self, Organism * aOrganism)
{
    Vector2 screenPosition= Interface_pixelFromPosition(self, aOrganism->position );
    Color color= {
        Color_red(aOrganism->color),
        Color_green(aOrganism->color),
        Color_blue(aOrganism->color),
        Color_alpha(aOrganism->color)
    };

    int radius= (int)( aOrganism->shape * self->scale );

    DrawCircleV(screenPosition, radius+4, color );// aOrganism->color);
    DrawCircleV(screenPosition, radius-4, RAYWHITE);
    for( int i= 0 ; i < aOrganism->size ; ++i )
    {
        Interface_drawPiece(self, aOrganism->position, aOrganism->cells[i]);
    }

    DrawText(aOrganism->name,
        (int)(screenPosition.x)+10,
        (int)(screenPosition.y)+20, 22,
        BLACK);
}


void Interface_drawEdge(Interface * self, Organism * aOrganism, Organism * aTarget)
{
    Vector2 source= Interface_pixelFromPosition(self, aOrganism->position);
    Vector2 target= Interface_pixelFromPosition(self, aTarget->position);
    
    Vector2 ortho= (Vector2){ target.x-source.x, target.y-source.y }; // Vector from Source to Target
    float ratio= (aOrganism->shape*self->scale) / Vector2Length( ortho );
    ortho= (Vector2){ -ortho.y*ratio, ortho.x*ratio };// 10 ortho-normal vector from v 

    Vector2 source1= (Vector2){source.x-ortho.x, source.y-ortho.y};
    Vector2 source2= (Vector2){source.x+ortho.x, source.y+ortho.y};
    
    Color color= {
        Color_red(aOrganism->color),
        Color_green(aOrganism->color),
        Color_blue(aOrganism->color),
        Color_alpha(aOrganism->color)
    };

    DrawTriangle(source1, source2, target, color);
}

void Interface_drawPiece(Interface * self, Float2 position, Organism * minion)
{
    Float2 tmpPosition= { position.x+minion->position.x, position.y+minion->position.y };
    Vector2 screenPosition= Interface_pixelFromPosition(self, tmpPosition );
    int radius= (int)(minion->shape*self->scale);

    Vector2 A= { screenPosition.x, screenPosition.y-radius*2 };
    Vector2 B= { screenPosition.x-(radius-4), screenPosition.y };
    Vector2 C= { screenPosition.x+(radius-4), screenPosition.y };
    
    Color color= {
        Color_red(minion->color),
        Color_green(minion->color),
        Color_blue(minion->color),
        Color_alpha(minion->color)
    };

    DrawCircleV(screenPosition, radius+2, color); //minion->color);
    DrawCircleV(screenPosition, radius-4, RAYWHITE);

    DrawTriangle(A, B, C, color); //minion->color);
    DrawCircleV(A, radius-4, color); //minion->color);

    // Generation attribute strings:
    char attributes_str[512]= "";
    char buffer[32];

    if( self->attributMask_size < 0 && minion->attrs_size > 0 )
    {// Regular way:
        sprintf(attributes_str, "%d", Organism_attribute(minion, 0) );
        for( int i = 1 ; i < minion->attrs_size ; ++i )
        {
            sprintf(buffer, "-%d", Organism_attribute(minion, i) );
            strcat( attributes_str, buffer );
        }
    }
    else if( self->attributMask_size > 0)
    {// selection of attributs based on Mask:
        assert( 0 <= self->attributMask[0] && self->attributMask[0] < minion->attrs_size ); // Mask out off the attribute range. 
        sprintf( attributes_str, "%d", Organism_attribute(minion, self->attributMask[0] ) );
        for( int i = 1 ; i < self->attributMask_size ; ++i )
        {
            assert( 0 <= self->attributMask[i] && self->attributMask[i] < minion->attrs_size ); // Mask out off the attribute range. 
            sprintf(buffer, "-%d", Organism_attribute(minion, self->attributMask[i]) );
            strcat( attributes_str, buffer );
        }
    }
    DrawRectangle(B.x+4, B.y, 4+8*strlen(attributes_str), 16, WHITE);
    DrawText( attributes_str, B.x+6, B.y, 16, BLACK);
}

Vector2 Interface_pixelFromPosition(Interface * self, Float2 p)
{
    Vector2 pixel= {
        self->screenCenter.x + (p.x - self->camera.x ) * self->scale,
        self->screenCenter.y - (p.y - self->camera.y ) * self->scale
    };
    return pixel;
}

Float2 Interface_positionFromPixel(Interface * self, Vector2 pix)
{
    Float2 position= {
        (pix.x - self->screenCenter.x) / self->scale + self->camera.x,
        (self->screenCenter.y - pix.y) / self->scale + self->camera.y
    };
    return position;
}

void Interface_control(Interface * self)
{
    Interface_controlCursor(self);
    Interface_controlCamera(self);
}

void Interface_controlCursor(Interface * self)
{
    Vector2 pixel= GetMousePosition();
    self->cursor= Interface_positionFromPixel(self, pixel);
}

void Interface_controlCamera(Interface * self)
{
    // KEYBOARD Control:
    float step= 3.0f / self->scale;
    if (IsKeyDown(KEY_RIGHT)) self->camera.x += step;
    if (IsKeyDown(KEY_LEFT)) self->camera.x -= step;
    if (IsKeyDown(KEY_UP)) self->camera.y += step;
    if (IsKeyDown(KEY_DOWN)) self->camera.y -= step;

    int move= GetMouseWheelMove();
    self->scale += ( (float)move * 1.f);
    self->scale = max( self->scale, 0.001f );
}
