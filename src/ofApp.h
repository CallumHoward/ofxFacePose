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

    ofTexture mPrimaryFace;
    std::vector<ofTexture> mFaceTextures;
    std::vector<ofRectangle> mFaceBounds;
    std::vector<bool> mFaceStored;
    const int sNumFaces = 6;
};
