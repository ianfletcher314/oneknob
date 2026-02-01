#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"

class OneKnobAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    OneKnobAudioProcessorEditor(OneKnobAudioProcessor&);
    ~OneKnobAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OneKnobAudioProcessor& audioProcessor;

    std::unique_ptr<OneKnobLookAndFeel> lookAndFeel;

    juce::Slider amountSlider;
    juce::Label titleLabel;
    juce::Label valueLabel;
    juce::ToggleButton bypassButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    juce::Image backgroundImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OneKnobAudioProcessorEditor)
};
