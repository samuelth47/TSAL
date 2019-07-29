#include "Delay.hpp"
#include "Mixer.hpp"

namespace tsal {

Delay::Delay(Mixer* mixer) : Effect(mixer) {
  init();
}

/* @brief Dynamically allocates a buffer based on the sample rate
 *
 * Since the buffer since of the delay is dependent on the sample
 * rate, the buffer has to be reallocated whenever the sample rate
 * changes
 */
void Delay::init() {
  mBuffer = std::make_unique<Buffer<double>>(mMixer->getSampleRate() * 2);
  Delay::mDelayRange = std::make_pair(0, mBuffer->size());
  setDelay(1000);
}

void Delay::setMixer(Mixer* mixer) {
  OutputDevice::setMixer(mixer);
  init();
}

double Delay::getOutput() {
  double const input = getInput();
  if (!mActive) {
    return input;
  }

  auto& buffer = *mBuffer.get();
  int offset = mCounter - mDelay;

  // Clip lookback buffer-bound
  if (offset < 0)
    offset = buffer.size() + offset;

  double const output = buffer[offset];
  double const bufferValue = input + output * mFeedback;
  buffer[mCounter++] = bufferValue;

  // Clip delay counter
  if(mCounter >= buffer.size())
    mCounter -= buffer.size();

  return output + input;
}

/**
 * @brief Set the delay
 * 
 * @param delay (ms)
 */
void Delay::setDelay(unsigned delay) {
  delay = std::round(mMixer->getSampleRate() * ((double) delay / 1000));
  mDelay = Util::checkParameterRange("Delay: Delay", delay, mDelayRange);
}

/**
 * @brief Set the feedback 
 * 
 * @param feedback 
 */
void Delay::setFeedback(double feedback) {
  mFeedback = Util::checkParameterRange("Delay: Feedback", feedback, mFeedbackRange);
}

Util::ParameterRange<double> Delay::mFeedbackRange = std::make_pair(0.0, 1.0);
}
