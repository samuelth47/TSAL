#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>


#define EXCLUSIVE 0.0001

namespace tsal {

/** @class ParameterManager
 * @brief An interface for defining and modifing parameters on a device
 *
 * ParameterManager offers a interface for defining the parameters and their limits on an audio device
 */
class ParameterManager {
  public:
    struct Parameter {
        // The name the of the parameter
        std::string name; 
        // The minimum value that a user can assign
        double min;
        // The maximum value that a user can assign
        double max;
        // The default value that the parameter is initialized with
        double defaultValue;
        // Allows value to approach minimum without actually equaling minimum
        bool exclusiveMax = false;
        // Allows value to approach maximum without actually equaling maximum
        bool exclusiveMin = false;
        // Pointer to value
        double* connection = NULL;
    };
    ParameterManager() = default;
    ParameterManager(std::vector<Parameter> parameters) {
      mParameters.swap(parameters);
      for (auto& param : mParameters) {
        initializeParameter(param);
      }
    };

    std::string getParameterName(unsigned id) const { return mParameters[id].name; };
    Parameter& getParameterData(unsigned id) { return mParameters[id]; };
    double* getParameterPointer(unsigned id) { return &(mValues[id]); };
    template <typename T>
    T getParameter(unsigned id) { return static_cast<T>(getParameter(id)); };
    int getParameterInt(unsigned id) { return std::round(getParameter(id)); };
    inline double getParameter(unsigned id) {
      return mParameters[id].connection == NULL ? mValues[id] : getConnectedParameter(id);
    };
    inline void setParameter(unsigned id, double value) {
      mValues[id] = std::min(std::max(value, mParameters[id].min), mParameters[id].max);
      parameterUpdate(id);
    }
    void connectParameter(unsigned id, double* connection) {
      mParameters[id].connection = connection;
    }
    void disconnectParameter(unsigned id) {
      mParameters[id].connection = NULL;
    }
    void disconnectParameters() {
      for (unsigned i = 0; i < mParameters.size(); i++) {
        disconnectParameter(i);
      }
    }
    size_t getParameterCount() { return mParameters.size(); };
  protected:
    virtual void parameterUpdate(unsigned id) {};
    void addParameters(std::vector<Parameter> parameters) {
      for (unsigned i = 0; i < parameters.size(); i++) {
        mParameters.push_back(parameters[i]);
        initializeParameter(mParameters.back());
      }
    }
  private:
    void initializeParameter(Parameter& parameter) {
      mValues.push_back(parameter.defaultValue);
      if (parameter.exclusiveMax) {
        parameter.max -= EXCLUSIVE;
      }
      if (parameter.exclusiveMin) {
        parameter.min += EXCLUSIVE;
      }
    }
    double getConnectedParameter(unsigned id) {
      return *(mParameters[id].connection);
    };
    std::vector<Parameter> mParameters;
    std::vector<double> mValues;
};

}
#endif
