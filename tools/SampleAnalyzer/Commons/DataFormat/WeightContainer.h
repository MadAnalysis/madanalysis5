#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <numeric>

#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

namespace MDUtils {
	std::pair<MAfloat64,MAfloat64> CombineWithDistribution(std::vector<MAfloat64> &weight_values, std::string method){

		if(method == "gaussian"){
			MAfloat64 sum = std::accumulate(weight_values.begin(), weight_values.end(), 0);
			MAfloat64 mean = sum/weight_values.size();
			MAfloat64 squarediff = 0;
			for(const auto &val : weight_values){
				squarediff += pow(val-mean, 2);
			}
			return std::make_pair(mean, sqrt(squarediff/weight_values.size()));

		}	
	}
}


namespace MA5
{

class WeightContainer {
	private:
		std::unordered_map<MAuint32, MAfloat64> weights;

	public:

		WeightContainer(){}
		~WeightContainer(){}

		void Reset(){
			weights.clear();
		}

		MAbool Add(MAuint32 id, MAfloat64 value){
			weights[id] = value;
		}

		const std::unordered_map<MAuint32, MAfloat64>& GetWeights() {return weights;}

		const MAfloat64& GetWeight(const MAuint32 id) const {
			if(weights.find(id) != weights.end()){
				return weights[id];
			}
			else {
				throw EXCEPTION_ERROR("The Weight '" + to_string(id) + "' is not defined. Return null value.", "", 0);
			}
		}

		const MAfloat64& operator[](const MAuint32 id) const {return GetWeight[id];}

		WeightContainer& operator*(MAfloat64 multiple){
			for(auto &[id, value] : weights){
				value *= multiple;
			}
		}

		void Print() const{
			for(const auto &[id, value] : weights){
				INFO << "ID=" << id << " : " << value << endmsg;
			}
		}

		std::pair<MAfloat64, MAfloat64> CombineWeights(const std::string method){
			vector<MAfloat64> weight_values;
			for(const auto &[id, value] : weights){
				weight_values.push_back(value);
			}
			
			return weight_values.size()>0?MDUtils::CombineWithDistribution(method):std::make_pair(0,0);
		}

};

}


