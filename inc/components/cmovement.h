#ifndef CMOVEMENT_H
#define CMOVEMENT_H

#include "common.h"
#include "component.h"

class CMovement : public Component
{
    INC_FACTORY_REGISTER(CMovement);

    Dir direction;
    int velocity;
    int turnAngle;

public:
    virtual bool init(YAML::Node node);

    Dir getUpdatedDirection();
    int getUpdatedVelocity();
};

#endif // CMOVEMENT_H
