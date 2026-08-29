#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"
#include "DSP/Oscillator.h"
#include "DSP/Filter.h"
#include "DSP/Distortion.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}
    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;

    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void updateParameters (float osc1Wave, float osc2Wave, float oscMix, float subLevel,
                           float noiseLevel, float cutoff, float reso, float drive,
                           float attack, float decay, float sustain, float release);

private:
    DoomOsc osc1, osc2, subOsc, noiseOsc;
    DoomFilter filter;
    DoomDistortion distortion;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    float currentVelocity = 0.0f;
    float oscMix = 0.5f;
    float subLevel = 0.4f;
    float noiseLevel = 0.15f;
    double sampleRate = 44100.0;
};
