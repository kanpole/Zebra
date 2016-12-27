/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Elephant
 *    > Created Time:  2016-12-25 20:22:04
**/

#include <sstream>

#include "nn.hpp"

namespace Elephant {

NeuralNetwork::NeuralNetwork(const std::vector<size_t> &layers, const Activation &activation)
:layers_(layers), activation_(activation)
{
	w_ = std::vector<Matrix<double>>(layers_.size());
	b_ = std::vector<Vector<double>>(layers_.size());
	dw_ = std::vector<Matrix<double>>(layers_.size());
	db_ = std::vector<Vector<double>>(layers_.size());
}

void NeuralNetwork::Train(double rate, size_t max_iter, size_t batch, double reg,
	const Matrix<uint8_t> &x, const Vector<uint8_t> &y)
{
	assert(layers_.size());
	auto shape = x.Shape();
	size_t a = shape.second, b = layers_[0];
	for (size_t i = 0; ; ++i) {
		w_[i] = Matrix<double>::Randomize(a, b, 0.1);
		b_[i] = Vector<double>::Randomize(b, 0.1);
		if (i == (layers_.size() - 1))
			break;
		a = b;
		b = layers_[i + 1];
	}

	std::vector<double> loss(max_iter);
	for (size_t i = 0; i != max_iter; ++i) {
		auto index = Vector<size_t>::RandomIndex(batch, shape.first);
		Matrix<double> m(x, index);
		Vector<uint8_t> n(y, index);
		loss[i] = ComputeLoss(m, n, reg);
		for (size_t j = 0; j != layers_.size(); ++j) {
			w_[j] -= dw_[j] * rate;
			b_[j] -= db_[j] * rate;
		}
	}
}

double NeuralNetwork::ComputeLoss(const Matrix<double> &x, const Vector<uint8_t> &y, double reg)
{
	std::vector<Matrix<double>> m(layers_.size());
	std::vector<Matrix<double>> n(layers_.size());
	n[0] = x;
	for (size_t i = 0; i != layers_.size(); ++i) {
		m[i] = n[i].Dot(w_[i]);
		m[i] += b_[i];
		n[i] = activation_.Forward(m[i]);
	}

	auto exp = m[layers_.size()-1].Exp();
	auto shape = exp.Shape();
	auto sum = exp.Sum(0);
	Vector<double> tmp(shape.first);
	for (size_t i = 0; i != shape.first; ++i)
		tmp[i] = exp[i][y[i]] / sum[i];
	tmp = -tmp.Log();

	double loss = tmp.Sum() / shape.first;
	for (size_t i = 0; i != layers_.size(); ++i)
		loss += 0.5 * reg * (w_[i] * w_[i]).Sum();

	auto dm = exp / sum;
	for (size_t i = 0; i != shape.first; ++i)
		dm[i][y[i]] -= 1;
	dm /= shape.first;

	for (int i = layers_.size() - 1; i >= 0; --i) {
		dw_[i] = n[i]
	}

	return loss;
}

} // namespace Elephant
