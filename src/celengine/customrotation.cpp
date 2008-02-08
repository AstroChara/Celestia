// customrotation.cpp
//
// Custom rotation models for Solar System bodies.
//
// Copyright (C) 2008, the Celestia Development Team
// Initial version by Chris Laurel, claurel@gmail.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include <celengine/customrotation.h>
#include <celengine/rotation.h>
#include <celengine/astro.h>

using namespace std;


class IAURotationModel : public RotationModel
{
public:
    IAURotationModel(double _period) :
        period(_period)
    {
    }
    
    virtual ~IAURotationModel() {}
    
    bool isPeriodic() const { return true; }
    double getPeriod() const { return period; }
    
    virtual Quatd spin(double t) const
    {
        t = t - astro::J2000;
        return Quatd::yrotation(-degToRad(180.0 + meridian(t)));
    }
    
    virtual Quatd equatorOrientationAtTime(double t) const
    {
        double poleRA = 0.0;
        double poleDec = 0.0;
        
        t = t - astro::J2000;
        pole(t, poleRA, poleDec);
        double node = poleRA + 90.0;
        double inclination = 90.0 - poleDec;
        return Quatd::xrotation(degToRad(-inclination)) * Quatd::yrotation(degToRad(-node));
    }
    
    // Return the RA and declination (in degrees) of the rotation axis
    virtual void pole(double t, double& ra, double& dec) const = 0;
    
    virtual double meridian(double t) const = 0;
    
private:
    double period;
};


/*
S1 = 353.32 + 75706.7 * T
S2 = 28.72 + 75706.7 * T
S3 = 177.40 - 36505.5 * T
S4 = 300.00 - 7225.9 * T
S5 = 53.59 - 8968.6 * T
S6 = 143.38 - 10553.5 * T
S7 = 345.20 - 1016.3 * T
S8 = 29.80 - 52.1 * T
S9 = 316.45 + 506.2 * T
*/

// Rotations of Saturnian satellites from Seidelmann, _Explanatory Supplement to the
// Astronomical Almanac_ (1992).

class IAUMimasRotationModel : public IAURotationModel
{
public:
    IAUMimasRotationModel() : IAURotationModel(360.0 / 381.9945550) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        double S3 = degToRad(177.40 - 36505.5 * T);
        ra = 40.66 - 0.036 * T + 13.56 * sin(S3);
        dec = 83.52 - 0.004 * T - 1.53 * cos(S3);
    }
    
    double meridian(double t) const
    {
        double T = t / 36525.0;
        double S3 = degToRad(177.40 - 36505.5 * T);
        double S9 = degToRad(316.45 + 506.2 * T);
        return 337.46 + 381.9945550 * t - 13.48 * sin(S3) - 44.85 * sin(S9);        
    }
};


class IAUEnceladusRotationModel : public IAURotationModel
{
public:
    IAUEnceladusRotationModel() : IAURotationModel(360.0 / 262.7318996) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        ra = 40.66 - 0.036 * T;
        dec = 83.52 - 0.004 * T;
    }
    
    double meridian(double t) const
    {
        return 2.82 + 262.7318996 * t;
    }
};


class IAUTethysRotationModel : public IAURotationModel
{
public:
    IAUTethysRotationModel() : IAURotationModel(360.0 / 190.6979085) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        double S4 = degToRad(300.00 - 7225.9 * T);
        ra = 40.66 - 0.036 * T - 9.66 * sin(S4);
        dec = 83.52 - 0.004 * T - 1.09 * cos(S4);
    }
    
    double meridian(double t) const
    {
        double T = t / 36525.0;
        double S4 = degToRad(300.00 - 7225.9 * T);
        double S9 = degToRad(316.45 + 506.2 * T);
        return 10.45 + 190.6979085 * t - 9.60 * sin(S4) + 2.23 * sin(S9);        
    }
};


class IAUTelestoRotationModel : public IAURotationModel
{
public:
    IAUTelestoRotationModel() : IAURotationModel(360.0 / 190.6979330) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        ra = 50.50 - 0.036 * T;
        dec = 84.06 - 0.004 * T;
    }
    
    double meridian(double t) const
    {
        return 56.88 + 190.6979330 * t;
    }
};


class IAUCalypsoRotationModel : public IAURotationModel
{
public:
    IAUCalypsoRotationModel() : IAURotationModel(360.0 / 190.6742373) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        double S5 = degToRad(53.59 - 8968.6 * T);
        ra = 40.58 - 0.036 * T - 13.943 * sin(S5) - 1.686 * sin(2.0 * S5);
        dec = 83.43 - 0.004 * T - 1.572 * cos(S5) + 0.095 * cos(2.0 * S5);
    }
    
    double meridian(double t) const
    {
        double T = t / 36525.0;
        double S5 = degToRad(53.59 - 8968.6 * T);
        return 149.36 + 190.6742373 * t - 13.849 * sin(S5) + 1.685 * sin(2.0 * S5);
    }
};


class IAUDioneRotationModel : public IAURotationModel
{
public:
    IAUDioneRotationModel() : IAURotationModel(360.0 / 131.5349316) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        ra = 40.66 - 0.036 * T;
        dec = 83.52 - 0.004 * T;
    }
    
    double meridian(double t) const
    {
        return 357.00 + 131.5349316 * t;
    }
};


class IAUHeleneRotationModel : public IAURotationModel
{
public:
    IAUHeleneRotationModel() : IAURotationModel(360.0 / 131.6174056) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        double S6 = degToRad(143.38 - 10553.5 * T);
        ra = 40.58 - 0.036 * T + 1.662 * sin(S6) + 0.024 * sin(2.0 * S6);
        dec = 83.52 - 0.004 * T - 0.187 * cos(S6) + 0.095 * cos(2.0 * S6);
    }
    
    double meridian(double t) const
    {
        double T = t / 36525.0;
        double S6 = degToRad(143.38 - 10553.5 * T);
        return 245.39 + 131.6174056 * t - 1.651 * sin(S6) + 0.024 * sin(2.0 * S6);
    }
};


class IAURheaRotationModel : public IAURotationModel
{
public:
    IAURheaRotationModel() : IAURotationModel(360.0 / 79.6900478) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        double S7 = degToRad(345.20 - 1016.3 * T);
        ra = 40.38 - 0.036 * T + 3.10 * sin(S7);
        dec = 83.55 - 0.004 * T - 0.35 * cos(S7);
    }
    
    double meridian(double t) const
    {
        double T = t / 36525.0;
        double S7 = degToRad(345.20 - 1016.3 * T);
        return 235.16 + 79.6900478 * t - 1.651 - 3.08 * S7;
    }
};


class IAUTitanRotationModel : public IAURotationModel
{
public:
    IAUTitanRotationModel() : IAURotationModel(360.0 / 22.5769768) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        double S8 = degToRad(29.80 - 52.1 * T);
        ra = 36.41 - 0.036 * T + 2.66 * sin(S8);
        dec = 83.94 - 0.004 * T - 0.30 * cos(S8);
    }
    
    double meridian(double t) const
    {
        double T = t / 36525.0;
        double S8 = degToRad(29.80 - 52.1 * T);
        return 189.64 + 22.5769768 * t - 2.64 * sin(S8);
    }
};


class IAUIapetusRotationModel : public IAURotationModel
{
public:
    IAUIapetusRotationModel() : IAURotationModel(360.0 / 4.5379572) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        ra = 318.16 - 3.949 * T;
        dec = 75.03 - 1.142 * T;
    }
    
    double meridian(double t) const
    {
        return 350.20 + 4.5379572 * t;
    }
};


class IAUPhoebeRotationModel : public IAURotationModel
{
public:
    IAUPhoebeRotationModel() : IAURotationModel(360.0 / 22.5769768) {}
    
    void pole(double t, double& ra, double& dec) const
    {
        double T = t / 36525.0;
        ra = 355.16;
        dec = 68.70 - 1.143 * T;
    }
    
    double meridian(double t) const
    {
        return 304.70 + 930.8338720 * t;
    }
};



RotationModel*
GetCustomRotationModel(const std::string& name)
{
    if (name == "iau-mimas")
    {
        return new IAUMimasRotationModel();
    }
    else if (name == "iau-enceladus")
    {
        return new IAUEnceladusRotationModel();
    }
    else if (name == "iau-tethys")
    {
        return new IAUTethysRotationModel();
    }
    else if (name == "iau-telesto")
    {
        return new IAUTelestoRotationModel();
    }
    else if (name == "iau-calypso")
    {
        return new IAUCalypsoRotationModel();
    }
    else if (name == "iau-dione")
    {
        return new IAUDioneRotationModel();
    }
    else if (name == "iau-helene")
    {
        return new IAUHeleneRotationModel();
    }
    else if (name == "iau-rhea")
    {
        return new IAURheaRotationModel();
    }
    else if (name == "iau-titan")
    {
        return new IAUTitanRotationModel();
    }
    else if (name == "iau-iapetus")
    {
        return new IAUIapetusRotationModel();
    }
    else if (name == "iau-phoebe")
    {
        return new IAUPhoebeRotationModel();
    }
    else
    {
        return NULL;
    }
}


#if 0
// Mercury
a = 280.01 - 0.033 * T;
d = 61.45 - 0.005 * T;
W = 329.71 + 6.1385025 * t;

// Venus
a = 272.72;
d = 67.15;
W = 160.26 - 1.4813596 * t;

// Mars
a = 317.681 - 0.108 * T;
d = 52.886 - 0.061 * T;
W = 176.868 + 250.8919830 * t;

// Phobos
a = 317.68 - 0.108 * T + 1.79 * sin M1;
d = 52.90 - 0.061 * T - 1.08 * cos M1;
W = 35.06 + 1128.8445850 * t + 0.6644e-9 * t*t - 1.42 * sin M1 - 0.78 * sin M2;

// Deimos
a = 316.65  - 0.108 * T + 2.98 * sin M3;
d = 53.52 - 0.061 * T - 1.78 * cos M3;
W = 79.41 + 285.1618970 * t - 0.390e-10 * t*t - 2.58 * sin M3 + 0.19 * cos M3;
M1 = 169.51 - 0.4357640 * t;
M2 = 192.93 + 1128.4096700 * t + 0.6644e-9*t*t;
M3 = 53.47 - 0.0181510*t;
#endif

