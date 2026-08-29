#pragma once
#include <JuceHeader.h>
#include <array>

class BoomBapSequencer
{
public:
    void prepare (double sampleRate) { sr = sampleRate; }

    void setTempo (float bpm)
    {
        tempo = bpm;
        samplesPerStep = (60.0 / tempo) * sr / 4.0; // 16th notes
    }

    void setSwing (float s) { swing = juce::jlimit (0.0f, 0.75f, s); }

    void reset() { currentStep = 0; sampleCounter = 0; }

    // Returns true when a new step triggers
    bool process (int numSamples)
    {
        sampleCounter += numSamples;
        double stepLen = samplesPerStep * (currentStep % 2 == 1 ? (1.0 + swing) : (1.0 - swing * 0.5));
        if (sampleCounter >= stepLen)
        {
            sampleCounter = 0;
            currentStep = (currentStep + 1) % 16;
            return pattern[currentStep];
        }
        return false;
    }

    void setStep (int step, bool on) { if (step >= 0 && step < 16) pattern[step] = on; }

    std::array<bool, 16> pattern { true, false, false, false, true, false, true, false,
                                   true, false, false, true, true, false, true, false };

private:
    double sr = 44100.0;
    float tempo = 90.0f;
    double samplesPerStep = 0.0;
    double sampleCounter = 0.0;
    int currentStep = 0;
    float swing = 0.35f; // classic boom-bap swing
};
