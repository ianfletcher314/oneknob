#pragma once

#include <JuceHeader.h>
#include <cmath>

class DynamicsProcessor
{
public:
    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        envL = 0.0f;
        envR = 0.0f;

        // Calculate attack/release coefficients
        float attackMs = 10.0f;
        float releaseMs = 100.0f;
        attackCoef = std::exp(-1.0f / (float(sampleRate) * attackMs / 1000.0f));
        releaseCoef = std::exp(-1.0f / (float(sampleRate) * releaseMs / 1000.0f));
    }

    void setAmount(float amount)
    {
        // amount: -1.0 = full expansion, 0.0 = bypass, +1.0 = full compression
        this->amount = juce::jlimit(-1.0f, 1.0f, amount);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        if (std::abs(amount) < 0.001f)
            return; // Bypass when centered

        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

        const int numSamples = buffer.getNumSamples();

        // Parameters based on amount
        float threshold = -20.0f; // dB
        float ratio = 1.0f + std::abs(amount) * 7.0f; // 1:1 to 8:1
        float knee = 6.0f; // dB

        bool isCompression = amount > 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            float inL = leftChannel[i];
            float inR = rightChannel ? rightChannel[i] : inL;

            // Envelope follower (peak)
            float peakL = std::abs(inL);
            float peakR = std::abs(inR);

            float coefL = peakL > envL ? attackCoef : releaseCoef;
            float coefR = peakR > envR ? attackCoef : releaseCoef;

            envL = coefL * envL + (1.0f - coefL) * peakL;
            envR = coefR * envR + (1.0f - coefR) * peakR;

            // Convert to dB
            float envDbL = 20.0f * std::log10(envL + 1e-10f);
            float envDbR = 20.0f * std::log10(envR + 1e-10f);

            // Calculate gain reduction/expansion
            float gainDbL = computeGain(envDbL, threshold, ratio, knee, isCompression);
            float gainDbR = computeGain(envDbR, threshold, ratio, knee, isCompression);

            // Apply gain
            float gainL = std::pow(10.0f, gainDbL / 20.0f);
            float gainR = std::pow(10.0f, gainDbR / 20.0f);

            // Mix with amount intensity
            float intensity = std::abs(amount);
            gainL = 1.0f + (gainL - 1.0f) * intensity;
            gainR = 1.0f + (gainR - 1.0f) * intensity;

            leftChannel[i] = inL * gainL;
            if (rightChannel)
                rightChannel[i] = inR * gainR;
        }
    }

private:
    float computeGain(float inputDb, float threshold, float ratio, float knee, bool isCompression)
    {
        float gainDb = 0.0f;

        if (isCompression)
        {
            // Soft knee compression
            if (inputDb < threshold - knee / 2.0f)
            {
                gainDb = 0.0f;
            }
            else if (inputDb > threshold + knee / 2.0f)
            {
                gainDb = (threshold + (inputDb - threshold) / ratio) - inputDb;
            }
            else
            {
                // Knee region
                float x = inputDb - threshold + knee / 2.0f;
                gainDb = ((1.0f / ratio - 1.0f) * x * x) / (2.0f * knee);
            }
        }
        else
        {
            // Expansion (opposite of compression)
            if (inputDb > threshold + knee / 2.0f)
            {
                gainDb = 0.0f;
            }
            else if (inputDb < threshold - knee / 2.0f)
            {
                gainDb = (threshold + (inputDb - threshold) * ratio) - inputDb;
            }
            else
            {
                // Knee region
                float x = threshold + knee / 2.0f - inputDb;
                gainDb = -((ratio - 1.0f) * x * x) / (2.0f * knee);
            }
        }

        return gainDb;
    }

    double sampleRate = 44100.0;
    float amount = 0.0f;
    float envL = 0.0f;
    float envR = 0.0f;
    float attackCoef = 0.0f;
    float releaseCoef = 0.0f;
};
