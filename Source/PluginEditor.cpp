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
            text = "";  // No text at center
            labelColor = Colors::textSecondary;
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

    // Load background image
    juce::File imageFile("/Users/ianfletcher/oneknob/Source/background.png");
    if (imageFile.existsAsFile())
        backgroundImage = juce::ImageFileFormat::loadFrom(imageFile);

    setSize(380, 400);
}

OneKnobAudioProcessorEditor::~OneKnobAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void OneKnobAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto panelBounds = bounds.reduced(4.0f);

    // Dark background for outer edge
    g.fillAll(juce::Colour(0xff0a0a12));

    // Clip to rounded panel for image
    juce::Path clipPath;
    clipPath.addRoundedRectangle(panelBounds, 12.0f);
    g.saveState();
    g.reduceClipRegion(clipPath);

    // Draw background image - FULL visibility, contained in panel
    if (backgroundImage.isValid())
    {
        float imgW = (float)backgroundImage.getWidth();
        float imgH = (float)backgroundImage.getHeight();
        float scale = juce::jmax(panelBounds.getWidth() / imgW, panelBounds.getHeight() / imgH);

        float scaledW = imgW * scale;
        float scaledH = imgH * scale;
        float srcX = (scaledW - panelBounds.getWidth()) / 2.0f / scale;
        float srcY = (scaledH - panelBounds.getHeight()) / 2.0f / scale;
        float srcW = panelBounds.getWidth() / scale;
        float srcH = panelBounds.getHeight() / scale;

        g.setOpacity(1.0f);
        g.drawImage(backgroundImage,
                    panelBounds.getX(), panelBounds.getY(), panelBounds.getWidth(), panelBounds.getHeight(),
                    (int)srcX, (int)srcY, (int)srcW, (int)srcH);
    }

    g.restoreState();

    // Softer dark gradient at top - more gradual fade
    auto topArea = bounds.reduced(4.0f);
    juce::ColourGradient topGradient(
        juce::Colour(0xff000000).withAlpha(0.9f), topArea.getCentreX(), topArea.getY(),
        juce::Colours::transparentBlack, topArea.getCentreX(), topArea.getY() + 120, false);
    g.setGradientFill(topGradient);
    g.fillRoundedRectangle(topArea.removeFromTop(120), 12.0f);

    // Softer dark gradient at bottom - more gradual fade
    auto bottomArea = bounds.reduced(4.0f);
    juce::ColourGradient bottomGradient(
        juce::Colours::transparentBlack, bottomArea.getCentreX(), bottomArea.getBottom() - 100,
        juce::Colour(0xff000000).withAlpha(0.9f), bottomArea.getCentreX(), bottomArea.getBottom(), false);
    g.setGradientFill(bottomGradient);
    g.fillRoundedRectangle(bottomArea.removeFromBottom(100), 12.0f);

    // Colorful border gradient - slightly thicker
    juce::ColourGradient borderGradient(
        Colors::expandColor, bounds.getX(), bounds.getCentreY(),
        Colors::compressColor, bounds.getRight(), bounds.getCentreY(), false);
    g.setGradientFill(borderGradient);
    g.drawRoundedRectangle(bounds.reduced(4.0f), 12.0f, 3.5f);

}

void OneKnobAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(16);

    // Title at top
    int titleHeight = 45;
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));

    // Main knob - give full width for side labels, vertically centered in remaining space
    int knobHeight = 260;
    int remainingHeight = bounds.getHeight();
    int topPad = (remainingHeight - knobHeight - 30) / 2;  // 30 for value label
    bounds.removeFromTop(topPad);
    amountSlider.setBounds(bounds.removeFromTop(knobHeight));

    // Value label - centered below knob
    bounds.removeFromTop(5);
    int labelHeight = 25;
    valueLabel.setBounds(bounds.removeFromTop(labelHeight));
}
