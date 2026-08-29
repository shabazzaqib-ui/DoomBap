#include "PluginProcessor.h"
#include "PluginEditor.h"

DoomBapAudioProcessor::DoomBapAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    presets = getFactoryPresets();

    for (int i = 0; i < 8; ++i)
        synth.addVoice (new SynthVoice());
    synth.addSound (new SynthSound());
}

DoomBapAudioProcessor::~DoomBapAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout DoomBapAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> ("osc1Wave", "Osc1 Wave", 0.0f, 4.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("osc2Wave", "Osc2 Wave", 0.0f, 4.0f, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("oscMix", "Osc Mix", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("subLevel", "Sub Level", 0.0f, 1.0f, 0.5f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("noiseLevel", "Noise", 0.0f, 1.0f, 0.15f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("cutoff", "Cutoff", 50.0f, 12000.0f, 1800.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("resonance", "Resonance", 0.1f, 1.0f, 0.6f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("drive", "Drive", 1.0f, 20.0f, 6.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("attack", "Attack", 0.001f, 2.0f, 0.01f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("decay", "Decay", 0.01f, 2.0f, 0.25f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("sustain", "Sustain", 0.0f, 1.0f, 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("release", "Release", 0.01f, 3.0f, 0.4f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("tempo", "Tempo", 60.0f, 140.0f, 90.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("swing", "Swing", 0.0f, 0.7f, 0.35f));

    return { params.begin(), params.end() };
}

void DoomBapAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
            v->prepareToPlay (sampleRate, samplesPerBlock);

    sequencer.prepare (sampleRate);
    updateVoices();
}

void DoomBapAudioProcessor::releaseResources() {}

bool DoomBapAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void DoomBapAudioProcessor::updateVoices()
{
    auto& p = apvts;
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
        {
            v->updateParameters (
                p.getRawParameterValue ("osc1Wave")->load(),
                p.getRawParameterValue ("osc2Wave")->load(),
                p.getRawParameterValue ("oscMix")->load(),
                p.getRawParameterValue ("subLevel")->load(),
                p.getRawParameterValue ("noiseLevel")->load(),
                p.getRawParameterValue ("cutoff")->load(),
                p.getRawParameterValue ("resonance")->load(),
                p.getRawParameterValue ("drive")->load(),
                p.getRawParameterValue ("attack")->load(),
                p.getRawParameterValue ("decay")->load(),
                p.getRawParameterValue ("sustain")->load(),
                p.getRawParameterValue ("release")->load()
            );
        }
    }
    sequencer.setTempo (p.getRawParameterValue ("tempo")->load());
    sequencer.setSwing (p.getRawParameterValue ("swing")->load());
}

void DoomBapAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();
    updateVoices();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
}

void DoomBapAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= (int) presets.size()) return;
    currentPreset = index;
    auto& pr = presets[index];

    apvts.getParameter ("osc1Wave")->setValueNotifyingHost (pr.osc1Wave / 4.0f);
    apvts.getParameter ("osc2Wave")->setValueNotifyingHost (pr.osc2Wave / 4.0f);
    apvts.getParameter ("oscMix")->setValueNotifyingHost (pr.oscMix);
    apvts.getParameter ("subLevel")->setValueNotifyingHost (pr.subLevel);
    apvts.getParameter ("noiseLevel")->setValueNotifyingHost (pr.noiseLevel);
    apvts.getParameter ("cutoff")->setValueNotifyingHost ((pr.cutoff - 50.0f) / 11950.0f);
    apvts.getParameter ("resonance")->setValueNotifyingHost (pr.resonance);
    apvts.getParameter ("drive")->setValueNotifyingHost ((pr.drive - 1.0f) / 19.0f);
    apvts.getParameter ("attack")->setValueNotifyingHost (pr.attack / 2.0f);
    apvts.getParameter ("decay")->setValueNotifyingHost (pr.decay / 2.0f);
    apvts.getParameter ("sustain")->setValueNotifyingHost (pr.sustain);
    apvts.getParameter ("release")->setValueNotifyingHost (pr.release / 3.0f);
    apvts.getParameter ("tempo")->setValueNotifyingHost ((pr.tempo - 60.0f) / 80.0f);
    apvts.getParameter ("swing")->setValueNotifyingHost (pr.swing / 0.7f);
}

const juce::String DoomBapAudioProcessor::getProgramName (int index)
{
    if (index >= 0 && index < (int) presets.size())
        return presets[index].name;
    return {};
}

void DoomBapAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void DoomBapAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* DoomBapAudioProcessor::createEditor()
{
    return new DoomBapAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DoomBapAudioProcessor();
}
