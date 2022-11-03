/**
 * Amplification inheritance of the main DubinsClass
 * - Enhancement of the DubinsStateSpace Class.
 */

# include "ompl/base/spaces/DubinsStateSpace.h"

class Ampli_DubinsStateStateSpace : public ompl::base::DubinsStateSpace
{
public:

    /**
     * Default conistructor
     * @param turningRadius
     * @param isSymmetric
     */
    explicit Ampli_DubinsStateStateSpace(double turningRadius = 1.0, bool isSymmetric = false)
            : ompl::base::DubinsStateSpace(turningRadius, isSymmetric)
    {

    }

    /**
     * Default destructor
     */
    ~ Ampli_DubinsStateStateSpace() override = default;


public:

    /**
     * Gets the current set radius in the DubinsStateSpace.
     * @return double
     */
    double getRadius()
    {
        return this->rho_;
    }

    /**
     * Sets the radius in the DubinsStateSpace.
     */
    void setRadius(double newRadius)
    {
        this->rho_ = newRadius;
    }

};