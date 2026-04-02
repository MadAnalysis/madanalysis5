////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2026 Jack Araz, Eric Conte & Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//
//  This file is part of MadAnalysis 5.
//  Official website: <https://github.com/MadAnalysis/madanalysis5>
//
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//
////////////////////////////////////////////////////////////////////////////////

#ifndef WEIGHT_COLLECTION_H
#define WEIGHT_COLLECTION_H

// STL headers
#include <map>
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>

// SampleAnalyzer headers
#include "SampleAnalyzer/Commons/Service/LogService.h"
#include "SampleAnalyzer/Commons/Service/ExceptionService.h"

namespace MA5
{
    class WeightCollection
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        std::vector<MAfloat64> weights_;
        static const MAfloat64 emptyvalue_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /// Constructor withtout arguments
        WeightCollection() {}

        // copy constructor
        WeightCollection(const WeightCollection &rhs)
        {
            weights_.clear();
            weights_ = rhs.weights_;
        }

        /// @brief Initialise weights with a certain size and default value
        /// @param size number of weights
        /// @param default_value default value for each weight
        WeightCollection(const MAuint32 &size, MAdouble64 default_value = 0.0) : weights_(size, default_value) {}

        /// Destructor
        ~WeightCollection() {}

        /// Clear all the content
        void Reset() { weights_.clear(); }
        void clear() { Reset(); }

        /// Size
        MAuint32 size() const { return weights_.size(); }

        /// Size
        MAuint32 size() { return weights_.size(); }

        void resize(MAuint32 n) { weights_.resize(n); }

        /// Add a new weight group
        MAbool Add(MAuint32 id, MAfloat64 value)
        {
            if (id < size())
            {
                weights_.at(id) = value;
                return true;
            }
            else
            {
                try
                {
                    std::stringstream str;
                    str << id;
                    std::string idname;
                    str >> idname;
                    throw EXCEPTION_ERROR("The Weight '" + idname +
                                              "' is not defined. A null value is returned.",
                                          "", 0);
                }
                catch (const std::exception &e)
                {
                    MANAGE_EXCEPTION(e);
                    return false;
                }
            }
        }

        /// Get all the Weight Collection
        const std::vector<MAfloat64> &GetWeights() const { return weights_; }

        /// Get all the Weights
        const std::vector<MAfloat64> &values() const { return weights_; }

        /// Get a weight
        const MAfloat64 &Get(MAuint32 id) const
        {
            if (id >= 0 && id < size())
                return weights_[id];

            try
            {
                std::stringstream str;
                str << id;
                std::string idname;
                str >> idname;
                throw EXCEPTION_ERROR("The Weight '" + idname +
                                          "' is not defined. A null value is returned.",
                                      "", 0);
            }
            catch (const std::exception &e)
            {
                MANAGE_EXCEPTION(e);
                return emptyvalue_;
            }
        }

        /// Get a weight
        const MAfloat64 &operator[](MAuint32 id) const { return Get(id); }

        /// @brief Print weight information
        void Print() const
        {
            if (!weights_.empty())
                for (MAuint32 i = 0; i < size(); i++)
                    INFO << "ID=" << i << " : " << weights_[i] << endmsg;
        }

        /// @brief add weight to specific location
        /// @param idx location
        /// @param weight weight value
        void add_weight_to(MAint32 idx, MAdouble64 weight) { weights_[idx] += weight; }

        // explicit setter from same-typed vector (if you need it)
        void SetWeights(const std::vector<MAfloat64> &v) { weights_ = v; }

        /// @brief multiply operator
        /// @param multiple
        WeightCollection &operator*=(const MAdouble64 &multiple)
        {
            for (auto &x : weights_)
                x *= multiple;
            return *this;
        }

        WeightCollection operator*(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x *= multiple;
            return result;
        }

        WeightCollection operator/(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x /= multiple;
            return result;
        }

        WeightCollection operator+(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x += multiple;
            return result;
        }

        WeightCollection operator-(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x -= multiple;
            return result;
        }

        /// @brief add operator
        /// @param input
        WeightCollection &operator+=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x += input;
            return *this;
        }

        /// @brief add operator
        /// @param input
        WeightCollection &operator+=(const std::vector<MAdouble64> &input)
        {
            if (size() != input.size())
                throw std::invalid_argument("Size mismatch in WeightCollection::operator+= (weights_ and input must have the same size)");
            std::transform(weights_.begin(), weights_.end(), input.begin(), weights_.begin(),
                           [](MAdouble64 a, MAdouble64 b)
                           { return a + b; });
            return *this;
        }

        /// @brief subtract operator
        /// @param input
        WeightCollection &operator-=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x -= input;
            return *this;
        }

        /// @brief divide operator
        /// @param input
        WeightCollection &operator/=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x /= input;
            return *this;
        }

        /// @brief assignment operator
        /// @param input
        WeightCollection &operator=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x = input;
            return *this;
        }

        /// @brief assignment operator
        /// @param input
        WeightCollection &operator=(const WeightCollection &w)
        {
            if (this == &w)
                return *this;
            weights_ = w.weights_;
            return *this;
        }
    };

    inline WeightCollection operator*(const MAdouble64 &multiple, const WeightCollection &w)
    {
        return w * multiple;
    }

    inline WeightCollection operator/(const MAdouble64 &multiple, const WeightCollection &w)
    {
        return w / multiple;
    }

    inline WeightCollection operator+(const MAdouble64 &multiple, const WeightCollection &w)
    {
        return w + multiple;
    }

    inline WeightCollection operator-(const MAdouble64 &multiple, const WeightCollection &w)
    {
        return w - multiple;
    }
}

#endif
