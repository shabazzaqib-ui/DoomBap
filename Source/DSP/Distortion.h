#pragma once
#include <JuceHeader.h>
#include <cmath>

class DoomDistortion
{
public:
    void setDrive (float d) { drive = juce::jlimit (1.0f, 25.0f, d); }
    void setMix (float m) { mix = juce::jlimit (0.0f, 1.0f, m); }

    float process (float input)
    {
        float wet = std::tanh (input * drive);          // soft clip
        wet = juce::jlimit (-1.0f, 1.0f, wet * 1.4f);   // extra grit
        return input * (1.0f - mix) + wet * mix;
    }

private:
    float drive = 4.0f;
    float mix = 0.6f;
};
