#include <algorithm>
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Setup grabber
    grabber.setDeviceID(1);
    grabber.setup(1280, 720);

    // All examples share data files from example-data, so setting data path to this folder
    // This is only relevant for the example apps
    ofSetDataPathRoot(ofFile(__BASE_FILE__).getEnclosingDirectory() + "../../model/");

    // Setup tracker
    tracker.setup();

    // Setup data structure to store faces
    mFaceTextures = std::vector<ofTexture>(sNumFaces);
    mFaceBounds = std::vector<ofRectangle>(sNumFaces);
    mFaceStored = std::vector<bool>(sNumFaces, false);

    // Setup Syphon
    mainOutputSyphonServer.setName("ofxFaceTracker Screen Output");
    mClient.setup();

    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();

    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        tracker.update(grabber);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw camera image
    //grabber.draw(0,0);

    // Draw debug pose
    //tracker.drawDebugPose();

    float size = 100;

    ofPushStyle();
    ofPushMatrix();

    auto faces = tracker.getInstances();

    // get face with lowest label
    int controlLabel = 0;
    for (int i = 1; i < faces.size(); ++i) {
        if (faces[i].getLabel() < faces[controlLabel].getLabel()) {
            controlLabel = i;
        }
    }

    // Iterate over all faces and store them
    for (auto& face : faces) {
        const int index = face.getLabel() % sNumFaces;
        mFaceTextures[index] = grabber.getTexture();
        mFaceBounds[index] = face.getBoundingBox();
        mFaceBounds[index].scaleFromCenter(2.5f);
        mFaceStored[index] = true;
    }

    // draw stored faces
    for (int i = 0; i < sNumFaces; ++i) {
        if (mFaceStored[i]) {
            const auto targetBox = ofRectangle{0, 50, size, size};
            mFaceTextures[i].drawSubsection(targetBox, mFaceBounds[i]);
        }
        ofTranslate(size, 0, 0);
    }

    if (faces.size() > 0) {
        // Apply the pose matrix
        ofPushView();
        faces[controlLabel].loadPoseMatrix();

        // Now position 0,0,0 is at the forehead
        ofSetColor(255,0,0,50);
        ofDrawRectangle(0, 0, 200, 200);

        ofPushMatrix();
        ofSetColor(0,255,0,50);
        ofRotateXDeg(-90);
        ofDrawRectangle(0, 0, 200, 200);
        ofPopMatrix();

        ofPushMatrix();
        ofSetColor(0,0,255,50);
        ofRotateXDeg(90);
        ofDrawRectangle(0, 0, 200, 200);
        ofPopMatrix();

        ofPopView();
    }

    ofPopStyle();
    ofPopMatrix();

    mainOutputSyphonServer.publishScreen();

    ofDrawBitmapStringHighlight("Tracker fps: "+ofToString(tracker.getThreadFps()), 10, 20);
}
