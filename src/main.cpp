#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	//ofSetupOpenGL(&window, 800, 600, OF_WINDOW);
    ofSetupOpenGL(&window, 800, 600, OF_FULLSCREEN);
   // ofSetupOpenGL(&window, 1024, 576, OF_FULLSCREEN);

	ofRunApp(new testApp());
}
