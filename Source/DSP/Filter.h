#pragma once
#include <JuceHeader.h>

class DoomFilter
{
public:
    void prepare (double sampleRate)
    {
        filter.prepare ({ sampleRate, 512, 2 });
        filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    }

    void setCutoff (float hz) { filter.setCutoffFrequency (hz); }
    void setResonance (float q) { filter.setResonance (q); }

    void process (juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        filter.process (ctx);
    }

    float processSample (float sample)
    {
        return filter.processSample (0, sample);
    }

private:
    juce::dsp::StateVariableTPTFilter<float> filter;
};
