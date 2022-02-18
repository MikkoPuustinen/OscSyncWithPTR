#pragma once

#include <JuceHeader.h>


struct SynthVoice {
    bool isActive = false;
    int note = 0;
    int currentAngle = 1;

    double frequency = 0;
    double sampleRate = 0;
    float syncFrequency = 0;
    int syncAngle = 1;
    int W = 0;

    float T0 = 0;
    float P0 = 0;

    float sT0 = 0;
    float sP0 = 0;

    float cdc = 0;

    float plus = 0;

    float r = 0;
    float prevFreq = 0;

    void updateAngle(double freq, double sRate, float syncFreq, int polynomial)
    {
        frequency = freq;
        prevFreq = freq;
        sampleRate = sRate;
        syncFrequency = syncFreq;
        W = polynomial;

        T0 = frequency / sampleRate;
        P0 = 1 / T0;

        sT0 = syncFrequency / sampleRate;
        sP0 = 1 / sT0;

        cdc = W * sT0;

        r = syncFrequency / frequency;

    }

    float increment()
    {
        currentAngle++;

        syncAngle++;

        const float phi = (std::fmod(currentAngle * T0, 1));

        if (phi < T0)
        {
            syncAngle = 0;
            plus = r * phi;
        }

        const float slavePhi = (std::fmod(syncAngle * sT0 + plus, 1));
        const float sample = 2 * slavePhi - 1;

        float D = slavePhi * sP0;

        //if (D > 3)
        //{
        //    D = 3;
        //}

        float  h = 1;

        if ((std::fmod(r, 1) == 0) || phi >= W * T0)
        {
            h = 1;
        }
        else
        {
            h = r - floor(r);
        }


        switch (W)
        {
        case 0:
        {
            return sample;
        }
        case 1:
        {
            return getOnePol(phi, slavePhi, sample, D, h);
        }
        case 2:
        {
            return getTwoPol(phi, slavePhi, sample, D, h);
        }
        case 3:
        {
            return getThreePol(phi, slavePhi, sample, D, h);
        }
        }
    }

    float getOnePol(float master, float slave, float sample, float D, float h)
    {
        if (master < 1 * T0)
        {
            return ((2 * sT0 - 2 * h) * D + 2 * h - 1) - cdc;
        }
        else if (slave < 1 * sT0)
        {
            return ((2 * sT0 - 2 * h) * D + 2 * h - 1) - cdc;
        }
        else
        {
            return sample - cdc;
        }
    }

    float getTwoPol(float master, float slave, float sample, float D, float h)
    {

        if (master < T0)
        {
            return -h * pow(D, 2) + 2 * sT0 * D + 2 * h - 1 - cdc;
        }
        else if (master < 2 * T0)
        {
            return  h * pow(D, 2) + (2 * sT0 - 4 * h) * D + 4 * h - 1 - cdc;
        }
        else if (slave < sT0)
        {
            return  -h * pow(D, 2) + 2 * sT0 * D + 2 * h - 1 - cdc;
        }
        else if (slave < 2 * sT0)
        {
            return  h * pow(D, 2) + (2 * sT0 - 4 * h) * D + 4 * h - 1 - cdc;
        }
        else {
            return  sample - cdc;
        }

    }

    float getThreePol(float master, float slave, float sample, float D, float h)
    {
        if (master < T0)
            return -h * pow(D, 3) / 3 + 2 * sT0 * D + 2 * h - 1 - cdc;
        else if (master < 2 * T0)
        {
            return 2 * h * pow(D, 3) / 3 - 3 * h * pow(D, 2) + (2 * sT0 + 3 * h) * D + h - 1 - cdc;
        }
        else if (master < 3 * T0)
        {
            return -h * pow(D, 3) / 3 + 3 * h * pow(D, 2) + (2 * sT0 - 9 * h) * D + 9 * h - 1 - cdc;
        }
        else if (slave < sT0)
        {
            return -h * pow(D, 3) / 3 + 2 * sT0 * D + 2 * h - 1 - cdc;
        }
        else if (slave < 2 * sT0)
        {
            return 2 * h * pow(D, 3) / 3 - 3 * h * pow(D, 2) + (2 * sT0 + 3 * h) * D + h - 1 - cdc;
        }
        else if (slave < 3 * sT0)
        {
            return -h * pow(D, 3) / 3 + 3 * h * pow(D, 2) + (2 * sT0 - 9 * h) * D + 9 * h - 1 - cdc;
        }
        else
        {
            return sample - cdc;
        }
    }

    void setNote(int midiNote)
    {
        note = midiNote;
    }

    void setStart(int sample)
    {
        currentAngle = sample;
        syncAngle = sample;
    }

    void reset()
    {
        isActive = false;
        note = 0;
        currentAngle = 1;
        syncAngle = 1;
    }
};