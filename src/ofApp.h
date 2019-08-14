#pragma once

#include <vector>

#include "ofMain.h"
#include "ofxFaceTracker2.h"
#include "ofxSyphon.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

private:
    ofVideoGrabber grabber;
    ofxFaceTracker2 tracker;
    ofxSyphonServer mainOutputSyphonServer;
    ofxSyphonClient mClient;

    ofImage mFaceMask;

    std::vector<bool> mFaceStored;
    std::vector<ofFbo> mFaceFbos;
    std::vector<glm::vec2> mFaceAnchors;
    ofRectangle mTargetBox;

    int mTick = 0;

    const int sNumFaces = 5;
    const float sSize = 150;
    const float sPrimaryFaceScale = 2;
    const float sPerlinScale = 0.001f;
};
