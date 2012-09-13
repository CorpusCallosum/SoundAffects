#include "testApp.h"

void testApp::setup() {
	ofSetVerticalSync(true);
    //PERFORMER (M or F)
    //N: Neutral
    //pitch of sounds effects is offset
    
    performer = "M";
    setSex(performer);
    fs = false;
    
    //CONTROL for version or style of the audio effects triggered
   // version = 1; //Using expression detection and playing audio samples
    version = 2; //Outputing midi to synthesizer
    
    //is this version for the ITP screens installation?
    screens = false;
    
    
    
    //video stuff
    if(screens){
        screenW = 1024;
        screenH = 576;
        camW = 1024;
        camH = 768;

        
    }
    else{
        screenW = 1280;
        screenH = 800;
        camW = 800;
        camH = 600;
    }
    
    
    cam.setDeviceID(10);
	cam.initGrabber(camW, camH);
   	
	tracker.setup();
	tracker.setRescale(.5);
	
	addExpression = false;
	addSample = false;
	saveData = false;
	loadData = true;//load expression data by default
    debug = false;
    
    //sounds stuff
	smile.setVolume(0.75f);
	smile.setMultiPlay(true);
    
	surprise.setVolume(0.75f);
	surprise.setMultiPlay(true);
   
    sad.loadSound("sounds/aquatic.wav");
    sad.setVolume(0.75f);
	sad.setMultiPlay(true);
    
    neutral.loadSound("sounds/angel.wav");
    neutral.setVolume(0.5f);
	neutral.setMultiPlay(true);
    
    heart.loadSound("sounds/heartbeatFade.wav");
	heart.setVolume(0.25f);
	    
    ofHideCursor();
    
    midiOut.listPorts();
    midiOut.openPort();

}

void testApp::setSex(string sex){
    float pan = .9;
    if(sex == "M"){
        //low pitch
        pitch = .75;
        //load male sounds
        smile.loadSound("sounds/synth.wav");
        surprise.loadSound("sounds/bondi.wav");
        //fade to left
        smile.setPan(-pan);
        surprise.setPan(-pan);
        neutral.setPan(-pan);
        //overall effect volume
        volume = .1;
        
        
    }
    else if(sex == "F"){
        //FEMALE
        //high pitch
        pitch = 1;
        //load female sounds
        smile.loadSound("sounds/bell.wav");
        surprise.loadSound("sounds/fiji.wav");
        smile.setPan(pan);
        surprise.setPan(pan);
        neutral.setPan(pan);
        //overall effect volume
        volume = 1;

    }
    else{
        //low pitch
        pitch = 1;
        //load male sounds
        smile.loadSound("sounds/synth.wav");
        surprise.loadSound("sounds/bondi.wav");
        //overall effect volume
        volume = 1;
    
    }
}

void testApp::update(){
	cam.update();
	if(cam.isFrameNew()) {
		if(tracker.update(toCv(cam))) {
			classifier.classify(tracker);
			if(addExpression) {
                //ADD EXPRESSSION
				addExpression = false;
				classifier.addExpression();
                classifier.addSample(tracker);
                classifier.save("expressions");
			}			
			if(addSample) {
                //ADD SAMPLE
				addSample = false;
				classifier.addSample(tracker);
			}				
			if(loadData) {
				loadData = false;
				classifier.load("expressions");
			}			
			if(saveData) {
				saveData = false;
				classifier.save("expressions");
			}
		}		
	}
}

void testApp::draw() {
	ofSetColor(255);
    	cam.draw(0,0);
    
	int w = 100, h = 12;
	ofPushStyle();
	ofPushMatrix();
	
	int n = classifier.size();
	int primary = classifier.getPrimaryExpression();
    
    if(screens){
        tracker.getImageMesh().drawWireframe();
    }
    
    if(debug){
        tracker.getImageMesh().drawWireframe();
        ofTranslate(5, 10);
        //print debug stuff on screen
        for(int i = 0; i < n; i++){
            ofSetColor(i == primary ? ofColor::red : ofColor::black);
            ofRect(0, 0, w * classifier.getProbability(i) + .5, h);
            ofSetColor(255);
            ofDrawBitmapString(classifier.getDescription(i), 5, 9);
            ofTranslate(0, h + 5);
        }
        ofDrawBitmapString("primary: "+ofToString(primary), 150, 9);
        
        ofDrawBitmapString(ofToString((int) ofGetFrameRate()), ofGetWidth() - 20, ofGetHeight() - 10);
        ofDrawBitmapString(
                           string() +
                           "r - reset\n" +
                           "e - add expression\n" +
                           "a - add sample\n" +
                           "s - save expressions\n"
                           "l - load expressions",
                           14, ofGetHeight() - 6 * 12);
        
        
        ofDrawBitmapString(ofToString(tracker.getGesture(ofxFaceTracker::JAW_OPENNESS)), 100, 20);
        ofDrawBitmapString("note: "+ofToString(note), 50, 30);


    }
    //************************************************************************
    //*******TRIGGER AUDIO***************************************************************
    //************************************************************************
    //"primary" is a number id of the expression
    if(primary){
        String currentExpression = classifier.getDescription(primary);
       // ofDrawBitmapString("currentExpression: "+currentExpression, 100, 18);
        if(currentExpression == "happy"){
            showFace();
            if(version == 1){
                //smile!
                smile.setVolume(classifier.getProbability(primary)*volume*tracker.size());
                float speed = ofMap(tracker.getGesture(ofxFaceTracker::JAW_OPENNESS),20,30,.1,2);
                smile.setSpeed(speed*pitch);
                smile.play();
            }
            
            if(version == 2){
                //turn off previous midi note
                if(note){
                    midiOut.sendNoteOff(1, note, 100);
                }
                
                float ja = tracker.getGesture(ofxFaceTracker::JAW_OPENNESS);
                note = floor(ofMap(ja, 25, 33, 25, 100));
                
                //midi output
                midiOut.sendProgramChange(1,50);
                midiOut.sendNoteOn(1, note, 100);
            }
        
        }
        else if(currentExpression == "surprise"){
            showFace();
            if(version == 1){
                surprise.setVolume(classifier.getProbability(primary)*1*volume);
                float speed = ofMap(tracker.getGesture(ofxFaceTracker::JAW_OPENNESS),20,30,.5,1.5);
                surprise.setSpeed(speed*pitch);
                surprise.play();
            }
            
            

        }
       /* else if(currentExpression == "sad"){
            showFace();
            sad.setVolume(classifier.getProbability(primary)*volume);
            
            sad.setSpeed((tracker.getGesture(ofxFaceTracker::JAW_OPENNESS)/30)*pitch);
            sad.play();
            int note = tracker.getGesture(ofxFaceTracker::JAW_OPENNESS);
            
           // midiOut.sendProgramChange(1,1);
           // midiOut.sendNoteOn(1, note, 100);
        }*/
        else if(currentExpression == "neutral"){
          //  tracker.draw();
            if(version == 1){
                neutral.setVolume(classifier.getProbability(primary)/7*volume);
                neutral.setSpeed((tracker.getGesture(ofxFaceTracker::JAW_OPENNESS)/30)*pitch);
                neutral.play();
            }
        }

    }
    else{
        //no face found
        turnMidiNoteOff();
    }
    
	ofPopMatrix();
	ofPopStyle();
	
    ofBackground(0,0,0);//clear stuff
}

void testApp::turnMidiNoteOff(){
    if(note){
        midiOut.sendNoteOff(1, note, 100);
    }    
}

void testApp::showFace(){
    if(!debug){
        tracker.getImageMesh().drawWireframe();
    }
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		tracker.reset();
		classifier.reset();
	}
	else if(key == 'e') {
		addExpression = true;
	}
	else if(key == 'a') {
		addSample = true;
	}
	else if(key == 's') {
		saveData = true;
	}
	else if(key == 'l') {
		loadData = true;
	}

    else if(key == 'd') {
		debug = !debug;
	}
    
    else if(key == 'c') {
        //camera 
        cam.videoSettings();

	}
    else if(key == 'b') {
        //play the heartbeat 
        heart.play();        
	}
    else if(key == 'f') {
     
        ofSetFullscreen(fs);
        fs = !fs;
        //   setSex("F");
	}
    else if(key == 'm') {
        setSex("M");
	}
    else if(key == 'v') {
        //cahnge between versions 1 and 2
        if(version == 1){
            version = 2;
        }
        else{
            version = 1;
        }
    }

}