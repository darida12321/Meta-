#include <iostream>
#include <Eigen/Dense>
#include <layer.h>
#include <ostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

// constructor
Layer::Layer(MatrixXd m, VectorXd b, std::function<double(double)> act_func,
             std::function<double(double)> act_func_der)
    : weights_(m), bias_(b), act_func_(act_func), act_func_der_(act_func_der)
    , backprop_weight_acc_(m - m), backprop_bias_acc_(b-b){} //TODO to do VERY

VectorXd Layer::forwardProp(VectorXd in) {
  VectorXd newV = weights_ * in + bias_;
  return newV.unaryExpr(act_func_);
}

void Layer::applyAccumulatedChange(int sampleSize){
    weights_ += backprop_weight_acc_;
    bias_ += backprop_bias_acc_;
    backprop_weight_acc_.setZero();
    backprop_bias_acc_.setZero();
}

VectorXd Layer::forwardPropAndStore(VectorXd in) {
  VectorXd newV = weights_ * in + bias_;

  last_input_ = in; // record input for use in back propagation
  // calculate derivatives of activation function for use in back propagation
  act_derivatives_ = newV.unaryExpr(act_func_der_);

  return newV.unaryExpr(act_func_);
}

// Precondition: err = dC/da {where C = cost, a = this layer's activation}
VectorXd Layer::backProp(VectorXd gradient, double stepSize) {
  // calculate dC/dz where z is the activation before applying act_func
  VectorXd tmp = gradient.cwiseProduct(act_derivatives_);

  // calculate dC/da for previous layer
  VectorXd propagated = weights_.transpose() * tmp;
  // VectorXd propagated = weights_ * tmp;

  // adjust weights and biases
  backprop_weight_acc_ -= stepSize * tmp * last_input_.transpose();
  backprop_bias_acc_ -= stepSize * tmp;

  return propagated;
}
MatrixXd Layer::getWeights() {
    return weights_;
}
VectorXd Layer::getBias() {
    return bias_;
}
