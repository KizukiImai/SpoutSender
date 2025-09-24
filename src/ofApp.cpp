#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(0);

    // カメラ初期化
    auto devices = cams[0].listDevices();
    for (int i = 0; i < NUM_CAM; i++) {
        cams[i].setDeviceID(i);      // デバイス番号は環境に合わせて調整
        cams[i].setDesiredFrameRate(30);
        cams[i].initGrabber(640, 480);
    }

    // FBO準備
    for (int i = 0; i < NUM_FBO; i++) {
        fbos[i].allocate(640, 480, GL_RGBA);
        fbos[i].begin();
        ofClear(0, 0, 0, 255);
        fbos[i].end();
    }

    for (int i = 0; i < NUM_CAM; i++) {
        sender[i].setupAuto("sender" + ofToString(i), fbos[i].getTexture());

    }

    // GUI準備
    gui.setup();
    for (int i = 0; i < NUM_FBO; i++) {
        std::string name = "FBO " + std::to_string(i) + " <- Cam";
        assign[i].set(name, i, -1, NUM_CAM - 1); // -1で未割り当て
        gui.add(assign[i]);
    }
}

void ofApp::update() {
    // 各カメラ更新
    for (int i = 0; i < NUM_CAM; i++) {
        cams[i].update();
        if (cams[i].isFrameNew()) {
            camTex[i].loadData(cams[i].getPixels());
        }
    }

    // FBOに描画
    for (int i = 0; i < NUM_FBO; i++) {
        fbos[i].begin();
        ofClear(0, 0, 0, 255);

        int camIndex = assign[i];
        if (camIndex >= 0 && camIndex < NUM_CAM && camTex[camIndex].isAllocated()) {
            camTex[camIndex].draw(0, 0, fbos[i].getWidth(), fbos[i].getHeight());
        }
        fbos[i].end();
    }
}

void ofApp::draw() {
    // 2x2 グリッドに並べて描画
    float w = ofGetWidth() / 2.0f;
    float h = ofGetHeight() / 2.0f;

    for (int i = 0; i < NUM_FBO; i++) {
        int x = (i % 2) * w;
        int y = (i / 2) * h;
        fbos[i].draw(x, y, w, h);
    }

    gui.draw();
}

void ofApp::exit() {
    for (int i = 0; i < NUM_CAM; i++) {
        sender[i].release();
    }
}