#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSpout2Sender.h"
class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void exit() override;
    static const int NUM_CAM = 4;
    static const int NUM_FBO = 4;

    ofVideoGrabber cams[NUM_CAM];
    ofFbo fbos[NUM_FBO];
    ofTexture camTex[NUM_CAM];

    // GUI
    ofxPanel gui;
    ofParameter<int> assign[NUM_FBO]; // FBO‚ÉŠ„‚è“–‚Ä‚éƒJƒƒ‰”Ô†
    ofTexture tex[NUM_CAM];
    ofx::spout2::Sender sender[NUM_CAM];

    
};
