/*
 * @file mpm_base.h 
 * @Brief base class of all MPM drivers.
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

#ifndef PHYSIKA_DYNAMICS_MPM_MPM_BASE_H_
#define PHYSIKA_DYNAMICS_MPM_MPM_BASE_H_

#include <vector>
#include <string>
#include "Physika_Core/Vectors/vector_2d.h"
#include "Physika_Core/Vectors/vector_3d.h"
#include "Physika_Core/Grid_Weight_Functions/grid_weight_function.h"
#include "Physika_Core/Grid_Weight_Functions/grid_weight_function_creator.h"
#include "Physika_Dynamics/Driver/driver_base.h"
#include "Physika_Dynamics/MPM/mpm_internal.h"
#include "Physika_Dynamics/MPM/MPM_Step_Methods/mpm_step_method.h"

namespace Physika{

template<typename Scalar> class DriverPluginBase;

template <typename Scalar, int Dim>
class MPMBase: public DriverBase<Scalar>
{
public:
    MPMBase();
    MPMBase(unsigned int start_frame, unsigned int end_frame, Scalar frame_rate, Scalar max_dt, bool write_to_file);
    virtual ~MPMBase();

    //virtual methods
    virtual void initConfiguration(const std::string &file_name)=0;
    virtual void printConfigFileFormat()=0;
    virtual void initSimulationData()=0;
    virtual void addPlugin(DriverPluginBase<Scalar> *plugin)=0;
    virtual bool withRestartSupport() const=0;
    virtual void write(const std::string &file_name)=0;
    virtual void read(const std::string &file_name)=0;

    virtual Scalar computeTimeStep();
    virtual void advanceStep(Scalar dt); //compute time step with CFL condition

    //getters&&setters
    Scalar cflConstant() const;
    void setCFLConstant(Scalar cfl);
    Scalar soundSpeed() const;
    void setSoundSpeed(Scalar sound_speed);
    Scalar gravity() const;
    void setGravity(Scalar gravity);

    //set the type of weight function with weight function type as template,
    //the scale between support domain and cell size is the method parameter
    template <typename GridWeightFunctionType>
    void setWeightFunction();
    //set the step method with the step method type as template
    template <typename MPMStepMethodType>
    void setStepMethod();
protected:
    virtual Scalar minCellEdgeLength() const=0; //minimum edge length of the background grid, for dt computation
    virtual Scalar maxParticleVelocityNorm() const=0; //return maximum norm the particles' velocity
    virtual void applyGravityOnGrid(Scalar dt) = 0;
    //allocate space for data related to particles according to particle number
    virtual void allocateSpaceForAllParticleRelatedData() = 0;
    //initialize the data related to all particles
    virtual void initializeAllParticleRelatedData() = 0;
    //append space for data related to the newly added particle 
    virtual void appendSpaceForParticleRelatedData() = 0;
    //initialize data related to the newly added particle, the last in list
    virtual void initializeLastParticleRelatedData() = 0;
    //delete data that is attached with particles, called when a particle is removed
    //need implementation in subclasses, when more data is attached to particles 
    virtual void deleteParticleRelatedData(unsigned int particle_idx);
protected:
    GridWeightFunction<Scalar,Dim> *weight_function_;
    MPMStepMethod<Scalar,Dim> *step_method_;
    //time step computation with CFL condition
    Scalar cfl_num_;
    Scalar sound_speed_;
    //gravity: along negative y direction
    Scalar gravity_;
    //precomputed weights and gradients of grid nodes that is within range of each particle
    //for each particle, store the node-value pair
    std::vector<std::vector<MPMInternal::NodeIndexWeightGradientPair<Scalar,Dim> > > particle_grid_weight_and_gradient_;
    std::vector<unsigned int> particle_grid_pair_num_; //the number of pairs in particle_grid_weight_and_gradient_ 
};

template <typename Scalar, int Dim>
template <typename GridWeightFunctionType>
void MPMBase<Scalar,Dim>::setWeightFunction()
{
    if(weight_function_)
        delete weight_function_;
    weight_function_ = GridWeightFunctionCreator<GridWeightFunctionType>::createGridWeightFunction();
    //reallocate space for particle_grid_weight_and_gradient_
    //for each particle, preallocate space that can store weight/gradient of maximum number of nodes in range
    allocateSpaceForAllParticleRelatedData();
}

template <typename Scalar, int Dim>
template <typename MPMStepMethodType>
void MPMBase<Scalar,Dim>::setStepMethod()
{
    if(step_method_)
        delete step_method_;
    step_method_ = new MPMStepMethodType();
    step_method_->setMPMDriver(this);
}

}  //end of namespace Physika

#endif