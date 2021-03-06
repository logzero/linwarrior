/**
 * File:     cBackground.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 *
 * Virtual Horizon with Sky and Ground
 * (SkySphere / SkyBox).
 */

#ifndef _CBACKGROUNDSYSTEM_H
#define _CBACKGROUNDSYSTEM_H

class BackgroundSystem;

#include <map>
#include <list>

#include "de/hackcraft/psi3d/Particle.h"

#include "de/hackcraft/world/Propmap.h"
#include "de/hackcraft/world/Subsystem.h"

/**
 * A background in form of a SkySphere just like
 * those virtual horizons found in aviation instruments.
 * Sets lightsource 0 as skylight according to time.
 */
class BackgroundSystem : public Subsystem {
private:
    static std::map<int, unsigned int> textures;
private:
    float hour;
    std::list<Particle*> rain;
    int seed;
    /** Total amount of rain drops at a time. */
    int raininess;
    /** Zero to negative shift (upward) makes horizontal curvature visible. */
    float heightshift;
    /** Relative amount of clouds [0,1]. */
    float cloudiness;
    /** Windspeed for cloud movement per (scaled) time. */
    float windspeed;
    /** Light direction according to celestial bodies and time (sun/moon). */
    vec4 light;
public:
    BackgroundSystem();
    BackgroundSystem(Propmap* properties);
    void init(Propmap* properties);
    void initTextures();
public:
    void advanceTime(int deltamsec);
    bool drawBack(); // Calls other draw*()
private:
    void drawGalaxy();
    void drawUpperDome();
    void drawLowerDome();
    void drawGround();
    void drawClouds();
    //void drawMountains();
    void drawSun();
    void drawOrbit();
    void drawRain();
};

#endif

