/**
 * Logic regarding camera movements controlled by a joystick.
 *
 * @author Maurice Tollmien. Github: MauriceGit
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* ---- Eigene Header einbinden ---- */
#include "joystickCamera.h"
#include "vector.h"
#include "types.h"
#include "hmd.h"
#include "quaternions.h"

/** Kameraposition */
Vec3D G_JoyUpVector;
Vec3D G_JoyViewVector;
Vec3D G_JoyCameraTranslation;
Vec3D G_JoyCameraPosition;

/* ------- GETTER / SETTER ------- */

/**
 * Gibt die Kamerakoordinate zurück.
 */
double getJoyCameraPosition (int axis)
{
    switch (axis) {
        case 0: return G_JoyCameraPosition.x;
        case 1: return G_JoyCameraPosition.y;
        case 2: return G_JoyCameraPosition.z;
        default: return 0.0;
    }
}

double getJoyUp(int axis) {
    switch (axis) {
        case 0: return G_JoyUpVector.x;
        case 1: return G_JoyUpVector.y;
        case 2: return G_JoyUpVector.z;
        default: return 0.0;
    }
}

double getJoyCenter(int axis) {
    switch (axis) {
        case 0: return G_JoyCameraPosition.x + G_JoyViewVector.x;
        case 1: return G_JoyCameraPosition.y + G_JoyViewVector.y;
        case 2: return G_JoyCameraPosition.z + G_JoyViewVector.z;
        default: return 0.0;
    }
}

/* ------- BERECHNUNGEN ------- */

/**
 * Berechnet alle Funktionen, die vom interval abhängig sind
 * @param interval - Zeit
 */
void calcJoyCameraMovement (double interval)
{

    handleHMDEvent();

    Vec3D sideDirection = normVector3D(crossProduct3D(G_JoyViewVector, G_JoyUpVector));

    double maxAngle = 179.0 - angleVectorVector(G_JoyViewVector, G_JoyUpVector); // bei 90° -- +70°
    double minAngle = 1.0 - angleVectorVector(G_JoyViewVector, G_JoyUpVector); // bei 90° -- -70°

    maxAngle = maxAngle < 0 ? -1.0 : maxAngle;
    minAngle = minAngle > 0 ? 1.0 : minAngle;

    //Quaternion q = getQuaternion(sideDirection, G_JoyUpVector);
    Quaternion q = getQuaternion(sideDirection, {.x=0, .y=1, .z=0}, minAngle, maxAngle, interval);
    //Quaternion q = getQuaternion(sideDirection, G_JoyViewVector);

    rotatePointWithQuaternion(q, &G_JoyViewVector);
    //rotatePointWithQuaternion(q, &G_JoyUpVector);

    double forwardTranslation = -getTranslationAxisValue(4) / 500000.0;
    Vec3D forwardVec = normVector3D({.x=G_JoyViewVector.x, .y=0, .z=G_JoyViewVector.z});
    G_JoyCameraTranslation = addVectorVector(G_JoyCameraTranslation, multiplyVectorScalar(forwardVec, forwardTranslation));

    double sideTranslation = getTranslationAxisValue(3) / 500000.0;
    Vec3D sideVec = normVector3D({.x=sideDirection.x, .y=0, .z=sideDirection.z});
    G_JoyCameraTranslation = addVectorVector(G_JoyCameraTranslation, multiplyVectorScalar(sideVec, sideTranslation));

    Vec3D upDirection = {.x=0, .y=1, .z=0};
    double upTranslation = (getTranslationAxisValue(2) + 32768) / 500000.0;
    G_JoyCameraTranslation = addVectorVector(G_JoyCameraTranslation, multiplyVectorScalar(upDirection, upTranslation));

    Vec3D downDirection = {.x=0, .y=-1, .z=0};
    double downTranslation = (getTranslationAxisValue(5) + 32768) / 500000.0;
    G_JoyCameraTranslation = addVectorVector(G_JoyCameraTranslation, multiplyVectorScalar(downDirection, downTranslation));

    //Vec3D basePos = {.x=CAMERA_X, .y=CAMERA_Y, .z=CAMERA_Z};
    G_JoyCameraPosition = G_JoyCameraTranslation;

    //G_JoyUpVector = normVector3D(G_JoyUpVector);
    G_JoyViewVector = normVector3D(G_JoyViewVector);

}

/* ------- INIT ------- */

/**
 * Hier findet die komplette Initialisierung des kompletten SPIEeles statt.
 * Inklusive der Datenhaltung und des SPIEelfeldes.
 */
int initJoyCamera (char* name)
{
    G_JoyUpVector = {.x=0, .y=1, .z=0};
    G_JoyViewVector = {.x=-CAMERA_X, .y=-CAMERA_Y, .z=-CAMERA_Z};
    G_JoyViewVector = normVector3D(G_JoyViewVector);

    G_JoyCameraTranslation = {.x=0, .y=0, .z=0};
    G_JoyCameraPosition = {.x=CAMERA_X, .y=CAMERA_Y, .z=CAMERA_Z};

    if (!initializeHMD(name)) {
        printf("ERROR: hmd could not be initialized.\n");
        return 0;
    }
    return 1;
}


