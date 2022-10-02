

#pragma once


// OMPL libraries
#include "ompl/base/ValidStateSampler.h"
#include "ompl/base/StateSampler.h"
#include "ompl/base/StateSpace.h"
#include "ompl/base/spaces/DubinsStateSpace.h"

#include "ompl/util/Exception.h"
#include "ompl/base/spaces/RealVectorStateSpace.h"
#include "ompl/base/SpaceInformation.h"

namespace ob = ompl::base;

// This is a problem-specific sampler that automatically generates valid
// states; it doesn't need to call SpaceInformation::isValid. This is an
// example of constrained sampling. If you can explicitly describe the set valid
// states and can draw samples from it, then this is typically much more
// efficient than generating random samples from the entire state space and
// checking for validity.
class DubinsStateSampler_Radius : public ob::StateSampler
{
public:
    DubinsStateSampler_Radius(const ob::StateSpace *space)
    : StateSampler(space)
    {
        defaultSettings();
        setSamplerBounds();
        min_radius_ = 5; // m
    }

    ~DubinsStateSampler_Radius() override = default;

    // Set space bounds for efficiency purposes
    void setSamplerBounds();

    // Sampler
    void sampleUniform(ob::State *state) override;

    void sampleGaussian(ob::State *state, const ob::State *mean, double stdDev) override;

    void sampleUniformNear(ob::State *state, const ob::State *near, double distance) override;

protected:
    ompl::RNG rng_;
    // ob::StateSamplerPtr sampler_;
private:
    const ob::DubinsStateSpace* stateSpace_;
    double min_radius_;

private:
    void defaultSettings();
};