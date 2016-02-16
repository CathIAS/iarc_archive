#ifndef _PID_H
#define _PID_H

class PID
{
    public:
        PID(float _Kp = 2.0, float _Ki = 0.001, float _Kd = 1.0, float _derivative = 0.0, float _integral = 0.0, float _integralMin = -200, float _integralMax = 200, float _ctrlMin = -10, float _ctrlMax = 10);
        float update(float currentPoint);
        void set_point(float _setPoint);

    private:
        float Kp, Ki, Kd;
        float PVal, IVal, DVal;
        float error;
        float setPoint;

        float derivative;
        float integral, integralMin, integralMax;
        float ctrl, ctrlMin, ctrlMax;
};

#endif
