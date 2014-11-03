/*
 * @file mpm_solid_base.h 
 * @Brief base class of all MPM drivers for solid.
 * @author Fei Zhu
 * 
 * This file is part of Physika, a versatile physics simulation library.
 * Copyright (C) 2013 Physika Group.
 *
 * This Source Code Form is subject to the terms of the GNU General Public License v2.0. 
 * If a copy of the GPL was not distributed with this file, you can obtain one at:
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 */

#ifndef PHYSIKA_DYNAMICS_MPM_MPM_SOLID_BASE_H_
#define PHYSIKA_DYNAMICS_MPM_MPM_SOLID_BASE_H_

#include <string>
#include <vector>
#include "Physika_Core/Vectors/vector_2d.h"
#include "Physika_Core/Vectors/vector_3d.h"
#include "Physika_Dynamics/MPM/mpm_base.h"

namespace Physika{

template<typename Scalar> class DriverPluginBase;
template<typename Scalar,int Dim> class SolidParticle;

template <typename Scalar, int Dim>
class MPMSolidBase: public MPMBase<Scalar,Dim>
{
public:
    MPMSolidBase();
    MPMSolidBase(unsigned int start_frame, unsigned int end_frame, Scalar frame_rate, Scalar max_dt, bool write_to_file);
    MPMSolidBase(unsigned int start_frame, unsigned int end_frame, Scalar frame_rate, Scalar max_dt, bool write_to_file,
                 const std::vector<SolidParticle<Scalar,Dim>*> &particles);
    virtual ~MPMSolidBase();

    //virtual methods
    virtual void initConfiguration(const std::string &file_name)=0;
    virtual void printConfigFileFormat()=0;
    virtual void initSimulationData()=0;
    virtual void addPlugin(DriverPluginBase<Scalar> *plugin)=0;
    virtual bool withRestartSupport() const=0;
    virtual void write(const std::string &file_name)=0;
    virtual void read(const std::string &file_name)=0;

    //get && set
    unsigned int particleNum() const;
    virtual void addParticle(const SolidParticle<Scalar,Dim> &particle);
    virtual void removeParticle(unsigned int particle_idx);
    virtual void setParticles(const std::vector<SolidParticle<Scalar,Dim>*> &particles); //set all simulation particles, data are copied
    const SolidParticle<Scalar,Dim>& particle(unsigned int particle_idx) const;
    SolidParticle<Scalar,Dim>& particle(unsigned int particle_idx);
    const std::vector<SolidParticle<Scalar,Dim>*>& allParticles() const;  //get all the simulation particles
    //set and get external force on particles, gravity is not included
    Vector<Scalar,Dim> externalForceOnParticle(unsigned int particle_idx) const;
    void setExternalForceOnParticle(unsigned int particle_idx, const Vector<Scalar,Dim> &force);   
    //particles used as Dirichlet boundary condition, velocity is prescribed 
    void addDirichletParticle(unsigned int particle_idx);  //the particle is set as boundary condition
    void addDirichletParticles(const std::vector<unsigned int> &particle_idx); //the particles are set as boundary condition

    //substeps in one time step
    virtual void rasterize()=0;  //rasterize data to grid
    virtual void solveOnGrid(Scalar dt)=0; //solve the dynamics system on grid
    virtual void performGridCollision(Scalar dt)=0; //perform grid based collision with collidable object in scene
    virtual void performParticleCollision(Scalar dt)=0;  //perform particle based collision with collidable object in scene
    virtual void updateParticleInterpolationWeight()=0;  //compute the interpolation weight between particles and grid nodes
    virtual void updateParticleConstitutiveModelState(Scalar dt)=0; //update the constitutive model state of particle, e.g., deformation gradient
    virtual void updateParticleVelocity()=0;  //update particle velocity using grid data
    virtual void applyExternalForceOnParticles(Scalar dt)=0;  //external force (gravity excluded) is applied on particles
    virtual void updateParticlePosition(Scalar dt)=0;  //update particle position with new particle velocity
    
    //different time integration methods
    enum IntegrationMethod{
        FORWARD_EULER,
        BACKWARD_EULER
    };
    void setTimeIntegrationMethod(const IntegrationMethod &method);
protected:
    virtual Scalar minCellEdgeLength() const = 0; //minimum edge length of the background grid, for dt computation
    virtual Scalar maxParticleVelocityNorm() const;
    virtual void applyGravityOnGrid(Scalar dt) = 0;
    virtual void allocateSpaceForAllParticleRelatedData();
    virtual void initializeAllParticleRelatedData();
    virtual void appendSpaceForParticleRelatedData();
    virtual void initializeLastParticleRelatedData();
    virtual void deleteParticleRelatedData(unsigned int particle_idx);
    //solve on grid with different integration methods, called in solveOnGrid()
    virtual void solveOnGridForwardEuler(Scalar dt) = 0;
    virtual void solveOnGridBackwardEuler(Scalar dt) = 0;
protected:
    std::vector<SolidParticle<Scalar,Dim>*> particles_;
    std::vector<unsigned char> is_dirichlet_particle_;  //for each particle in particles_, 
                                                        //use one byte to indicate whether it's set as dirichlet boundary condition
    std::vector<Scalar> particle_initial_volume_;
    std::vector<Vector<Scalar,Dim> > particle_external_force_; //external force(/N), not acceleration
    IntegrationMethod integration_method_; 
};

}//namespace Physika

#endif //PHYSIKA_DYNAMICS_MPM_MPM_SOLID_BASE_H_