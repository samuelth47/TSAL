#include "Filter.hpp"

namespace tsal {

std::vector<ParameterManager::Parameter> Filter::FilterParameters
({
  { .name="Filter Mode", .min=0.0, .max=2.0, .defaultValue=LOWPASS },
  { .name="Cutoff", .min=0.0, .max=1.0, .defaultValue=0.0, .exclusiveMax=true },
    { .name="Resonance", .min=0.0, .max=1.0, .defaultValue=0.0 }
});

double Filter::process(double input) {
  // https://www.musicdsp.org/en/latest/Filters/131-cascaded-resonant-lp-hp-filter.html
  const double cutoff = getParameter(CUTOFF);
  const double resonance = getParameter(RESONANCE);
  const double cutoffLow = (FilterMode) getParameter(FILTER_MODE) == LOWPASS ? getParameterData(CUTOFF).max : cutoff;
  const double cutoffHigh = (FilterMode) getParameter(FILTER_MODE) == HIGHPASS ? getParameterData(CUTOFF).max : cutoff;
  const double fbLow = resonance + resonance/(1 - cutoffLow);
  const double fbHigh = resonance + resonance/(1 - cutoffHigh);
  m1 = m1 + cutoffLow*(input - m1 + fbLow*(m1 - m2)) + mPentium4;
  m2 = m2 + cutoffLow*(m1 - m2);
  m3 = m3 + cutoffHigh*(m2 - m3 + fbHigh*(m3 - m4)) + mPentium4;
  m4 = m4 + cutoffHigh*(m3 - m4);
  return m2 - m4;
}

void Filter::setCutoff(double cutoff) {
  setParameter(CUTOFF, cutoff);
}

void Filter::setResonance(double resonance) {
  setParameter(RESONANCE, resonance);
}

double Filter::mPentium4 = 1.0e-24;
}
