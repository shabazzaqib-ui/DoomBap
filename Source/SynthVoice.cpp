#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    adsrParams.attack  = 0.01f;
    adsrParams.decay   = 0.25f;
    adsrParams.sustain = 0.3f;
    adsrParams.release = 0.4f;
    adsr.setParameters (adsrParams);
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::prepareToPlay (double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;
    osc1.prepare (sr);
    osc2.prepare (sr);
    subOsc.prepare (sr);
    noiseOsc.prepare (sr);
    filter.prepare (sr);
    adsr.setSampleRate (sr);
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    currentVelocity = velocity;
    float freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    osc1.setFrequency (freq);
    osc2.setFrequency (freq * 1.005f); // slight detune for width
    subOsc.setFrequency (freq * 0.5f);
    noiseOsc.setFrequency (freq);
    adsr.noteOn();
}

void SynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    adsr.noteOff();
    if (!allowTailOff || !adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::updateParameters (float osc1Wave, float osc2Wave, float mix,
                                   float sub, float noise, float cutoff, float reso,
                                   float drive, float a, float d, float s, float r)
{
    osc1.setWaveform ((int) osc1Wave);
    osc2.setWaveform ((int) osc2Wave);
    oscMix = mix;
    subLevel = sub;
    noiseLevel = noise;
    filter.setCutoff (cutoff);
    filter.setResonance (reso);
    distortion.setDrive (drive);

    adsrParams.attack  = a;
    adsrParams.decay   = d;
    adsrParams.sustain = s;
    adsrParams.release = r;
    adsr.setParameters (adsrParams);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive()) return;

    for (int i = 0; i < numSamples; ++i)
    {
        float o1 = osc1.process();
        float o2 = osc2.process();
        float sub = subOsc.process() * subLevel;
        float nse = noiseOsc.process() * noiseLevel;

        float mixed = (o1 * (1.0f - oscMix) + o2 * oscMix) * 0.7f + sub + nse;
        mixed *= currentVelocity;

        mixed = filter.processSample (mixed);
        mixed = distortion.process (mixed);
        mixed *= adsr.getNextSample();

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample + i, mixed * 0.6f);
    }

    if (!adsr.isActive())
        clearCurrentNote();
}
