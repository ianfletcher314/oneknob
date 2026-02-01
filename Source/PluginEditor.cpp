#include "PluginProcessor.h"
#include "PluginEditor.h"

OneKnobAudioProcessorEditor::OneKnobAudioProcessorEditor(OneKnobAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    lookAndFeel = std::make_unique<OneKnobLookAndFeel>();
    setLookAndFeel(lookAndFeel.get());

    // Title
    titleLabel.setText("ONEKNOB", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(32.0f, juce::Font::bold));
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
    valueLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    valueLabel.setColour(juce::Label::textColourId, Colors::accentYellow);
    valueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(valueLabel);

    amountSlider.onValueChange = [this]()
    {
        float val = (float)amountSlider.getValue();
        juce::String text;
        juce::Colour labelColor;

        if (std::abs(val) < 1.0f)
        {
            text = "BYPASS";
            labelColor = Colors::accentYellow;
        }
        else if (val < 0)
        {
            text = "EXPAND " + juce::String(std::abs(val), 0) + "%";
            labelColor = Colors::expandColor;
        }
        else
        {
            text = "COMPRESS " + juce::String(val, 0) + "%";
            labelColor = Colors::compressColor;
        }
        valueLabel.setText(text, juce::dontSendNotification);
        valueLabel.setColour(juce::Label::textColourId, labelColor);
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

    setSize(380, 480);
}

OneKnobAudioProcessorEditor::~OneKnobAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void OneKnobAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Draw background image if loaded - more visible now
    if (backgroundImage.isValid())
    {
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

        g.setOpacity(0.6f);  // More visible background
        g.drawImage(backgroundImage,
                    bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                    (int)srcX, (int)srcY, (int)srcW, (int)srcH);
        g.setOpacity(1.0f);
    }
    else
    {
        // Fallback gradient if no image
        juce::ColourGradient bgGradient(
            Colors::background, 0, 0,
            Colors::panelBg, 0, (float)getHeight(), false);
        g.setGradientFill(bgGradient);
        g.fillAll();
    }

    // Semi-transparent dark overlay for readability
    g.setColour(Colors::background.withAlpha(0.5f));
    g.fillAll();

    // Main panel - more transparent to show background
    auto panelBounds = getLocalBounds().reduced(16).toFloat();
    g.setColour(Colors::panelBg.withAlpha(0.75f));
    g.fillRoundedRectangle(panelBounds, 16.0f);

    // Colorful border gradient
    juce::ColourGradient borderGradient(
        Colors::expandColor, panelBounds.getX(), panelBounds.getCentreY(),
        Colors::compressColor, panelBounds.getRight(), panelBounds.getCentreY(), false);
    g.setGradientFill(borderGradient);
    g.drawRoundedRectangle(panelBounds, 16.0f, 2.0f);

    // Subtitle
    g.setColour(Colors::textSecondary);
    g.setFont(12.0f);
    g.drawText("COMPRESSOR / EXPANDER", panelBounds.removeFromTop(60).translated(0, 45),
               juce::Justification::centred);
}

void OneKnobAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(16);

    // Title at top
    int titleHeight = 50;
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));

    // Spacing
    bounds.removeFromTop(15);

    // Main knob - BIG and centered
    int knobSize = 260;
    int knobX = (bounds.getWidth() - knobSize) / 2;
    amountSlider.setBounds(bounds.getX() + knobX, bounds.getY(), knobSize, knobSize);
    bounds.removeFromTop(knobSize);

    // Spacing
    bounds.removeFromTop(15);

    // Value label - centered
    int labelHeight = 28;
    valueLabel.setBounds(bounds.removeFromTop(labelHeight));

    // Spacing
    bounds.removeFromTop(20);

    // Bypass button - centered
    int buttonWidth = 100;
    int buttonX = (bounds.getWidth() - buttonWidth) / 2;
    bypassButton.setBounds(bounds.getX() + buttonX, bounds.getY(), buttonWidth, 28);
}
