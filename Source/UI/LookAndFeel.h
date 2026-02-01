#pragma once

#include <JuceHeader.h>

namespace Colors
{
    // Colorful samba-inspired palette
    const juce::Colour background     = juce::Colour(0xff1a1a2e);  // Deep navy
    const juce::Colour panelBg        = juce::Colour(0xff16213e);  // Dark blue
    const juce::Colour panelBorder    = juce::Colour(0xff0f3460);  // Blue border
    const juce::Colour accent         = juce::Colour(0xffe94560);  // Vibrant pink/red
    const juce::Colour accentYellow   = juce::Colour(0xffffc107);  // Samba gold
    const juce::Colour accentGreen    = juce::Colour(0xff00b894);  // Tropical green
    const juce::Colour accentOrange   = juce::Colour(0xffff6b35);  // Carnival orange
    const juce::Colour textPrimary    = juce::Colour(0xfff0f0f0);
    const juce::Colour textSecondary  = juce::Colour(0xffaaaaaa);
    const juce::Colour textDim        = juce::Colour(0xff666666);
    const juce::Colour expandColor    = juce::Colour(0xff00b894);  // Green for expansion
    const juce::Colour compressColor  = juce::Colour(0xffe94560);  // Pink for compression
}

class OneKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OneKnobLookAndFeel()
    {
        setColour(juce::Slider::textBoxTextColourId, Colors::textPrimary);
        setColour(juce::Slider::textBoxBackgroundColourId, Colors::panelBg);
        setColour(juce::Slider::textBoxOutlineColourId, Colors::panelBorder);
        setColour(juce::Label::textColourId, Colors::textPrimary);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float, float,
                          juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(8.0f);
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();
        float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 4.0f;

        // Outer glow based on position
        float normalizedPos = sliderPosProportional * 2.0f - 1.0f; // -1 to +1
        juce::Colour glowColor = normalizedPos < 0 ? Colors::expandColor : Colors::compressColor;
        float glowIntensity = std::abs(normalizedPos) * 0.4f;

        if (glowIntensity > 0.05f)
        {
            g.setColour(glowColor.withAlpha(glowIntensity * 0.5f));
            g.fillEllipse(cx - radius - 8, cy - radius - 8, (radius + 8) * 2, (radius + 8) * 2);
        }

        // Outer ring with gradient
        juce::ColourGradient ringGradient(
            Colors::expandColor, cx - radius, cy,
            Colors::compressColor, cx + radius, cy, false);
        g.setGradientFill(ringGradient);
        g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

        // Inner dark circle
        float innerRadius = radius * 0.85f;
        g.setColour(juce::Colour(0xff1a1a2e));
        g.fillEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

        // Knob body with metallic gradient
        float knobRadius = radius * 0.75f;
        juce::ColourGradient knobGradient(
            juce::Colour(0xff555555), cx - knobRadius * 0.5f, cy - knobRadius * 0.5f,
            juce::Colour(0xff2a2a2a), cx + knobRadius * 0.5f, cy + knobRadius * 0.5f, true);
        g.setGradientFill(knobGradient);
        g.fillEllipse(cx - knobRadius, cy - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        // Knob highlight
        g.setColour(juce::Colour(0xff666666));
        g.drawEllipse(cx - knobRadius, cy - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.0f);

        // Draw center marker
        float markerRadius = knobRadius * 0.15f;
        g.setColour(Colors::textDim);
        g.fillEllipse(cx - markerRadius, cy - markerRadius, markerRadius * 2.0f, markerRadius * 2.0f);

        // Indicator line
        float indicatorAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, -2.356f, 2.356f) - juce::MathConstants<float>::halfPi;
        float indicatorLength = knobRadius * 0.6f;
        float ix1 = cx + knobRadius * 0.25f * std::cos(indicatorAngle);
        float iy1 = cy + knobRadius * 0.25f * std::sin(indicatorAngle);
        float ix2 = cx + indicatorLength * std::cos(indicatorAngle);
        float iy2 = cy + indicatorLength * std::sin(indicatorAngle);

        // Indicator color based on position
        juce::Colour indicatorColor = normalizedPos < -0.05f ? Colors::expandColor :
                                      normalizedPos > 0.05f ? Colors::compressColor :
                                      Colors::accentYellow;
        g.setColour(indicatorColor);
        g.drawLine(ix1, iy1, ix2, iy2, 3.0f);

        // Draw scale labels around the knob
        g.setFont(10.0f);

        // Left label (EXPAND)
        g.setColour(Colors::expandColor);
        g.drawText("EXP", cx - radius - 30, cy - 5, 25, 12, juce::Justification::centredRight);

        // Right label (COMPRESS)
        g.setColour(Colors::compressColor);
        g.drawText("COMP", cx + radius + 5, cy - 5, 35, 12, juce::Justification::centredLeft);

        // Center tick mark
        g.setColour(Colors::accentYellow);
        float tickY = cy - radius - 5;
        g.drawLine(cx, tickY, cx, tickY - 8, 2.0f);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        float ledSize = 14.0f;
        float ledX = 4.0f;
        float ledY = bounds.getCentreY() - ledSize / 2.0f;

        bool isOn = button.getToggleState();

        if (isOn)
        {
            g.setColour(Colors::accent.withAlpha(0.3f));
            g.fillEllipse(ledX - 2, ledY - 2, ledSize + 4, ledSize + 4);
        }

        g.setColour(isOn ? Colors::accent : Colors::textDim);
        g.fillEllipse(ledX, ledY, ledSize, ledSize);

        g.setColour(juce::Colours::white.withAlpha(isOn ? 0.4f : 0.1f));
        g.fillEllipse(ledX + 2, ledY + 2, 4, 4);

        g.setColour(shouldDrawButtonAsHighlighted ? Colors::textPrimary : Colors::textSecondary);
        g.setFont(12.0f);
        g.drawText(button.getButtonText(), bounds.withTrimmedLeft(ledSize + 10),
                   juce::Justification::centredLeft);
    }
};
