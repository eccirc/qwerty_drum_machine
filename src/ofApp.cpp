/*Basic drum machine with keyboard input and some simple sound visualisation
 * Inspired by and credit to Mick Grierson's AAVP class and old-school drum machines!
 * https://github.com/micknoise/Maximilian
 *
 * David Williams January 2019
 */


#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(0);

    //initialse boolean array (default = false) and the default sound triggers to zero

    for(int i = 0; i < BEATS; i ++){
        bkTrig.push_back(false);
        kTrigArray.push_back(0);
        skTrig.push_back(false);
        sTrigArray.push_back(0);

    }

    //GUI setup
    Kgui.setup("kick drum settings","KdSettings",0,0);
    Kgui.add(kickFreq.setup("kick frequency", 40, 20, 100));
    Kgui.add(kickModFreq.setup("kick mod osc frequency",40,0,100));
    Kgui.add(kickModFreqAmt.setup("kick mod osc amount",0,0,1000));
    Kgui.add(kickA.setup("kick attack",0,0,1000));
    Kgui.add(kickD.setup("kick decay",1,0,1));
    Kgui.add(kickS.setup("kick sustain",1,0,1));
    Kgui.add(kickR.setup("kick release",1000,0,2000));
    Kgui.add(kickCutoff.setup("kick cutoff",0.5,0,1));
    Kgui.add(kickVol.setup("kick volume",0.5,0,1));

    Sgui.setup("snare drum settings","SdSettings",ofGetWidth()/2,0);
    Sgui.add(snareCutoff.setup("cutoff",0.1,0,1));
    Sgui.add(snareA.setup("snare attack",0,0,1000));
    Sgui.add(snareD.setup("snare decay",1,0,1));
    Sgui.add(snareS.setup("snare sustain",1,0,1));
    Sgui.add(snareR.setup("snare release",100,0,2000));
    Sgui.add(snareVol.setup("snare volume",0.5,0,1));
    Sgui.add(speed.setup("global tempo",4,0.5,16));


    //This is the necessary audio setup stuff for running Maxim with OF

    //Get and print a list of possible output devices

    auto devices = soundStream.getDeviceList();

    //cout << devices << endl;


    //This is the working setup for OF 0.10.0
    ofSoundStreamSettings settings;

    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.sampleRate = 44100;
    settings.bufferSize = 512;
    settings.numBuffers = 2;
    settings.setOutListener(this);
    settings.setOutDevice(devices[5]);
    soundStream.setup(settings);



}
//--------------------------------------------------------------
void ofApp::update(){


    //set envelope values
    kickEnv.setAttack(kickA);
    kickEnv.setDecay(kickD);
    kickEnv.setSustain(kickS);
    kickEnv.setRelease(kickR);

    snareEnv.setAttack(snareA);
    snareEnv.setDecay(snareD);
    snareEnv.setSustain(snareS);
    snareEnv.setRelease(snareR);

    //update the triggers as per the boolean array
    for(int i = 0; i < BEATS; i ++){
        if(bkTrig[i] ==true){
            kTrigArray[i] = 1;
        }
        else if (bkTrig[i] == false) {
            kTrigArray[i] = 0;
        }
    }
        for(int i = 0; i < BEATS; i ++){
            if(skTrig[i] ==true){
                sTrigArray[i] = 1;
            }
            else if (skTrig[i] == false) {
                sTrigArray[i] = 0;
            }
    }


}

//--------------------------------------------------------------
void ofApp::draw(){


    int rad = ofGetWidth() / BEATS;
    ofColor c1, c2;

    //Kick section - black if on, white if off ( same for snare )
    ofPushStyle();
    for(int i = 0; i < BEATS; i ++){
        if(bkTrig[i]){
            c1 = ofColor::black;
        }
        else{
            c1 = ofColor::white;
        }
        ofSetColor(c1);
        ofDrawRectangle(i*rad,ofGetHeight() - rad,rad-1,rad-1);
    }
    ofPopStyle();

    //Snare section
    ofPushStyle();
    for(int i = 0; i < BEATS; i ++){
        if(skTrig[i]){
            c2 = ofColor::black;
        }
        else{
            c2 = ofColor::white;
        }
        ofSetColor(c2);

        ofDrawRectangle(i*rad, ofGetHeight() - rad*2, rad-1, rad-1);
    }
    ofPopStyle();

    //draw gui
    Kgui.draw();

    Sgui.draw();

    //draw radial visualisations
    spacing = TWO_PI / 512;
    radius = rad*2;


    for(int i = 0; i < 512; i ++){

    //kick
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(rad*2, rad*3);
    ofSetColor(255);
    ofDrawLine(0, 0,(cos(i * spacing)) * radius * drawKickOutput[i],(sin(i * spacing)) * radius * drawKickOutput[i]);
    ofPopMatrix();

    //snare
    ofPushMatrix();
    ofTranslate(rad*6, rad*3);
    ofSetColor(0);
    ofSetColor(255);
    ofDrawLine(0, 0,(cos(i * spacing)) * radius * drawSnareOutput[i],(sin(i * spacing)) * radius * drawSnareOutput[i]);
    ofPopMatrix();
    ofPopStyle();
    }


}
void ofApp::audioOut(ofSoundBuffer &outBuffer){


    //Put all sound related code within this for loop (This setup for OF 0.10.0)
    for(size_t i = 0; i < outBuffer.getNumFrames(); i ++){

        //use phasor to count - change tempo with slider
        currentCount = int(timer.phasor(speed));


        //envelope triggers
        kEnvOut = kickEnv.adsr(1.0, kickEnv.trigger);
        sEnvOut = snareEnv.adsr(1.0, snareEnv.trigger);
        //thanks to Mick Griersons drum machine example - this method works as a
        //reliable counter
        if(lastCount != currentCount){
            kTrig = kTrigArray[playHead % BEATS];
            sTrig = sTrigArray[playHead % BEATS];
            playHead ++;
            lastCount = 0;

        }
        //sample triggers 0 = off, 1 = on
        kickEnv.trigger = kTrig;
        snareEnv.trigger = sTrig;

        //put both outputs through a low-pass filter to tailor the sound

        //kick works well with a sinewave
        kOut = kickFilter.lopass(kOsc.sinewave(kickFreq * kEnvOut + (kOscMod.sinewave(kickModFreq)*kickModFreqAmt)) * kEnvOut, kickCutoff);

        //simple snare sound made with white noise
        sOut = snareFilter.lopass(snareOsc.noise() * sEnvOut, snareCutoff);

        mixOut = (kOut * kickVol) + (sOut * snareVol);

        //These are the audio stereo outs
        outBuffer.getSample(i, 0) = mixOut;
        outBuffer.getSample(i, 1) = mixOut;

        //rest to 0 in order to retrigger the envelope every beat
        kTrig = 0;
        sTrig = 0;

        //get basic values from the output to draw visualisations with
        drawKickOutput[i] = kOut;
        drawSnareOutput[i] = sOut;




    }

}
void ofApp::keyPressed(int key){
//kick section corresponds to the 'asdfg...' section of the keyboard
//snare the 'qwerty...' section


    switch(key){
        //kick trigs
        case 'a':
        bkTrig[0] = !bkTrig[0];
        break;
        case 's':
        bkTrig[1] = !bkTrig[1];
        break;
        case 'd':
        bkTrig[2] = !bkTrig[2];
        break;
        case 'f':
        bkTrig[3] = !bkTrig[3];
        break;
        case 'g':
        bkTrig[4] = !bkTrig[4];
        break;
        case 'h':
        bkTrig[5] = !bkTrig[5];
        break;
        case 'j':
        bkTrig[6] = !bkTrig[6];
        break;
        case 'k':
        bkTrig[7] = !bkTrig[7];
        break;
        //snare trigs
        case 'q':
        skTrig[0] = !skTrig[0];
        break;
        case 'w':
        skTrig[1] = !skTrig[1];
        break;
        case 'e':
        skTrig[2] = !skTrig[2];
        break;
        case 'r':
        skTrig[3] = !skTrig[3];
        break;
        case 't':
        skTrig[4] = !skTrig[4];
        break;
        case 'y':
        skTrig[5] = !skTrig[5];
        break;
        case 'u':
        skTrig[6] = !skTrig[6];
        break;
        case 'i':
        skTrig[7] = !skTrig[7];
        break;
    }

}
