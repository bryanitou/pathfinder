//
// Created by bryan on 17/04/22.
//

#include "state_sampler.h"
#include "ompl/base/spaces/DubinsStateSpace.h"

void DubinsStateSampler_Radius::defaultSettings()
{
    stateSpace_ = space_->as<ob::DubinsStateSpace>();
    if (stateSpace_ == nullptr)
        throw ompl::Exception("No st ate space for state sampler");
}

void DubinsStateSampler_Radius::sampleUniform(ob::State *state)
{

    double x = rng_.uniformReal(-1,1);
    double y = rng_.uniformReal(-1,1);
    double yaw = rng_ . uniformReal (- M_PI , + M_PI );
    double sampled_radius = rng_.uniformReal(this->min_radius_, this->min_radius_*10);

    state->as<ob::DubinsStateSpace::StateType>()->setX(x);
    state->as<ob::DubinsStateSpace::StateType>()->setY(y);
    state->as<ob::DubinsStateSpace::StateType>()->setYaw(yaw);
}

void DubinsStateSampler_Radius::setSamplerBounds() {
    auto state_bounds = stateSpace_->getBounds();

    auto high = state_bounds.high;
    auto low = state_bounds.low;


}

void DubinsStateSampler_Radius::sampleUniformNear(ob::State *state, const ob::State *near, double distance)
{
    // Nothing here

}

void DubinsStateSampler_Radius::sampleGaussian(ob::State *state, const ob::State *mean, double stdDev)
{
    // Nothing here

}
