/**
 * File:     rController.h
 * Project:  LinWarrior 3D
 * Home:     hackcraft.de
 *
 * Created on March 28, 2008, 21:25 PM (1999)
 *
 * AI and autopilot stacked state machine for controlling entities.
 */

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

struct rController;

//#include "cObject.h"
#include "OID.h"
#include "rComponent.h"
#include "psi3d/math3d.h"

#include <vector>
#include <string>

/**
 * Autopilot/AI object controller, controlls usualy Mechs.
 * Implements an stack-automaton for object behavior control
 * and plan execution.
 */
struct rController : public rComponent {
public:
    /// Available opcodes of stack-machine.
    enum Opcodes {
        /// Play idle until something happens.
        WAIT,
        /// Attack a target.
        ATTACK,
        /// Follow a target (and watchout).
        FOLLOW,
        /// Goto a target (and watchout).
        GOTO,
        /// Repeat stack commands.
        REPEAT,
        /// The Amount of Opcodes.
        OPCODE_MAX
    };

    /// Whether control is enabled (autopilot/ai enabled/disabled).
    bool enabled;
    /// The command stack: n Values may form a command frame (command+arguments).
    std::vector<OID> commandStack;
    /// Who disturbed me the last time - ignore now.
    OID lastDisturbedBy;
public: // Input hooks.
    OID disturbedBy;
    OID enemyNearby;
public: // Output hooks.
    OID targetAim;
    //vec3 targetAim;
    bool targetAimActive;
    bool targetAimFire;
    bool targetGotoAim;
    //OID targetGoto;
    vec3 targetGoto;
    bool targetGotoActive;
    bool idling;
public:
    /// Initialises a en-/disabled controller for the given entity->
    rController(cObject* entity = NULL, bool enable = true);
    /// Destructor.
    virtual ~rController();
    /// Clone this.
    virtual rComponent* clone() { return NULL; }

    /// Do a single Instruction step as the top frame on the stack says.
    virtual void animate(float spf = 1.0f);

    /// Currently there is no hud.
    virtual void drawHUD() {
    };

    /// Print out State and Stackinformation.
    void printState();

protected:
    /// Set output impulses.
    void doit(OID aim, float* go, bool fire);

    /// Get the name of the current stack/command frame.
    std::string getFrameName();

    /// Get the stack frame size of the opcode (command+arguments).
    unsigned int getFrameSizeOf(int opcode);

    /// Get the stack frame size of the current command.
    unsigned int getFrameSize();

    /// Set Parameter value at top of the stack minus offset.
    void setParameter(int offset, OID value);

    /// Get Parameter value from top of the stack minus offset.
    OID getParameter(int offset);

    /// Push a single value onto the stack.
    void push(OID value);

    /// Remove whole instruction/frame from stack.
    void pop();

public:
    // 1. Transition Function
    // 2. State Function

    /// Wait until a Timeout occures or the selected event occures (patrol==true => Enemy Contact).
    void pushWaitEvent(long mseconds = -1, bool patrol = true);
    /// Execute wait command.
    void waitEvent();

    /// Attack that entity.
    void pushAttackEnemy(OID entity);
    /// Execute attack command.
    void attackEnemy();

    /// Follow the given entity and patrol (watchout) if required.
    void pushFollowLeader(OID entity, bool patrol = true);
    /// Execute follow command.
    void followLeader();

    /// Go to the given location and patrol (watchout) if required.
    void pushGotoDestination(float* v, bool patrol = true);
    /// Execute goto command.
    void gotoDestination();

    /// Repeat the n commands below this one (copies/pushes n frames onto stack).
    void pushRepeatInstructions(int n);
    /// Execute repeat command.
    void repeatInstructions();

    // pushRandomWalk();

    // pushSendWave(OID entity, float radius, int mseconds, int attributes);

    // pushForValue(int n, int offset, int start, int end, int step);

    // Use these from controlled Object (controlledDevice):
    // Queries - ask for objects
    // Fuzzy Predicates [0,1] - ask for fulfillment
    // Output Actions - ask for taking action
};


#endif