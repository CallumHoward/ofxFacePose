#include <algorithm>
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // Setup grabber
    grabber.setDeviceID(1);
    grabber.setup(1280,720);

    // All examples share data files from example-data, so setting data path to this folder
    // This is only relevant for the example apps
    ofSetDataPathRoot(ofFile(__BASE_FILE__).getEnclosingDirectory()+"../../model/");

    // Setup tracker
    tracker.setup();

    // Setup Syphon
    mainOutputSyphonServer.setName("ofxFaceTracker Screen Output");
    mClient.setup();

    ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
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

    float size = 500;

    ofPushStyle();
    ofPushMatrix();

    auto faces = tracker.getInstances();

    // Iterate over all faces
    for (auto& face : faces) {
        ofRectangle boundingBox = face.getBoundingBox();
        boundingBox.scaleFromCenter(2.5f);

        const auto targetBox = ofRectangle{0, 50, size, size};
        grabber.getTexture().drawSubsection(targetBox, boundingBox);

        //ofDrawBitmapStringHighlight("Label: " + ofToString(face.getLabel()), 10, 45);

        ofTranslate(size, 0, 0);
    }

    if (faces.size() > 0) {
        // get face with lowest label
        int controlLabel = 0;
        for (int i = 1; i < faces.size(); ++i) {
            if (faces[i].getLabel() < faces[controlLabel].getLabel()) {
                controlLabel = i;
            }
        }

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
