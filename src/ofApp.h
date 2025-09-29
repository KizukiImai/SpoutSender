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

    static const int NUM_FBO = 4;     // �v���r���[�pFBO
    static const int MAX_CAM = 8;     // �ő�J������
    static const int CAM_OUT_W = 1920;  // ���M�𑜓x
    static const int CAM_OUT_H = 1080;
    static const int PREV_W = 640;   // �v���r���[�𑜓x
    static const int PREV_H = 360;

private:
    bool isVirtualOrUnsupported(const std::string& name) const;

    std::vector<std::unique_ptr<ofVideoGrabber>> cams;
    std::vector<ofTexture> camRawTex;
    std::vector<ofFbo> camFbo1080;

    ofFbo fbos[NUM_FBO];

    ofxPanel gui;
    ofParameterGroup params;
    std::vector<ofParameter<int>> assign;

    // �� �Œ蒷�z��Ƃ��ĊǗ�
    ofx::spout2::Sender camSender[MAX_CAM];
};
