#include "ofApp.h"
#include <algorithm>

bool ofApp::isVirtualOrUnsupported(const std::string& name) const {
    std::string s = name;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    static const std::vector<std::string> bad = {
        "obs", "virtual", "manycam", "snap", "xsplit", "vcam"
    };
    for (const auto& k : bad) {
        if (s.find(k) != std::string::npos) return true;
    }
    return false;
}

void ofApp::setup() {
    ofSetFrameRate(60);
    ofBackground(0);
    ofDisableArbTex();

    auto devices = ofVideoGrabber().listDevices();
    ofLogNotice() << "===== Devices =====";
    for (auto& d : devices) ofLogNotice() << d.id << ": " << d.deviceName;

    cams.clear();
    camRawTex.clear();
    camFbo1080.clear();

    int camCount = 0;
    for (auto& d : devices) {
        if (isVirtualOrUnsupported(d.deviceName)) {
            ofLogNotice() << "[Skip] " << d.deviceName;
            continue;
        }
        if (camCount >= MAX_CAM) break;

        auto cam = std::make_unique<ofVideoGrabber>();
        cam->setDeviceID(d.id);
        cam->setDesiredFrameRate(30);
        cam->initGrabber(CAM_OUT_W, CAM_OUT_H);

        int rw = cam->getWidth();
        int rh = cam->getHeight();
        if (rw <= 0 || rh <= 0) { rw = CAM_OUT_W; rh = CAM_OUT_H; }

        ofTexture raw;
        raw.allocate(rw, rh, GL_RGBA);
        camRawTex.push_back(std::move(raw));

        ofFbo f1080;
        f1080.allocate(CAM_OUT_W, CAM_OUT_H, GL_RGBA8);
        f1080.begin(); ofClear(0, 0, 0, 255); f1080.end();
        camFbo1080.push_back(std::move(f1080));

        // ★ setupAuto を配列に対して直接呼ぶ
        camSender[camCount].setupAuto("Cam_" + ofToString(camCount),
            camFbo1080.back().getTexture());

        cams.push_back(std::move(cam));
        camCount++;
    }

    for (int i = 0; i < NUM_FBO; ++i) {
        fbos[i].allocate(PREV_W, PREV_H, GL_RGBA8);
        fbos[i].begin(); ofClear(0, 0, 0, 255); fbos[i].end();
    }

    params.setName("Camera Router");
    assign.resize(NUM_FBO);
    for (int i = 0; i < NUM_FBO; ++i) {
        assign[i].set("FBO " + std::to_string(i),
            (i < (int)cams.size() ? i : -1),
            -1, std::max((int)cams.size() - 1, 0));
        params.add(assign[i]);
    }
    gui.setup(params);
    gui.setPosition(20, 20);
}

void ofApp::update() {
    for (size_t i = 0; i < cams.size(); ++i) {
        auto& grab = *cams[i];
        grab.update();
        if (grab.isFrameNew()) {
            camRawTex[i].loadData(grab.getPixels());
            camFbo1080[i].begin();
            ofClear(0, 0, 0, 255);
            camRawTex[i].draw(0, 0, CAM_OUT_W, CAM_OUT_H);
            camFbo1080[i].end();
        }
        // ★ setupAuto の場合 send() は不要。内部で ofEvents().update にフック
    }

    for (int fi = 0; fi < NUM_FBO; ++fi) {
        fbos[fi].begin();
        ofClear(0, 0, 0, 255);
        int camIndex = assign[fi];
        if (camIndex >= 0 && camIndex < (int)camFbo1080.size()) {
            camFbo1080[camIndex].draw(0, 0, PREV_W, PREV_H);
        }
        fbos[fi].end();
    }
}

void ofApp::draw() {
    float w = ofGetWidth() / 2.0f;
    float h = ofGetHeight() / 2.0f;
    for (int i = 0; i < NUM_FBO; ++i) {
        fbos[i].draw((i % 2) * w, (i / 2) * h, w, h);
    }
    gui.draw();
}

void ofApp::exit() {
    for (int i = 0; i < MAX_CAM; i++) {
        camSender[i].release();
    }
}
