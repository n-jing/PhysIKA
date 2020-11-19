#ifndef PhysIKA_GEN_MS_PROBLEM
#define PhysIKA_GEN_MS_PROBLEM
#include <boost/property_tree/ptree.hpp>

#include "Common/framework.h"
#include "Problem/constraint/constraints.h"
#include "Problem/energy/basic_energy.h"



namespace PhysIKA{

template<typename T>
class ms_problem_builder : public embedded_problem_builder<T, 3>{
 public:
  ms_problem_builder(const T*x, const boost::property_tree::ptree& pt); 
  ms_problem_builder() { }
  std::shared_ptr<Problem<T, 3>> build_problem() const;

  virtual int update_problem(const T* x, const T* v = nullptr);
  
  Eigen::Matrix<T, -1, -1> get_nods()const {return REST_;}
  Eigen::MatrixXi get_cells()const {return cells_;}
  std::shared_ptr<constraint_4_coll<T>> get_collider() const {return collider_;}
  
  protected:
  Eigen::Matrix<T, -1, -1> REST_;
  Eigen::MatrixXi cells_;

  std::shared_ptr<constraint_4_coll<T>> collider_;
  std::shared_ptr<momentum<T, 3>> kinetic_;
  std::vector<std::shared_ptr<Functional<T, 3>>> ebf_;
  std::vector<std::shared_ptr<Constraint<T>>> cbf_;

  boost::property_tree::ptree pt_;
};

}
#endif