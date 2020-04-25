#include "PolySynth.hpp"

namespace tsal {

std::vector<ParameterManager::Parameter> PolySynth::PolySynthParameters
({
  { .name="Osc1 Mode", .min=0.0, .max=3.0, .defaultValue=0.0 },
  { .name="Osc2 Mode", .min=0.0, .max=3.0, .defaultValue=0.0 },
  { .name="Osc2 Offset", .min=0.0, .max=1.0, .defaultValue=0.0},
  { .name="Modulation Mode", .min=0.0, .max=3.0, .defaultValue=0.0 },
  { .name="Env Attack", .min=0.0, .max=100.0, .defaultValue=0.1, .exclusiveMin=true },
  { .name="Env Decay", .min=0.0, .max=100.0, .defaultValue=0.5, .exclusiveMin=true},
  { .name="Env Sustain", .min=0.0, .max=1.0, .defaultValue=0.5 },
  { .name="Env Release", .min=0.0, .max=100.0, .defaultValue=2.0, .exclusiveMin=true},
});

void PolySynth::getOutput(AudioBuffer<float> &buffer) {
  if (!mActive) {
    return;
  }
  const auto channels = buffer.getChannelCount();
  const auto frames = buffer.getFrameCount();

  setChannelPanning(channels);

  for (unsigned long i = 0; i < frames; i++) {
    // Get the collective output of the voices
    double output = 0.0;
    double activeVoices = 0.0;
    for (size_t i = 0; i < mVoices.size(); i++) {
      if (mVoices[i].isActive()) {
        output += mVoices[i].getSample();
        ++activeVoices;
      }
    }
    // output += mVoices[0].getSample();
    // Scale the output by the number of active voices 
    output = activeVoices > 0 ? output / activeVoices : output;

    for (unsigned j = 0; j < channels; j++) {
      buffer[i * channels + j] = output * mChannelPanning[j] * mAmp; 
    }
  }
}

void PolySynth::updateContext(const Context& context) {
  OutputDevice::updateContext(context); 
  for (auto& voice : mVoices) {
    voice.updateContext(context);
  }
}

/* @brief Play a note with velocity
 *
 * @param note
 * @param velocity
 */
void PolySynth::play(double note, double velocity) {
  Voice* voice = getInactiveVoice();
  if (voice == nullptr) {
    voice = &mVoices[0]; 
    voice->stop();
  }
  voice->play(note, velocity);
}

/* @brief Stop playing a note
 *
 * @param note
 */
void PolySynth::stop(double note) {
  for (auto& voice : mVoices) {
    if (voice.getNote() == note) {
      voice.stop();
      voice.setActive(false);
    }
  }
}

/* @brief Set the mode of the underlying synths
 *
 * @param mode
 */
void PolySynth::setMode(Oscillator::OscillatorMode mode) {
  for (unsigned i = 0; i < mVoices.size(); i++) {
  }
}

PolySynth::Voice* PolySynth::getInactiveVoice() {
  // Whenever a note is pressed, an inactive voice needs to be found an played
  // If all the voices are active, a nullptr is returned
  Voice* voice = nullptr;
  for (unsigned i = 0; i < mVoices.size(); i++) {
    if (!mVoices[i].isActive()) {
      mVoices[i].setActive();
      voice = &mVoices[i];
      break;
    }
  }
  return voice;
}

PolySynth& PolySynth::operator=(const PolySynth& synth) {
  if (&synth != this) {
    // mVoices = std::vector<Voice>();

    // for (Synth s : mVoices) {
    //   mRoutedSynths.remove(s);
    // }

    // for (Synth s : synth.mVoices) {
    //   Synth copy = Synth(s);
    //   mVoices.push_back(std::move(copy));
    //   mRoutedSynths.add(s);
    // }
  }

  return *this;
}

double PolySynth::Voice::getSample() {
  mOsc1.setParameter(Oscillator::MODULATION, mOsc2.getSample());
  return mFilter.process(mOsc1.getSample() *
                         (mVelocity / 120.0) *
                         mEnvelope.getSample());
}

void PolySynth::Voice::play(double note, double velocity) {
  mNote = note;
  setActive();
  mOsc1.setActive();
  mOsc2.setActive();
  mEnvelope.start();
  mVelocity = velocity; 
  mOsc1.setNote(note);
  mOsc2.setNote(note);
}

void PolySynth::Voice::stop(double note) {
  (void)note;
  if (mEnvelope.isActive()) {
    mEnvelope.stop();
  } else {
    mOsc1.setActive(false);
    mOsc2.setActive(false);
  }
}

void PolySynth::Voice::updateContext(const Context& context) {
    mOsc1.updateContext(context);
    mOsc2.updateContext(context);
    mEnvelope.updateContext(context);
}

}
