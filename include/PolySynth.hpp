#ifndef POLYSYNTH_H
#define POLYSYNTH_H

#include "Synth.hpp"
#include "Oscillator.hpp"
#include "Instrument.hpp"

#define NUM_VOICES 20

namespace tsal {

/** @class PolySynth
 * @brief A polyphonic synthesizer
 *
 * PolySynth uses multiple Synths to create a polyphonic 
 * synthesizer that can handle multiple notes being played
 * at the same time
 */
class PolySynth : public Instrument {
  public:
    PolySynth();
    virtual double getOutput() override;
    virtual void noteOn(unsigned note, double velocity = 100.0) override;
    virtual void noteOff(unsigned note) override;
    void setMode(Oscillator::OscillatorMode mode);
 private:
    Synth* getInactiveVoice();
    Oscillator::OscillatorMode mMode;
    Synth mVoices[20];
};
  
}

#endif
