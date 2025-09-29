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

    static const int NUM_FBO = 4;

    // �J�����i�R�s�[����̂��� unique_ptr �ŕێ��j
    std::vector<std::unique_ptr<ofVideoGrabber>> cams;
    std::vector<ofTexture> camTex;   // ���f��������e�N�X�`��

    // FBO�i�Œ�j
    ofFbo fbos[NUM_FBO];

    // GUI
    ofxPanel gui;
    ofParameterGroup params;
    std::vector<ofParameter<int>> assign; // �eFBO�Ɋ��蓖�Ă�J�����ԍ��i-1=�������j

    // ===== Spout Sender =====
    // �J�����̐��f���𑗐M�i�J�����䐔�Ԃ�j
    std::vector<ofx::spout2::Sender> camSender;

    // FBO���e������ꍇ�i�C�Ӂj
    ofx::spout2::Sender fboSender[NUM_FBO];
};
