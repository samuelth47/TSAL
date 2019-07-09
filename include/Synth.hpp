#ifndef SYNTH_H
#define SYNTH_H

#include "Oscillator.hpp"
#include "Envelope.hpp"
#include "Instrument.hpp"
#include "MidiNotes.hpp"

namespace tsal {

/** @class Synth
 * @brief An audio synth 
 * 
 * Synth uses the oscillator and envelope classes to create a basic 
 * monophonic synthesizer that can be used to play midinotes
 */
class Synth : public Instrument {
  public:
    virtual double getOutput() override;
    virtual void noteOn(unsigned note, double velocity = 100.0) override;
    virtual void noteOff(unsigned note = A0) override;
    void setVelocity(double velocity);
    void setMode(Oscillator::OscillatorMode mode) { mOscillator.setMode(mode); };
    unsigned getNote() const;

  private:
    Oscillator mOscillator;
    Envelope mEnvelope;
    double mVelocity;
    unsigned mNote = 0;
    
    static ParameterRange<double> mVelocityRange;
}; 

}

#endif 
