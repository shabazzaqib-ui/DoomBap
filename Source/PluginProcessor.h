#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"
#include "SynthSound.h"
#include "DSP/Sequencer.h"
#include "Presets.h"

class DoomBapAudioProcessor : public juce::AudioProcessor
{
public:
    DoomBapAudioProcessor();
    ~DoomBapAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return (int) presets.size(); }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    BoomBapSequencer sequencer;
    std::vector<DoomPreset> presets;
    int currentPreset = 0;

    juce::Synthesiser synth;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateVoices();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DoomBapAudioProcessor)
};
