#include <algorithm>
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    const int width = 1280;
    const int height = 720;
    ofSetWindowShape(width, height);

    // Setup grabber
    grabber.setDeviceID(1);
    grabber.setup(width, height);

    // Setup tracker
    tracker.setup();

    // Setup face mask
    //mFaceMask.load("soft_mask01.png");
    //mFaceMask.resize(sSize, sSize);

    // Setup data structure to store faces
    mFaceStored = std::vector<bool>(sNumFaces, false);
    mFaceFbos = std::vector<ofFbo>(sNumFaces);
    mTargetBox = ofRectangle{0, 0, sSize, sSize};
    for (auto& fbo : mFaceFbos) {
        fbo.allocate(mTargetBox.width, mTargetBox.height, GL_RGBA);
        //fbo.getTexture().setAlphaMask(mFaceMask.getTexture());
    }

    // Setup face anchors
    const float stretch = 1.2f;
    mFaceAnchors = std::vector<glm::vec2>({
            glm::vec2{-1.0f * stretch, 0},
            glm::vec2{-0.71f * stretch, 0.71},
            glm::vec2{0, 1},
            glm::vec2{0.71f * stretch, 0.71},
            glm::vec2{1.0f * stretch, 0}});

    // Setup Syphon
    mainOutputSyphonServer.setName("ofxFaceTracker Screen Output");
    mClient.setup();

    ofSetFrameRate(30);
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();

    // Update tracker when there are new frames
    if(grabber.isFrameNew()){
        tracker.update(grabber);
    }

    mTick++;
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

    ofPushStyle();
    ofPushMatrix();

    auto faces = tracker.getInstances();

    // get face with lowest label
    int controlIndex = 0;
    for (int i = 0; i < faces.size(); ++i) {
        if (faces[i].getLabel() < faces[controlIndex].getLabel()) {
            controlIndex = i;
            std::cout << controlIndex << '\n';
        }
    }

    // Iterate over all faces and store them
    for (auto& face : faces) {
        const int index = face.getLabel() % sNumFaces;
        mFaceStored[index] = true;

        // scale bounding box
        ofRectangle boundingBox = face.getBoundingBox();
        boundingBox.scaleFromCenter(2.5f);

        // create fbo
        mFaceFbos[index].begin();
        grabber.getTexture().drawSubsection(mTargetBox, boundingBox);
        mFaceFbos[index].end();
    }

    // draw stored face fbos
    ofPushMatrix();
    ofTranslate((float)ofGetWindowWidth() * 0.5f, (float)ofGetWindowHeight() * 0.7f);
    for (int i = 0; i < sNumFaces; ++i) {
        if (!mFaceStored[i]) { continue; }
        ofPushMatrix();
        const auto offsetX = ofNoise(mFaceAnchors[i].x + mTick * sPerlinScale) - 0.5f;
        const auto offsetY = ofNoise(mFaceAnchors[i].y + mTick * sPerlinScale) - 0.5f;
        const auto scaleOffset = ofMap(ofNoise(mFaceAnchors[i].x + mTick * sPerlinScale), 0, 1, 0.9f, 1.1f);
        ofTranslate(offsetX * sSize * 0.5f, offsetY * sSize * 0.5f);
        ofScale(scaleOffset);

        const auto anchor = -mFaceAnchors[i] * ofGetWindowHeight() * 0.5f;
        mFaceFbos[i].draw(anchor + glm::vec2(-sSize * 0.5f, -sSize * 0.5f));
        ofPopMatrix();
    }
    ofPopMatrix();

    // draw primary face
    if (faces.size() > 0) {
        ofPushMatrix();
        ofTranslate((float)ofGetWindowWidth() * 0.5f, (float)ofGetWindowHeight() * 0.65f);
        ofScale(sPrimaryFaceScale, sPrimaryFaceScale);

        const int controlLabel = faces[controlIndex].getLabel();
        mFaceFbos[controlLabel % sNumFaces].draw(-sSize * 0.5f, -sSize * 0.5f);

        ofPopMatrix();
    }

    mainOutputSyphonServer.publishScreen();

    if (faces.size() > 0) {
        // Apply the pose matrix
        ofPushView();
        faces[controlIndex].loadPoseMatrix();

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

    //mFaceMask.draw(100, 100);

    ofDrawBitmapStringHighlight("Tracker fps: "+ofToString(tracker.getThreadFps()), 10, 20);
}
