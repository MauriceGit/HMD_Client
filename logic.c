
/**
 * @file
 * Hier ist die Datenhaltung und Programmlogik
 *
 * @author Tilman Nedele, Maurice Tollmien
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "vector.h"
#include "types.h"

/** Mausbewegung zoom/move/none */
MouseInterpretType G_Mouse;
/** Kameraposition */
Vec3D G_CameraPosition;
/** Position der Maus */
Movement G_MouseMove = {0,0,0};
/** Startpunkt der Mausbewegung */
Vec3D G_LastMouseCenter;

/** Partikel hinzufügen? */
int G_UpDownKeys[2] = {0,0};
/** FPS */
int G_FPS = 0;
/** FPS counter */
double G_Counter = 0.0-EPS;

/* ------- GETTER / SETTER ------- */

int getFPS(void)
{
    return G_FPS;
}

/**
 * Setzt, ob ein Up/Down-Key gedrückt ist.
 * key == 0 = down
 * key == 1 = up
 * value = gesetzt/nicht gesetzt.
 */
void setKey (int key, int value)
{
    if (key < 2)
        G_UpDownKeys[key] = value;
}

/**
 * Set-Function für den Status der Maus
 * @param Status der Maus
 */
void setMouseEvent(MouseInterpretType state,int x, int y) {
    G_MouseMove[0] = 0.0;
    G_MouseMove[2] = 0.0;
    G_LastMouseCenter.x = x;
    G_LastMouseCenter.z = y;

    G_Mouse = state;
}

/**
 * Mouse Status.
 */
void setMouseState(MouseInterpretType state) {
    G_Mouse = state;
}

void setMouseCoord(int x, int y) {
    G_LastMouseCenter.x = x;
    G_LastMouseCenter.z = y;
}

/**
 * Get-Function für den Status der Maus
 * @return Status der Maus
 */
MouseInterpretType getMouseEvent() {
    return G_Mouse;
}

/**
 * Gibt die Kamerakoordinate zurück.
 */
double getCameraPosition (int axis)
{
    switch (axis) {
        case 0: return G_CameraPosition.x;
        case 1: return G_CameraPosition.y;
        case 2: return G_CameraPosition.z;
    }
    return 0.0;
}

/**
 * Bewegt die Kamera auf einem Kugelradius um den Szenenmittelpunkt.
 */
void setCameraMovement(int x,int y)
{
    Vec3D tmp = toVector3D(G_CameraPosition.x, G_CameraPosition.y,G_CameraPosition.z);
    double factor, radius = vectorLength3D(G_CameraPosition);

    G_MouseMove[0] = x-G_LastMouseCenter.x;
    G_MouseMove[2] = y-G_LastMouseCenter.z;

    G_LastMouseCenter.x = x;
    G_LastMouseCenter.z = y;

    /* Bewegung um Y-Achse: */
    G_CameraPosition.x = cos(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp.x + sin(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp.z;
    G_CameraPosition.z = -sin(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp.x + cos(-G_MouseMove[0]*PI/180.0/CAMERA_MOVEMENT_SPEED)*tmp.z;

    /* Bewegung oben/unten */
    G_CameraPosition.y += G_MouseMove[2]/(CAMERA_MOVEMENT_SPEED/2)*(vectorLength3D(G_CameraPosition)/100.0);
    factor = 1.0 / (vectorLength3D(G_CameraPosition) / radius);
    G_CameraPosition = multiplyVectorScalar (G_CameraPosition, factor);

}

/**
 * Verlängert/verkürzt den Vektor zur Kamera.
 */
void setCameraZoom(int x,int y)
{
    double factor = 1.0 + (CAMERA_ZOOM_SPEED / 1000.0) * ((G_MouseMove[2] < 0.0) ? -1.0 : 1.0);

    G_MouseMove[0] = x-G_LastMouseCenter.x;
    G_MouseMove[2] = y-G_LastMouseCenter.z;

    G_LastMouseCenter.x = x;
    G_LastMouseCenter.z = y;

    G_CameraPosition = multiplyVectorScalar(G_CameraPosition, factor);
}

/* ------- BERECHNUNGEN ------- */

/**
 * Berechnet alle Funktionen, die vom interval abhängig sind
 * @param interval - Zeit
 */
void calcTimeRelatedStuff (double interval)
{
    G_Counter += interval;

    if (G_Counter >= 1.0)
        G_Counter = 0.0 -EPS;

    if (G_Counter <= 0.0)
        G_FPS = (int) 1.0/interval;
}

/* ------- INIT ------- */

/**
 * Initialisiert die Kamera.
 */
void initCameraPosition ()
{
    G_CameraPosition.x = CAMERA_X;
    G_CameraPosition.y = CAMERA_Y;
    G_CameraPosition.z = CAMERA_Z;
}

/**
 * Hier findet die komplette Initialisierung des kompletten SPIEeles statt.
 * Inklusive der Datenhaltung und des SPIEelfeldes.
 */
void initGame ()
{
    G_LastMouseCenter = toVector3D(0,0,0);
    initCameraPosition();
}
