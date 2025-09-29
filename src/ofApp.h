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

    // カメラ（コピー回避のため unique_ptr で保持）
    std::vector<std::unique_ptr<ofVideoGrabber>> cams;
    std::vector<ofTexture> camTex;   // 生映像を入れるテクスチャ

    // FBO（固定）
    ofFbo fbos[NUM_FBO];

    // GUI
    ofxPanel gui;
    ofParameterGroup params;
    std::vector<ofParameter<int>> assign; // 各FBOに割り当てるカメラ番号（-1=未割当）

    // ===== Spout Sender =====
    // カメラの生映像を送信（カメラ台数ぶん）
    std::vector<ofx::spout2::Sender> camSender;

    // FBO内容も送る場合（任意）
    ofx::spout2::Sender fboSender[NUM_FBO];
};
