#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobAudioProcessorEditor::OneKnobAudioProcessorEditor(OneKnobAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    lookAndFeel = std::make_unique<OneKnobLookAndFeel>();
    setLookAndFeel(lookAndFeel.get());

    // Title
    titleLabel.setText("ONEKNOB", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Main knob
    amountSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    amountSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    amountSlider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(amountSlider);

    amountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "amount", amountSlider);

    // Value label
    valueLabel.setFont(juce::Font(14.0f));
    valueLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
    valueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(valueLabel);

    amountSlider.onValueChange = [this]()
    {
        float val = (float)amountSlider.getValue();
        juce::String text;
        if (std::abs(val) < 1.0f)
            text = "BYPASS";
        else if (val < 0)
            text = "EXPAND " + juce::String(std::abs(val), 0) + "%";
        else
            text = "COMPRESS " + juce::String(val, 0) + "%";
        valueLabel.setText(text, juce::dontSendNotification);
    };
    amountSlider.onValueChange(); // Initialize

    // Bypass button
    bypassButton.setButtonText("Bypass");
    addAndMakeVisible(bypassButton);

    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "bypass", bypassButton);

    // Load background image
    juce::File imageFile("/Users/ianfletcher/oneknob/Source/background.png");
    if (imageFile.existsAsFile())
        backgroundImage = juce::ImageFileFormat::loadFrom(imageFile);

    setSize(280, 340);
}

OneKnobAudioProcessorEditor::~OneKnobAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void OneKnobAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient bgGradient(
        Colors::background, 0, 0,
        Colors::panelBg, 0, (float)getHeight(), false);
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Draw background image if loaded
    if (backgroundImage.isValid())
    {
        auto bounds = getLocalBounds().toFloat();

        // Calculate scaling to cover the area
        float imgW = (float)backgroundImage.getWidth();
        float imgH = (float)backgroundImage.getHeight();
        float scale = juce::jmax(bounds.getWidth() / imgW, bounds.getHeight() / imgH);

        float scaledW = imgW * scale;
        float scaledH = imgH * scale;
        float srcX = (scaledW - bounds.getWidth()) / 2.0f / scale;
        float srcY = (scaledH - bounds.getHeight()) / 2.0f / scale;
        float srcW = bounds.getWidth() / scale;
        float srcH = bounds.getHeight() / scale;

        g.setOpacity(0.25f); // Subtle background
        g.drawImage(backgroundImage,
                    bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                    (int)srcX, (int)srcY, (int)srcW, (int)srcH);
        g.setOpacity(1.0f);

        // Dark overlay for readability
        g.setColour(Colors::background.withAlpha(0.7f));
        g.fillAll();
    }

    // Main panel
    auto panelBounds = getLocalBounds().reduced(12).toFloat();
    g.setColour(Colors::panelBg.withAlpha(0.9f));
    g.fillRoundedRectangle(panelBounds, 12.0f);

    g.setColour(Colors::panelBorder);
    g.drawRoundedRectangle(panelBounds, 12.0f, 1.5f);

    // Subtitle
    g.setColour(Colors::textDim);
    g.setFont(10.0f);
    g.drawText("COMPRESSOR / EXPANDER", panelBounds.removeFromTop(50).translated(0, 35),
               juce::Justification::centred);
}

void OneKnobAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(12);

    // Use FlexBox for centered layout
    juce::FlexBox mainFlex;
    mainFlex.flexDirection = juce::FlexBox::Direction::column;
    mainFlex.justifyContent = juce::FlexBox::JustifyContent::center;
    mainFlex.alignItems = juce::FlexBox::AlignItems::center;

    // Title at top
    int titleHeight = 40;
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));

    // Spacing
    bounds.removeFromTop(10);

    // Main knob - centered
    int knobSize = 180;
    int knobX = (bounds.getWidth() - knobSize) / 2;
    amountSlider.setBounds(bounds.getX() + knobX, bounds.getY(), knobSize, knobSize);
    bounds.removeFromTop(knobSize);

    // Spacing
    bounds.removeFromTop(8);

    // Value label - centered
    int labelHeight = 20;
    valueLabel.setBounds(bounds.removeFromTop(labelHeight));

    // Spacing
    bounds.removeFromTop(12);

    // Bypass button - centered
    int buttonWidth = 80;
    int buttonX = (bounds.getWidth() - buttonWidth) / 2;
    bypassButton.setBounds(bounds.getX() + buttonX, bounds.getY(), buttonWidth, 24);
}
