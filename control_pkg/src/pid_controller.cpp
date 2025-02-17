#include "control_pkg/pid_controller.h"
#include <iostream>
#include <cmath>


namespace control
{

double PIDController::Control(const double error)
{
    if (nts_ <= 0) 
    {
        std::cout << "nts_ <= 0, will use the last output, nts_: " << nts_ << std::endl;
        return previous_output_;
    }
    double diff = 0;
    double output = 0;

    if (first_hit_) 
    {
        first_hit_ = false;
    } 
    else 
    {
        diff = (error - previous_error_) / nts_;
    }
    // integral hold
    if (!integrator_enabled_) 
    {
        integral_ = 0;
    } 
    else if (!integrator_hold_) 
    {
        integral_ += error * nts_ * ki_;
        // apply Ki before integrating to avoid steps when change Ki at steady state
        if (integral_ > integrator_saturation_high_)
        {
            integral_ = integrator_saturation_high_;
            integrator_saturation_status_ = 1;
        } 
        else if (integral_ < integrator_saturation_low_) 
        {
            integral_ = integrator_saturation_low_;
            integrator_saturation_status_ = -1;
        } 
        else 
        {
            integrator_saturation_status_ = 0;
        }
    }
    previous_error_ = error;
    output = error * kp_ + integral_ + diff * kd_;  // Ki already applied
    previous_output_ = output;
    return output;
}

void PIDController::Reset()
{
    previous_error_ = 0.0;
    previous_output_ = 0.0;
    integral_ = 0.0;
    first_hit_ = true;
    integrator_saturation_status_ = 0;
    output_saturation_status_ = 0;
}

void PIDController::Init(const PidConf &pid_conf)
{
    previous_error_ = 0.0;
    previous_output_ = 0.0;
    integral_ = 0.0;
    first_hit_ = true;
    integrator_enabled_ = pid_conf.integrator_enable;
    integrator_saturation_high_ =
        std::fabs(pid_conf.integrator_saturation_level);
    integrator_saturation_low_ =
        -std::fabs(pid_conf.integrator_saturation_level);
    integrator_saturation_status_ = 0;
    integrator_hold_ = false;
    output_saturation_high_ = std::fabs(pid_conf.output_saturation_level);
    output_saturation_low_ = -std::fabs(pid_conf.output_saturation_level);
    output_saturation_status_ = 0;
    kp_ = pid_conf.kp;
    ki_ = pid_conf.ki;
    kd_ = pid_conf.kd;
    nts_ = pid_conf.nts;
    kaw_ = pid_conf.kaw;
}

int PIDController::IntegratorSaturationStatus() const
{
    return integrator_saturation_status_;
}

bool PIDController::IntegratorHold() const { return integrator_hold_; }

void PIDController::SetIntegratorHold(bool hold) { integrator_hold_ = hold; }

}  // namespace control
