#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobAudioProcessor::OneKnobAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

OneKnobAudioProcessor::~OneKnobAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OneKnobAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Main knob: -100 (full expand) to +100 (full compress), 0 = bypass
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("amount", 1),
        "Amount",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Bypass
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("bypass", 1),
        "Bypass",
        false));

    return { params.begin(), params.end() };
}

const juce::String OneKnobAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneKnobAudioProcessor::acceptsMidi() const { return false; }
bool OneKnobAudioProcessor::producesMidi() const { return false; }
bool OneKnobAudioProcessor::isMidiEffect() const { return false; }
double OneKnobAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int OneKnobAudioProcessor::getNumPrograms() { return 1; }
int OneKnobAudioProcessor::getCurrentProgram() { return 0; }
void OneKnobAudioProcessor::setCurrentProgram(int) {}
const juce::String OneKnobAudioProcessor::getProgramName(int) { return {}; }
void OneKnobAudioProcessor::changeProgramName(int, const juce::String&) {}

void OneKnobAudioProcessor::prepareToPlay(double sampleRate, int)
{
    dynamicsProcessor.prepare(sampleRate);
}

void OneKnobAudioProcessor::releaseResources()
{
}

bool OneKnobAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void OneKnobAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    bool bypassed = apvts.getRawParameterValue("bypass")->load() > 0.5f;
    if (bypassed)
        return;

    float amount = apvts.getRawParameterValue("amount")->load() / 100.0f; // Normalize to -1 to +1
    dynamicsProcessor.setAmount(amount);
    dynamicsProcessor.process(buffer);
}

bool OneKnobAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* OneKnobAudioProcessor::createEditor()
{
    return new OneKnobAudioProcessorEditor(*this);
}

void OneKnobAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OneKnobAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneKnobAudioProcessor();
}
