cmake_minimum_required(VERSION 3.22)
project(DoomBap VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Point this to your JUCE path
add_subdirectory(/path/Daniel/desktop/JUCE JUCE)

juce_add_plugin(DoomBap
    COMPANY_NAME "DoomLabs"
    PLUGIN_MANUFACTURER_CODE Doom
    PLUGIN_CODE Dbap
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "DoomBap"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(DoomBap PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/SynthVoice.cpp
)

target_compile_definitions(DoomBap PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(DoomBap PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
