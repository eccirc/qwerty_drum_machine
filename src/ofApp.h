#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"



class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    //These are the necessary's for setting up Maxim in OF
    //This is the setup for OF 0.10.0 (See 'Sound' exaples in the examples directory for more detailed setup
            void audioOut(ofSoundBuffer &outBuffer);
            //setup ofSoundStream
            ofSoundStream soundStream;
            //ofSoundBuffer lastBuffer;


    //PUT MAXIM OBJECTS HERE
    maxiOsc kOsc, kOscMod, snareOsc, snareOscMod, timer;

    //envelopes for the kick and snare sounds
    maxiEnv kickEnv, snareEnv;
    //for filtering the sounds
    maxiFilter snareFilter, kickFilter;


    //PUT OTHER STUFF HERE

    //DIY counter (to be used in conjunction with the timer osc)
    int currentCount, lastCount, playHead;

    //we don't want this number to change - this will just be the amount of beats per bar of the drum machine
    #define BEATS 8


    //vector arrays to hold triggers (0 = off, 1 = on)

    //kick drum

    vector<int> kTrigArray;

    int kTrig;

    vector<bool> bkTrig;

    //snare

    vector<int> sTrigArray;

    int sTrig;

    vector<bool> skTrig;


    //Outputs (double for higher res, float for running on limited devices e.g. raspberry pi)

    double kOut, kEnvOut, sOut, sEnvOut, mixOut;

    //visualisation stuff
    int counter;

    double drawKickOutput[512];
    double drawSnareOutput[512];

    float spacing, radius, pos;

    //GUI stuff
    ofxPanel Kgui;
    ofxFloatSlider kickFreq;
    ofxFloatSlider kickModFreq;
    ofxFloatSlider kickModFreqAmt;
    ofxFloatSlider kickA;
    ofxFloatSlider kickD;
    ofxFloatSlider kickS;
    ofxFloatSlider kickR;
    ofxFloatSlider kickCutoff;
    ofxFloatSlider kickVol;

    ofxPanel Sgui;
    ofxFloatSlider snareCutoff;
    ofxFloatSlider snareA;
    ofxFloatSlider snareD;
    ofxFloatSlider snareS;
    ofxFloatSlider snareR;
    ofxFloatSlider snareVol;

    ofxFloatSlider speed;













};
