#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	vid.setup(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	float r = 100;
	float cx = ofGetWidth() * 0.5;
	float cy = ofGetHeight() * 0.5;
	float t = ofGetElapsedTimef();
	float x = cx + cos(t*1.3) * r;
	float y = cy + sin(t*1.3) * r;
	
	glColor3f(1,1,0);
	ofCircle( x, y, 10);
	
	vid.addFrame();
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}