#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxMidi.h"
#include "Sequencer.h"


using namespace ofxCv;
using namespace cv;

#include "ofxFaceTracker.h"
using namespace FACETRACKER;

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void showFace();
    void setSex(string sex);
	void keyPressed(int key);

    BitMap makeGray;
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ExpressionClassifier classifier;
	
	bool addExpression, addSample, saveData, loadData, debug, screens;
    int screenW, screenH, camW, camH;
    float volume, pitch;
    string performer;
    
    //sound stuff integration
    ofSoundPlayer  smile, sad, neutral;
    ofSoundPlayer  surprise;
    ofSoundPlayer  heart;
    
    
    //midi
    ofxMidiOut midiOut;
    Sequencer sequencer;
    ofImage blood;
};
