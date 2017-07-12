#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "YAMPE/Particle.h"
#include "YAMPE/Printable.h"

class ofApp : public ofBaseApp {

public:
    void setup();
    void update();
    void draw();
    void fire();
    void track();
    float distance(ofVec3f pos1, ofVec3f pos2);
    float randomTargetPosition();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    /** Return the range of a cannon ball for a given elevation. */
    float range(float e);
    
    /** Calculate elevation needed to hit at a given distance. */
    float calculateElevation(float distance);

    // simple 3D world with ground and axes
    const float RANGE = 20;
    ofEasyCam easyCam;
    bool isAxisVisible = true;
    bool isGridVisible = true;
    bool isGroundVisible = true;
    bool isFullScreen = false;
    float cameraHeightRatio;
    ofPlanePrimitive ground;

    // ofxUI code
    void guiEvent(ofxUIEventArgs &e);
    void exit();
    ofxUICanvas *gui;
    ofxUICanvas *guiDebug;
    
private:
    
    bool aim;
    bool fireCannon;
    bool ai;
    
    enum GameState {START, PLAY, FIRED, HIT};
    int gameState;
    vector <string> gameStates;
    
    YAMPE::Particle ball;
        ofVec3f target;
    
    float muzzleSpeed;              ///< magnitude of initial velocity
    ofQuaternion barrelRotation;
    
    float barrelAngleX; // base almost always 0.0
    float barrelAngleY; // base dynamic for horizontal rotation
    float barrelAngleZ; // base dynamic for vertical rotation
    
    ofVec3f gravity;

};
