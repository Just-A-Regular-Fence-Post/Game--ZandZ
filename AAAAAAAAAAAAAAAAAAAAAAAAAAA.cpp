#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <ctime>

enum BossState {
    FIREBREATH,
    SPAWNITONYASSPROJECTILE,
    BRUTALCOUNTER,
    REALLYSTRONGASSPUNCH,
    NOLIZARDSALLOWED,
    MORPH,
    FROSTBREATH,
    AVALANCHE,
    SPAWNITONYOASSPROJECTILEV2,
    FROSTPUNCH,
    FISSURE,
    CANTESCAPETHISHAMMER,
    THEWHEELOFMISFORTUNE,
    NEVERGONNALETYOUDOWN,
    CASTTHEDIEOFFATE,
    WHATSINTHECARDS,

    RETREAT,
    CHASE,
    DASH,
    IDLE,

}

class Boss {
    // Boss properties
    BossState state = IDLE;
    float health = 1000;
    float cd = 0;
    


    //


    // Boss state cases
    BossState state;
    void update() {
        switch (state) {
            case FIREBREATH:
                // Implement FIREBREATH behavior
                break;
            case SPAWNITONYASSPROJECTILE:
                // Implement SPAWNITONYASSPROJECTILE behavior
                break;
            case BRUTALCOUNTER:
                // Implement BRUTALCOUNTER behavior
                break;
            case REALLYSTRONGASSPUNCH:
                // Implement REALLYSTRONGASSPUNCH behavior
                break;
            case NOLIZARDSALLOWED:
                // Implement NOLIZARDSALLOWED behavior
                break;
            case MORPH:
                // Implement MORPH behavior
                break;
            case FROSTBREATH:
                // Implement FROSTBREATH behavior
                break;
            case AVALANCHE:
                // Implement AVALANCHE behavior
                break;
            case SPAWNITONYOASSPROJECTILEV2:
                // Implement SPAWNITONYOASSPROJECTILEV2 behavior
                break;
            case FROSTPUNCH:
                // Implement FROSTPUNCH behavior
                break;
            case FISSURE:
                // Implement FISSURE behavior
                break;
            case CANTESCAPETHISHAMMER:
                // Implement CANTESCAPETHISHAMMER behavior
                break;
            case THEWHEELOFMISFORTUNE:
                // Implement THEWHEELOFMISFORTUNE behavior
                break;
            case NEVERGONNALETYOUDOWN:
                // Implement NEVERGONNALETYOUDOWN behavior
                break;
            case CASTTHEDIEOFFATE:
                // Implement CASTTHEDIEOFFATE behavior
                break;
            case WHATSINTHECARDS:
                // Implement WHATSINTHECARDS behavior
                break;
        }
    }




}