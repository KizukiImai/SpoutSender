#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(0);

    // �J����������
    auto devices = cams[0].listDevices();
    for (int i = 0; i < NUM_CAM; i++) {
        cams[i].setDeviceID(i);      // �f�o�C�X�ԍ��͊��ɍ��킹�Ē���
        cams[i].setDesiredFrameRate(30);
        cams[i].initGrabber(640, 480);
    }

    // FBO����
    for (int i = 0; i < NUM_FBO; i++) {
        fbos[i].allocate(640, 480, GL_RGBA);
        fbos[i].begin();
        ofClear(0, 0, 0, 255);
        fbos[i].end();
    }

    for (int i = 0; i < NUM_CAM; i++) {
        sender[i].setupAuto("sender" + ofToString(i), fbos[i].getTexture());

    }

    // GUI����
    gui.setup();
    for (int i = 0; i < NUM_FBO; i++) {
        std::string name = "FBO " + std::to_string(i) + " <- Cam";
        assign[i].set(name, i, -1, NUM_CAM - 1); // -1�Ŗ����蓖��
        gui.add(assign[i]);
    }
}

void ofApp::update() {
    // �e�J�����X�V
    for (int i = 0; i < NUM_CAM; i++) {
        cams[i].update();
        if (cams[i].isFrameNew()) {
            camTex[i].loadData(cams[i].getPixels());
        }
    }

    // FBO�ɕ`��
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
    // 2x2 �O���b�h�ɕ��ׂĕ`��
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