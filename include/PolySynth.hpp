#ifndef POLYSYNTH_H
#define POLYSYNTH_H

#include "Synth.hpp"
#include "Oscillator.hpp"
#include "Instrument.hpp"
#include "RouteDevice.hpp"

#define NUM_VOICES 1 

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
    PolySynth() :
      Instrument(PolySynthParameters),
      mVoices(NUM_VOICES, Voice()) {
      for (auto& voice : mVoices) {
        voice.setActive(false);
      }
      connectParameters();
      mLFO.setMode(Oscillator::SINE);
      mLFO.setActive();
    };
    PolySynth& operator=(const PolySynth& synth) {
      if (this == &synth) return *this;
      mVoices = synth.mVoices;
      mLFO = synth.mLFO;
      connectParameters();
      return *this;
    };
    PolySynth(const PolySynth& synth) : PolySynth() {
      mVoices = synth.mVoices;
      mLFO = synth.mLFO;
      connectParameters();
    };
    PolySynth(const PolySynth&& synth) : PolySynth() {
      mVoices = synth.mVoices;
      mLFO = synth.mLFO;
      connectParameters();
    }
    static std::vector<Parameter> PolySynthParameters;
    enum Parameters {
                     VOLUME = 0, 
                     PANNING,
                     OSC1_MODE,
                     OSC2_MODE,
                     OSC2_OFFSET,
                     MODULATION_MODE,
                     ENV_ATTACK,
                     ENV_DECAY,
                     ENV_SUSTAIN,
                     ENV_RELEASE,
                     LFO_ACTIVE,
                     LFO_MODE,
                     LFO_FREQUENCY,
                     LFO_ATTACK,
                     LFO_DECAY,
                     LFO_SUSTAIN,
                     LFO_RELEASE,
    };
    virtual void getOutput(AudioBuffer<float> &buffer) override;
    void play(double note, double velocity = 100.0) override;
    void stop(double note) override;
    void setMode(Oscillator::OscillatorMode mode);
    virtual void updateContext(const Context& context) override;
  protected:
    virtual void parameterUpdate(unsigned id) override {
      Instrument::parameterUpdate(id);
      switch(id) {
      case LFO_FREQUENCY:
        mLFO.setFrequency(getParameter(LFO_FREQUENCY));
        break;
      }
    };
  private:
    void connectParameters() {
      mLFO.connectParameter(Oscillator::OSCILLATOR_MODE, getParameterPointer(LFO_MODE));
      for (auto& voice : mVoices) {
        voice.mOsc1.connectParameter(Oscillator::OSCILLATOR_MODE, getParameterPointer(OSC1_MODE));
        voice.mOsc2.connectParameter(Oscillator::OSCILLATOR_MODE, getParameterPointer(OSC2_MODE));
        voice.mOsc1.connectParameter(Oscillator::MODULATION_MODE, getParameterPointer(PolySynth::MODULATION_MODE));
        voice.mOsc2.connectParameter(Oscillator::PHASE_OFFSET, getParameterPointer(OSC2_OFFSET));

        voice.mEnvelope.connectParameter(Envelope::ATTACK, getParameterPointer(ENV_ATTACK));
        voice.mEnvelope.connectParameter(Envelope::DECAY, getParameterPointer(ENV_DECAY));
        voice.mEnvelope.connectParameter(Envelope::SUSTAIN, getParameterPointer(ENV_SUSTAIN));
        voice.mEnvelope.connectParameter(Envelope::RELEASE, getParameterPointer(ENV_RELEASE));

        voice.mLFOEnvelope.connectParameter(Envelope::ATTACK, getParameterPointer(LFO_ATTACK));
        voice.mLFOEnvelope.connectParameter(Envelope::DECAY, getParameterPointer(LFO_DECAY));
        voice.mLFOEnvelope.connectParameter(Envelope::SUSTAIN, getParameterPointer(LFO_SUSTAIN));
        voice.mLFOEnvelope.connectParameter(Envelope::RELEASE, getParameterPointer(LFO_RELEASE));
      }
    }
    class Voice : public Instrument {
      public:
        Voice() {
          mEnvelope.setActive();
          mLFOEnvelope.setActive();
          mFilter.setMode(Filter::BANDPASS);
          mFilter.setCutoff(0.1);
        }
        double getSample(double lfo);
        void play(double note, double velocity) override;
        void stop(double note = MidiNote::A0) override;
        virtual void updateContext(const Context& context) override; 
        double getNote() { return mOsc1.getNote(); };
        
        Oscillator mOsc1;
        Oscillator mOsc2;
        Filter mFilter;
        Envelope mEnvelope;
        Envelope mLFOEnvelope;

        Oscillator::ModulationMode mModulationMode = Oscillator::MIX;
        double mVelocity = 100.0;
        unsigned mNote = 0.0;
    };
    Voice* getInactiveVoice();
    Oscillator mLFO;
    std::vector<Voice> mVoices;
};
  
}

#endif
