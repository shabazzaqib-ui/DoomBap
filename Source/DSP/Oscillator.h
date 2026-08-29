#pragma once
#include <JuceHeader.h>
#include <cmath>

class DoomOsc
{
public:
    void prepare (double sampleRate)
    {
        sr = sampleRate;
        phase = 0.0;
    }

    void setFrequency (float freq)
    {
        frequency = freq;
        phaseIncrement = frequency / sr;
    }

    void setWaveform (int type) { waveform = type; } // 0=saw, 1=square, 2=pulse, 3=triangle, 4=noise

    float process()
    {
        float sample = 0.0f;
        switch (waveform)
        {
            case 0: // saw
                sample = 2.0f * phase - 1.0f;
                break;
            case 1: // square
                sample = phase < 0.5f ? 1.0f : -1.0f;
                break;
            case 2: // pulse (variable)
                sample = phase < pulseWidth ? 1.0f : -1.0f;
                break;
            case 3: // triangle
                sample = phase < 0.5f ? (4.0f * phase - 1.0f) : (3.0f - 4.0f * phase);
                break;
            case 4: // noise (gritty)
                sample = noise.nextFloat() * 2.0f - 1.0f;
                break;
        }
        phase += phaseIncrement;
        if (phase >= 1.0) phase -= 1.0;
        return sample;
    }

    void setPulseWidth (float pw) { pulseWidth = juce::jlimit (0.05f, 0.95f, pw); }

private:
    double sr = 44100.0;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    float frequency = 440.0f;
    int waveform = 0;
    float pulseWidth = 0.5f;
    juce::Random noise;
};
