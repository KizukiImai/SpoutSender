#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(0);

    // 接続されているカメラを列挙
    auto devices = ofVideoGrabber().listDevices();
    int numDevices = (int)devices.size();

    cams.resize(numDevices);
    camTex.resize(numDevices);
    camSender.resize(numDevices);

    for (int i = 0; i < numDevices; i++) {
        cams[i] = std::make_unique<ofVideoGrabber>();  // コピーを避ける
        cams[i]->setDeviceID(devices[i].id);
        cams[i]->setDesiredFrameRate(30);
        cams[i]->initGrabber(640, 480);

        // grabber と同サイズでテクスチャ確保（RGBでOK）
        camTex[i].allocate(cams[i]->getWidth(), cams[i]->getHeight(), GL_RGB);

        // === カメラ生映像を Spout 送信（自動送信） ===
        camSender[i].setupAuto("Cam_" + ofToString(i), camTex[i]);
    }

    // FBO 準備
    for (int i = 0; i < NUM_FBO; i++) {
        fbos[i].allocate(640, 480, GL_RGBA);
        fbos[i].begin(); ofClear(0, 0, 0, 255); fbos[i].end();

        // === FBO も Spout 送信したい場合は有効化（自動送信） ===
        fboSender[i].setupAuto("FBO_" + ofToString(i), fbos[i].getTexture());
    }

    // GUI 準備
    params.setName("Camera Router");
    assign.resize(NUM_FBO);
    for (int i = 0; i < NUM_FBO; i++) {
        std::string label = "FBO " + std::to_string(i) + " <- Cam";
        assign[i].set(label, (i < numDevices ? i : -1), -1, std::max(numDevices - 1, 0));
        params.add(assign[i]);
    }
    gui.setup(params);
    gui.setPosition(20, 20);
}

void ofApp::update() {
    // 各カメラ更新 → 新フレームなら camTex に転送
    for (size_t i = 0; i < cams.size(); i++) {
        if (!cams[i]) continue;
        cams[i]->update();
        if (cams[i]->isFrameNew()) {
            camTex[i].loadData(cams[i]->getPixels());
            // setupAuto は update リスナーで自動送信するので send() は不要
            // もし必要なら： camSender[i].send(camTex[i]);
        }
    }

    // 各FBOに描画
    for (int i = 0; i < NUM_FBO; i++) {
        fbos[i].begin();
        ofClear(0, 0, 0, 255);

        int camIndex = assign[i];
        if (camIndex >= 0 &&
            camIndex < (int)camTex.size() &&
            camTex[camIndex].isAllocated()) {
            camTex[camIndex].draw(0, 0, fbos[i].getWidth(), fbos[i].getHeight());
        }

        // 任意のデバッグ表示
        ofSetColor(255);
        ofDrawBitmapStringHighlight("FBO" + std::to_string(i) + " <- Cam " + std::to_string(camIndex),
            10, 20);
        ofSetColor(255);

        fbos[i].end();

        // FBO 送信は setupAuto の自動送信に任せる
 fboSender[i].send(fbos[i].getTexture());
    }
}

void ofApp::draw() {
    // 2x2 グリッドでプレビュー
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
    // Spout Sender を解放
    for (auto& s : camSender) s.release();
    for (int i = 0; i < NUM_FBO; i++) fboSender[i].release();
}
