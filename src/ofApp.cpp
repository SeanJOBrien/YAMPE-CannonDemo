#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    gameState = START;
    
    easyCam.setDistance(RANGE);
    
    // instantiate the ground
    ground.set(RANGE, RANGE);
    ground.rotate(90, 1,0,0);
    
    // lift camera to 'eye' level
    cameraHeightRatio = 0.2f;
    float d = easyCam.getDistance();
    easyCam.setPosition(0, cameraHeightRatio*d, d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio));
    easyCam.setTarget(ofVec3f::zero());
    
    // specify items on scene
    isGridVisible = false;
    isAxisVisible = true;
    isGroundVisible = true;
    
    // create a minimal gui
    gui = new ofxUICanvas();
    guiDebug = new ofxUICanvas();
    
    gui->addWidgetDown(new ofxUILabel("Controls", OFX_UI_FONT_MEDIUM));
    
    gui->addSpacer();
    gui->addSlider("Barrel Rotation", 0.0, 360, &barrelAngleY);
    gui->addSlider("Barrel Elevation", 0.0, -90.0, &barrelAngleZ);
    gui->addWidgetDown(new ofxUILabelButton("Aim", false, 40, OFX_UI_FONT_SMALL_SIZE));
    gui->addWidgetRight(new ofxUILabelButton("Fire", false, 40, OFX_UI_FONT_SMALL_SIZE));
    gui->addWidgetRight(new ofxUILabelButton("Aim & Fire", false, 100, OFX_UI_FONT_SMALL_SIZE));
    
    gui->addSpacer();
    gui->addWidgetDown(new ofxUILabel("DEBUG", OFX_UI_FONT_MEDIUM));
    static const string arr[] = {"NONE","TEXT","GRAPH"};
    vector<string> vec (arr, arr + sizeof(arr) / sizeof(arr[0]));
    gui->addRadio("Debug", vec, 1, 20, 20);

    gui->addSpacer();
    gui->addLabelButton("Quit", false);
    gui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings("GUI/guiSettings.xml");
    
    guiDebug->setPosition(ofGetWindowWidth()-250 , 0);
    guiDebug->addWidgetDown(new ofxUILabel("DEBUG", OFX_UI_FONT_MEDIUM));
    guiDebug->addSpacer();
    
    
    // constant randomness
    ofSeedRandom();
    
}

//--------------------------------------------------------------
void ofApp::update(){

	float dt = ofGetLastFrameTime();
    //track();
    switch(gameState)
    {
        case START:
        {
            gravity.x = 0.0;
            gravity.y = -0.98;
            gravity.z = 0.0;
            
            muzzleSpeed = 4;
            
            barrelAngleX = 0.0;
            barrelAngleY = 0.0;
            barrelAngleZ = 0.0;
            
            target.x = randomTargetPosition();
            target.z = randomTargetPosition();
            target.y = 0.0f;
            gameState = PLAY;
            
            aim = false;
            fireCannon = false;
            ai = false;
            
            break;
        }
        case PLAY:
        {
            track();
            if(fireCannon)
            {
                fire();
                fireCannon = false;
            }
            break;
        }
            
        case FIRED:
        {
            ball.applyForce(gravity);
            ball.integrate(dt);
            
            // check for ground or target
            if (ball.position.y < 0)
            {
                ball.setPosition(ofVec3f(0.0,0.7,0.0));
                gameState = PLAY;
            }
            if (distance(target,ball.position) < (ball.radius+1.0))
            {
                gameState = HIT;
            }
                
            break;
        }
        case HIT:
        {
            target.x = randomTargetPosition();
            target.z = randomTargetPosition();
            ball.setPosition(ofVec3f(0.0,0.7,0.0));
            gameState = PLAY;
        }
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::fire()
{
    if(gameState == FIRED){
        
    }
    else{
        ball.setPosition(ofVec3f(0.0,0.7,0.0));
    
        ofQuaternion elevationQuate = ofQuaternion(barrelAngleZ, ofVec3f(0,0,1));
        ofQuaternion directionQuate = ofQuaternion(barrelAngleY, ofVec3f(0,1,0));
    
        barrelRotation = elevationQuate*directionQuate;
    
        ofVec3f firingDirection = ofVec3f(0.0,1,0.0);
    
        ball.setVelocity((firingDirection*barrelRotation)*muzzleSpeed);
        gameState = FIRED;
    }
    
    
}
//--------------------------------------------------------------
void ofApp::track()
{
    if(aim || ai){
        float angle = (float)atan2(target.z, target.x)*(180.0/M_PI);
    
        if (angle<0)
        {
            angle = abs(angle);
        }
        else
        {
            angle = 360 - angle;
        }
        if (barrelAngleY < angle)
        {
            if(abs(barrelAngleY-angle) < 5 )
            {
                barrelAngleY = angle;
            }
            else {
                barrelAngleY += 5;
            }
        }
        else if (barrelAngleY > angle)
        {
            barrelAngleY -= 5;
        }
        
        if (barrelAngleY == angle)
        {
            float angleElevation = -calculateElevation(distance(ofVec3f(0.0,0.0,0.0), target));
            if(angleElevation < barrelAngleZ)
            {
                barrelAngleZ -= 5;
            }
            else if(angleElevation > barrelAngleZ)
            {
                if(abs(barrelAngleZ-angleElevation) < 5 )
                {
                    barrelAngleZ = angleElevation;
                    if (ai) {
                        fire();
                    }
                    else{
                        aim = false;
                    }
                }
                else {
                    barrelAngleZ += 5;
                }
            }
        }
    }
    
}
//--------------------------------------------------------------
float ofApp::randomTargetPosition()
{
    float randomNumber = 0;
    while(randomNumber<2.0 && randomNumber > -2.0)
    {
        randomNumber = ofRandom(-9.0, 9.0);
    }
    return randomNumber;
}
//--------------------------------------------------------------
float ofApp::distance(ofVec3f pos1, ofVec3f pos2)
{
    float xd = abs(pos1.x)-abs(pos2.x);
    float yd = abs(pos1.y)-abs(pos2.y);
    float zd = abs(pos1.z)-abs(pos2.z);
    return sqrt(xd*xd + yd*yd + zd*zd);
}
//--------------------------------------------------------------
float ofApp::range(float e) {
    float ux = muzzleSpeed * cos(ofDegToRad(e));
    float uy = muzzleSpeed * sin (ofDegToRad(e ));
    float h = 0.07f;
    float g = -gravity.y;
    return (ux/g) * (uy + sqrt(uy*uy + 2*g*h));
}

//--------------------------------------------------------------
float ofApp:: calculateElevation(float targetDistance) {
    float xMin(0.0f), fxMin(targetDistance-range(xMin));
    float xMax(90.0f), fxMax(targetDistance-range(xMax));
    while (xMax-xMin>0.1e-3) { // loop until close enough
        float x = 0.5*(xMin+xMax);
        float fx = targetDistance -range(x);
        ofLog(OF_LOG_NOTICE, "%f .. %f .. %f => %f .. %f .. %f \n" , xMin, x, xMax, fxMin, fx, fxMax);
        if(fxMin*fx<0) {
            xMax = x; fxMax = fx;
        }else{
            xMin = x; fxMin = fx;
        }
    }
    return 0.5*(xMin+xMax);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableDepthTest();
    ofBackgroundGradient(ofColor(128), ofColor(0), OF_GRADIENT_BAR);
    // Display some useful info
    ofSetColor(255, 0, 0);
    
    
    ofPushStyle();
    easyCam.begin();
    
    //Draw Cannon
    
    ofDrawBox(0.0, 0.2, 0.0, 0.8, 0.4, 0.8);
    ofSetColor(100, 100, 100);
    ofDrawSphere(0.0, 0.7, 0.3);
    
    ofPushMatrix();
    ofTranslate(0.0, 0.7, 0.0);  //Centroid
    ofRotateX(barrelAngleX); // Rotate around the x axis 15 degrees
    ofRotateY(barrelAngleY);
    ofRotateZ(barrelAngleZ);
    // Draw here
    ofDrawCylinder(0.0, 0.7, 0.15, 1.0);
    ofPopMatrix();
    
    if(gameState == FIRED)
    {
        ball.draw();
    }
    
    ofSetColor(0, 255, 255);
    ofDrawSphere(target, 1.0);
    
    if (isGridVisible) {
        ofDrawGrid(10.0f, 5.0f, false, true, true, true);
    } else
        ofDrawGrid(10.0f, 10.0f, false, false, true, false);
    
    if (isAxisVisible) ofDrawAxis(1);
    if (isGroundVisible) {
        ofPushStyle();
        ofSetHexColor(0xB87333);
        ground.draw();
        ofPopStyle();
    }
    
    easyCam.end();
    
    ofPopStyle();
    

}
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    float d = easyCam.getDistance();
    switch (key) {
        case 'h':   // hide/show gui
            gui->toggleVisible();
            break;
        case 'q':   // quit
            ofExit();
            break;
        case 'a':
            isAxisVisible = !isAxisVisible;
            break;
        case 'd':
            isGridVisible = !isGridVisible;
            break;
        case 'g':
            isGroundVisible = !isGroundVisible;
            break;
        case 'l':   // set the ground to be level
            easyCam.setTarget(ofVec3f::zero());
            break;
        case 'z':
            easyCam.setPosition(0, cameraHeightRatio*d,
                                d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio));
            easyCam.setTarget(ofVec3f::zero());
            break;
        case 'Z':
            easyCam.setPosition(0, 0, d);
            easyCam.setTarget(ofVec3f::zero());
            break;
        case 'x':
            easyCam.setPosition(d*sqrt(1.0f-cameraHeightRatio*cameraHeightRatio), cameraHeightRatio*d, 0);
            easyCam.setTarget(ofVec3f::zero());
            break;
        case 'X':
            easyCam.setPosition(d, 0, 0);
            easyCam.setTarget(ofVec3f::zero());
            break;
        case 'Y':
            easyCam.setPosition(0.001, d, 0);
            easyCam.setTarget(ofVec3f::zero());
            break;
        case 'f':
            isFullScreen = !isFullScreen;
            if (isFullScreen) {
                ofSetFullscreen(false);
            } else {
                ofSetFullscreen(true);
            }
            break;
        case OF_KEY_LEFT:
            {
                barrelAngleY += 5;
                if (barrelAngleY > 360) {
                    barrelAngleY -= 360;
                }
                break;
            }
        case OF_KEY_RIGHT:
            {
                barrelAngleY -= 5;
                if (barrelAngleY < 0) {
                    barrelAngleY += 360;
                }
                break;
            }
        case OF_KEY_UP:
            {
                barrelAngleZ += 5;
                if (barrelAngleZ > 90) {
                    barrelAngleZ = 90;
                }
                break;
            }
        case OF_KEY_DOWN:
            {
                barrelAngleZ -= 5;
                if (barrelAngleZ < -90) {
                    barrelAngleZ = -90;
                }
                break;
            }
        case ' ':
            {
                fire();
            }
    }

}

//--------------------------------------------------------------
void ofApp::exit() {
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e) {
    
    string name = e.widget->getName();
    if (name=="Camera Height") {
        // no need to do anything (variable value is automatically updated)
    } else if (name=="Quit") {
        ofExit();
    }
    else if(name == "Aim")
    {
        aim = true;
        ai = false;
    }
    else if(name == "Fire")
    {
        fireCannon = true;
    }
    else if(name == "Aim & Fire")
    {
        ai = true;
    }
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    // easy camera should ignore GUI mouse clicks
    if (gui->getRect()->inside(x,y))
        easyCam.disableMouseInput();
    else
        easyCam.enableMouseInput();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}
void ofApp::mouseMoved(int x, int y) {}
void ofApp::mouseDragged(int x, int y, int button) {}
void ofApp::mouseReleased(int x, int y, int button) {}
void ofApp::windowResized(int w, int h) {}
void ofApp::gotMessage(ofMessage msg) {}
void ofApp::dragEvent(ofDragInfo dragInfo) {}
