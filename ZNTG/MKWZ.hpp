#pragma once

#define EFFECTIVE_DIGHITS 1000																			//保留有效位数
#define LIMIT_MAX 10000000																				//循环次数
#define EFFECTIVE_NUMBER 100																			//保留结果集
#define RETRACEMENT_MAX 0.08																			//最大回撤率
#define ED 0.01																										//最小权重幅度

#include <vector>
#include <algorithm>
#include <random>
#include "optional.hpp"

template <class T>
class Base {
public:
	virtual Optional<T> get(std::vector<std::vector<T>> data) { return Optional<T>(); }
};

template <class T>
class EXP : public Base<T> {
public:
	virtual Optional<T> get(std::vector<std::vector<T>> data) {
		if (data.size() != 1)
			return Optional<T>();
		auto d = data[0];
		T ret = 0;
		for (size_t i = 0; i < d.size(); i++) {
			ret += d[i];
		}
		return ret / d.size();
	}
};

template <class T>
class VAR : public Base<T> {
public:
	virtual Optional<T> get(std::vector<std::vector<T>> data) {
		if (data.size() != 2)
			return Optional<T>();
		auto d1 = data[0], d2 = data[1];
		T ret = 0, r1 = 0, r2 = 0, r3 = 0;
		for (size_t i = 0; i < d1.size(); i++) {
			r1 += d1[i];
			r2 += d2[i];
			r3 += d1[i] * d2[i];
		}
		return r3 / d1.size() - (r1 * r2) / (d1.size() * d1.size());
	}
};

template <class T>
class STDEV : public Base<T> {
public:
	virtual Optional<T> get(std::vector<std::vector<T>> data) {
		if (data.size() != 2)
			return Optional<T>();
		auto d1 = data[0], d2 = data[1];
		T ret = 0, r1 = 0, r2 = 0, r3 = 0;
		for (size_t i = 0; i < d1.size(); i++) {
			r1 += d1[i];
			r2 += d2[i];
			r3 += d1[i] * d2[i];
		}
		return sqrt(r3 / d1.size() - (r1 * r2) / (d1.size() * d1.size()));
	}
};

template <class T>
class CG {
public:
	std::vector<T> get(std::vector<T> data) {
		std::vector<T> gain;
		double value = 1;
		for (size_t i = 0; i < data.size(); i++) {
			value *= 1 + data[i];
			gain.push_back((value - 1));
		}
		return gain;
	}
};

class RD {
public:
	/*返回随机排序的数组*/
	std::vector<int> arrRd(int range) {
		std::vector<int> arr(range);
		std::random_device rd;
		std::mt19937 mt(rd());
		for (int i = 0; i < range; i++) {
			arr[i] = i;
		}
		for (int i = 0; i < range; i++) {
			auto position = mt() % range;
			auto temp = arr[i];
			arr[i] = arr[position];
			arr[position] = temp;
		}
		return arr;
	}
	/*返回累加为固定值的随机数组*/
	std::vector<int> arrRdCC(int constant, int range) {
		std::vector<int> arr(range);
		std::random_device rd;
		std::mt19937 mt(rd());
		auto temp = constant;
		for (int i = 0; i < range; i++) {
			arr[i] = mt() % (constant / range) + 1;
			temp -= arr[i];
		}
		while (temp != 0) {
			for (int i = 0; i < range; i++) {
				if (arr[i] + temp < constant / 2) {
					arr[i] += temp;
					temp = 0;
					break;
				}
				else {
					auto t = mt() % (temp / range);
					arr[i] += t;
					temp -= t;
				}
			}
		}
		return arr;
	}
	/*返回随机数*/
	int Rd(int range) {
		std::random_device rd;
		std::mt19937 mt(rd());
		return mt() % range;
	}
};

class  EV {
private:
	void copy(EV evd) {
		weight.assign(evd.weight.begin(), evd.weight.end());
		exp_arr.assign(evd.exp_arr.begin(), evd.exp_arr.end());
		w_var_arr.assign(evd.w_var_arr.begin(), evd.w_var_arr.end());
		cov.assign(evd.cov.begin(), evd.cov.end());
		exp = evd.exp;
		var = evd.var;
	}
	void recalculate(double rise, int x, int y, double &nExp, double &nVar){
		nExp = exp + rise * (exp_arr[x] - exp_arr[y]);
		nVar = var + rise*rise*(cov[x][x] - 2 * cov[x][y] + cov[y][y]) + 2 * rise * (w_var_arr[x] - w_var_arr[y]);
	}
	void renew(double rise, int x, int y, double nExp, double nVar) {
		exp = nExp;
		var = nVar;
		weight[x] += rise;
		weight[y] -= rise;
		for (size_t i = 0; i < w_var_arr.size(); i++) {
			w_var_arr[i] += rise * cov[i][x];
			w_var_arr[i] -= rise * cov[i][y];
		}
	}
public:
	double exp, var;
	std::vector<std::vector<double>> cov;
	std::vector<double> weight , exp_arr, w_var_arr;

	EV() :exp(0),var(0){}
	//EV(const EV& evd) { copy(evd); }
	//void  operator=(const EV& evd) { copy(evd); }

	void init(std::vector<std::vector<double>> rate , std::vector<double> weight , std::vector<int> position) {
		auto number = rate.size();
		this->weight.resize(number);
		for (size_t i = 0; i < position.size(); i++) {
			this->weight[position[i]] = weight[i];
		}

		this->exp_arr.resize(number);
		this->cov.resize(number);
		this->w_var_arr.resize(number);
		Base<double> *e = new EXP<double>() ,*v = new VAR<double>();
		for (size_t i = 0; i < number; i++) {
			this->exp_arr[i] = *e->get({ rate[i] });
			this->cov[i].resize(number);
			for (size_t j = 0; j < number; j++) {
				this->cov[i][j] = *v->get({ rate[i], rate[j] });
				this->w_var_arr[i] += this->cov[i][j] * this->weight[j];
			}
			this->exp += this->exp_arr[i] * this->weight[i];
			this->var += this->w_var_arr[i] * this->weight[i];
		}
		delete e;
		delete v;
	}

	void  update(double rise, int x, int y) {
		double nExp, nVar;
		this->recalculate(rise, x, y, nExp, nVar);
		renew(rise, x, y, nExp, nVar);
	}

	void update(std::vector<double> weight, std::vector<int> position) {
		this->exp = this->var = 0;
		auto number = this->weight.size();
		std::vector<double> w_temp(number);
		for (size_t i = 0; i < position.size(); i++) {
			w_temp[position[i]] = weight[i];
		}
		this->weight.assign(w_temp.begin(), w_temp.end());
		for (int i = 0; i < number; i++) {
			this->w_var_arr[i] = 0;
			this->exp += this->exp_arr[i] * this->weight[i];
			for (int j = 0; j < number; j++)
				this->w_var_arr[i] += this->cov[i][j] * this->weight[j];
			this->var += this->w_var_arr[i] * this->weight[i];
		}
	}

	/*更新求导取最小值函数*/
	void derivation(int x, int y) {
		double extremum = (this->w_var_arr[y] - this->w_var_arr[x]) / (this->cov[x][x] - 2 * this->cov[x][y] + this->cov[y][y]);
		double nExp, nVar  , rise;
		if (weight[x] + weight[y] > 0.5 ) {
			rise = extremum > 0.5 - ED ? 0.5 - weight[x] - ED :
				(extremum < weight[x] + weight[y] - 0.5 ? weight[y] - 0.5: extremum - weight[x]);
		}else {
			rise = extremum > weight[x] + weight[y] - ED ? weight[y] - ED :
				(extremum < ED ? ED - weight[x] : extremum - weight[x]);
		}
		recalculate(rise, x, y, nExp, nVar);
		renew(rise, x, y, nExp, nVar);
	}

	//迭代
	void optimum(int x,int y) {
		double nExp, nVar, min, max;
		if (weight[x] + weight[y] > 0.5) {
			min = weight[y] - 0.5 + ED;
			max = 0.5 - weight[x] - ED;
		}else {
			min = ED - weight[x];
			max = weight[y] - ED;
		}
		double e = -99999, v = -99999, r = -99999;
		for (double rise = min; rise < max; rise += 0.01) {
			recalculate(rise, x, y, nExp, nVar);
			if (nExp > e) {
				e = nExp; v = nVar; r = rise;
			}
		}
		renew(r, x, y, e, v);
	}

	double maxEXP() {
		return *std::max_element(this->exp_arr.begin(), this->exp_arr.end());
	}

	double minEXP() {
		return *std::min_element(this->exp_arr.begin(), this->exp_arr.end());
	}

	double maxVAR() {
		std::vector<double> temp;
		for (unsigned int i = 0; i < this->cov.size(); i++) {
			temp.push_back(*std::max_element(this->cov[i].begin(), this->cov[i].end()));
		}
		return *std::max_element(temp.begin(), temp.end());
	}

	double minVAR() {
		std::vector<double> temp;
		for (unsigned int i = 0; i < this->cov.size(); i++) {
			temp.push_back(*std::min_element(this->cov[i].begin(), this->cov[i].end()));
		}
		return *std::min_element(temp.begin(), temp.end());
	}

	std::vector<int> getPosition() {
		std::vector<int> position;
		for (unsigned int i = 0; i < this->weight.size(); i++) {
			if(weight[i] > 0.0000001)
				position.push_back(i);
		}
		return position;
	}
	std::vector<double> getWeight() {
		std::vector<double> w;
		for (size_t i = 0; i < this->weight.size(); i++) {
			if (weight[i] > 0.0000001)
				w.push_back(weight[i]);
		}
		return w;
	}
};

class MKWZ {
	struct pool {
		double e, v;
		std::vector<double>  w;
		std::vector<int> p;
		pool(EV ev) :e(ev.exp), v(ev.var), p(ev.getPosition()), w(ev.getWeight()) {}
	};
	std::vector<pool> ret;
	
	inline void combination(int n, int m, std::vector<int> &r, int num, std::vector<std::vector<int> > & result) {
		if (m == 0)
			return;
		if (m == 1) {
			for (int k = 0; k < n; k++) {
				std::vector<int> temp(num);
				r[num - 1] = k;
				for (int i = 0; i < num; i++)
					temp[i] = r[i];
				result.push_back(temp);
			}
		}
		else {
			for (int k = n; k >= m; k--) {
				r[m - 2] = k - 1;
				combination(k - 1, m - 1, r, num, result);
			}
		}
	}
	inline void CDS(EV ev) {
		auto complare = [=](pool a, pool b) {
			return a.e > b.e;
		};

		auto max = ev.maxEXP(), min = ev.minEXP(), range = max / EFFECTIVE_NUMBER;
		std::vector<pool> result;
		std::sort(ret.begin(), ret.end(), complare);
		int num = 0;
		for (auto r : ret) {
			if (r.e > max)
				exit(-1);
			auto n_t = int(r.e / range) - 1;
			if (num != n_t) {
				num = n_t;
				result.push_back(r);
			}
		}
		ret.assign(result.begin(), result.end());
	}
	/*全域搜索，要求基金数与入选基金数的组合总数不大于1kw*/
	void GlobalSearch(std::vector<std::vector<double>> rate) {
		
		EV ev;
		RD rd;
		std::vector<double> weight(SHORTLISTED_FUND_NUMBER);
		std::vector<int> temp(FUND_NUMBER);
		std::vector<std::vector<int>> result;
		
		combination(FUND_NUMBER, SHORTLISTED_FUND_NUMBER, temp, SHORTLISTED_FUND_NUMBER, result);
		auto w_temp = rd.arrRdCC(EFFECTIVE_DIGHITS, SHORTLISTED_FUND_NUMBER);
		for (int i = 0; i < SHORTLISTED_FUND_NUMBER; i++) 
			weight[i] = (double)w_temp[i] / EFFECTIVE_DIGHITS;

		ev.init(rate, weight, result[0]);
		auto num = result.size();
		for (int step = 1; step < num; step++) {
			auto position = ev.getPosition();
			for (int i = SHORTLISTED_FUND_NUMBER -1; i > 0; i--) {
				for (int j = i - 1; j > -1; j--) {
					//求导去最值
					//ev.derivation(position[i], position[j]);
					//迭代
					ev.optimum(position[i], position[j]);
					pool pool_temp(ev);
					ret.push_back(pool_temp);
				}
			}
			if (ret.size() > 100000)
				CDS(ev);
			auto w_temp = rd.arrRdCC(EFFECTIVE_DIGHITS, SHORTLISTED_FUND_NUMBER);
			for (int i = 0; i < SHORTLISTED_FUND_NUMBER; i++)
				weight[i] = (double)w_temp[i] / EFFECTIVE_DIGHITS;
			ev.update(weight, result[step]);
		}
		CDS(ev);
	}
	/*随机搜索*/
	void RandomSearch(std::vector<std::vector<double>> rate) {
		auto BS = [=](std::vector<int> arr, int key) {
			int r = (int)arr.size() - 1, l = 0, mid;
			while (l <= r) {
				mid = (r + l) / 2;
				if (key < arr[mid])
					r = mid - 1;
				else if (key > arr[mid])
					l = mid + 1;
				else
					return true;
			}
			return false;
		};
		
		EV ev;
		RD rd;
		std::vector<double> weight(SHORTLISTED_FUND_NUMBER);
		std::vector<int> position(SHORTLISTED_FUND_NUMBER);

		auto p_temp = rd.arrRd(FUND_NUMBER);
		auto w_temp = rd.arrRdCC(EFFECTIVE_DIGHITS, SHORTLISTED_FUND_NUMBER);
		for (int i = 0; i < SHORTLISTED_FUND_NUMBER; i++) {
			position[i] = p_temp[i];
			weight[i] = (double)w_temp[i] / EFFECTIVE_DIGHITS;
		}
		ev.init(rate, weight, position);

		for (int n = 0; n < LIMIT_MAX; n++) {
			auto p = ev.getPosition();
			for (int i = SHORTLISTED_FUND_NUMBER -1; i > 0; i--) {
				for (int j = i - 1; j > -1; j--) {
					//求导去最值
					//ev.derivation(p[i], p[j]);
					//迭代
					ev.optimum(p[i], p[j]);
					pool pool_temp(ev);
					ret.push_back(pool_temp);
				}
			}
			if (ret.size() > 100000)
				CDS(ev);
			while (true) {
				auto cp_temp = rd.Rd(FUND_NUMBER);
				if (!BS(p, cp_temp)) {
					auto p_temp = rd.arrRd(SHORTLISTED_FUND_NUMBER)[0];
					ev.update(ev.weight[p[p_temp]], cp_temp, p[p_temp]);
					break;
				}
			}
		}
		CDS(ev);
	}

	double SharpeRatio(pool ev) {
		double rf = 4.7945205479452054794520547945205e-5;
		return (ev.e - rf) / std::sqrt(ev.v) * 365;
	}

	void operatoin(std::vector<std::vector<double>> rate) {
		auto factorial = [=](int  n, int m) {
			size_t r = 1;
			for (size_t i = n; i > n - m; i--)
				r *= i / (n - i + 1);
			return r;
		};

		auto num = factorial(FUND_NUMBER, SHORTLISTED_FUND_NUMBER);
		if (num > 10000000)
			this->RandomSearch(rate);
		else
			this->GlobalSearch(rate);
	}
public:
	std::vector<pool> getAllData(std::vector<std::vector<double>> rate) {
		auto complare = [=](pool a, pool b) {
			return a.e > b.e;
		};
		
		operatoin(rate);
		std::sort(this->ret.begin(), this->ret.end(), complare);
		return this->ret;
	}

	pool getSpRtData(std::vector<std::vector<double>> rate , std::vector<double>retracement) {
		auto cprt = [=](std::vector<double>rt, std::vector<int>p, std::vector<double>w) {
			double r = 0;
			for (int i = 0; i < p.size(); i++) {
				r += w[i] * rt[p[i]];
			}
			return r;
		};
		operatoin(rate);

		pool result = this->ret[0];
		double sp = 0;
		for (auto r : this->ret) {
			if (cprt(retracement, r.p, r.w) < RETRACEMENT_MAX) {
				auto sp_t = SharpeRatio(r);
				if (sp_t > sp) {
					sp = sp_t;
					result = r;
				}
			}
		}
		return result;
	}
};
