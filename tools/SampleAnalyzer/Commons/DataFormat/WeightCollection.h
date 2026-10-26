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

/**
 * @file WeightCollection.h
 * @brief Collection of the weights of an event (multiweight support).
 */

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
    /**
     * @brief Weights of an event, indexed from 0 (index 0 is the nominal weight).
     *
     * Arithmetic operators act element-wise on all the weights.
     */
    class WeightCollection
    {

        // -------------------------------------------------------------
        //                        data members
        // -------------------------------------------------------------
    private:
        /** @brief Weights. */
        std::vector<MAfloat64> weights_;
        /** @brief Value returned for an undefined weight (0). */
        static const MAfloat64 emptyvalue_;

        // -------------------------------------------------------------
        //                      method members
        // -------------------------------------------------------------
    public:
        /** @brief Constructor (empty collection). */
        WeightCollection() {}

        /**
         * @brief Copy constructor.
         *
         * @param rhs collection to copy.
         */
        WeightCollection(const WeightCollection &rhs)
        {
            weights_.clear();
            weights_ = rhs.weights_;
        }

        /**
         * @brief Constructor with a given number of weights.
         *
         * @param size number of weights.
         * @param default_value initial value of each weight.
         */
        explicit WeightCollection(const MAuint32 &size, MAdouble64 default_value = 0.0) : weights_(size, default_value) {}

        /** @brief Destructor. */
        ~WeightCollection() {}

        /** @brief Remove all the weights. */
        void Reset() { weights_.clear(); }
        /** @brief Remove all the weights (alias of Reset()). */
        void clear() { Reset(); }

        /**
         * @brief Number of weights.
         *
         * @return the number of weights.
         */
        MAuint32 size() const { return weights_.size(); }

        /**
         * @brief Number of weights.
         *
         * @return the number of weights.
         */
        MAuint32 size() { return weights_.size(); }

        /**
         * @brief Change the number of weights.
         *
         * @param n new size.
         */
        void resize(MAuint32 n) { weights_.resize(n); }

        /**
         * @brief Set an existing weight.
         *
         * @param id index of the weight.
         * @param value weight.
         * @return false (with an error) if the index does not exist.
         */
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
                    // NOTE: the message ('A null value is returned') is copied from Get().
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

        /**
         * @brief Accessor to all the weights.
         *
         * @return the weights.
         */
        const std::vector<MAfloat64> &GetWeights() const { return weights_; }

        /**
         * @brief Accessor to all the weights.
         *
         * @return the weights.
         */
        const std::vector<MAfloat64> &values() const { return weights_; }

        /**
         * @brief Accessor to one weight.
         *
         * @param id index.
         * @return the weight, or 0 (with an error) if the index does not exist.
         */
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

        /**
         * @brief Accessor to one weight.
         *
         * @param id index.
         * @return the weight (see Get()).
         */
        const MAfloat64 &operator[](MAuint32 id) const { return Get(id); }

        /** @brief Print the weights. */
        void Print() const
        {
            if (!weights_.empty())
                for (MAuint32 i = 0; i < size(); i++)
                    INFO << "ID=" << i << " : " << weights_[i] << endmsg;
        }

        /**
         * @brief Add a value to one weight (no bound check).
         *
         * @param idx index.
         * @param weight value to add.
         */
        void add_weight_to(MAint32 idx, MAdouble64 weight) { weights_[idx] += weight; }

        /**
         * @brief Replace all the weights.
         *
         * @param v new weights.
         */
        void SetWeights(const std::vector<MAfloat64> &v) { weights_ = v; }

        /**
         * @brief Multiply all the weights.
         *
         * @param multiple factor.
         * @return this collection.
         */
        WeightCollection &operator*=(const MAdouble64 &multiple)
        {
            for (auto &x : weights_)
                x *= multiple;
            return *this;
        }

        /**
         * @brief Multiply all the weights.
         *
         * @param multiple factor.
         * @return a scaled copy.
         */
        WeightCollection operator*(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x *= multiple;
            return result;
        }

        /**
         * @brief Divide all the weights.
         *
         * @param multiple divisor.
         * @return a scaled copy.
         */
        WeightCollection operator/(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x /= multiple;
            return result;
        }

        /**
         * @brief Add a value to all the weights.
         *
         * @param multiple value.
         * @return a shifted copy.
         */
        WeightCollection operator+(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x += multiple;
            return result;
        }

        /**
         * @brief Subtract a value from all the weights.
         *
         * @param multiple value.
         * @return a shifted copy.
         */
        WeightCollection operator-(const MAdouble64 &multiple) const
        {
            WeightCollection result(*this); // copy
            for (auto &x : result.weights_)
                x -= multiple;
            return result;
        }

        /**
         * @brief Add a value to all the weights.
         *
         * @param input value.
         * @return this collection.
         */
        WeightCollection &operator+=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x += input;
            return *this;
        }

        /**
         * @brief Add a vector of values element-wise.
         *
         * @param input values (same size as the collection).
         * @return this collection.
         */
        WeightCollection &operator+=(const std::vector<MAdouble64> &input)
        {
            if (size() != input.size())
                throw std::invalid_argument("Size mismatch in WeightCollection::operator+= (weights_ and input must have the same size)");
            std::transform(weights_.begin(), weights_.end(), input.begin(), weights_.begin(),
                           [](MAdouble64 a, MAdouble64 b)
                           { return a + b; });
            return *this;
        }

        /**
         * @brief Subtract a value from all the weights.
         *
         * @param input value.
         * @return this collection.
         */
        WeightCollection &operator-=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x -= input;
            return *this;
        }

        /**
         * @brief Divide all the weights.
         *
         * @param input divisor.
         * @return this collection.
         */
        WeightCollection &operator/=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x /= input;
            return *this;
        }

        /**
         * @brief Set all the weights to a value.
         *
         * @param input value.
         * @return this collection.
         */
        WeightCollection &operator=(const MAfloat64 &input)
        {
            for (auto &x : weights_)
                x = input;
            return *this;
        }

        /**
         * @brief Copy assignment.
         *
         * @param w collection to copy.
         * @return this collection.
         */
        WeightCollection &operator=(const WeightCollection &w)
        {
            if (this == &w)
                return *this;
            weights_ = w.weights_;
            return *this;
        }
    };

    /**
     * @brief Multiply all the weights (scalar on the left).
     *
     * @param multiple factor.
     * @param w collection.
     * @return a scaled copy.
     */
    inline WeightCollection operator*(const MAdouble64 &multiple, const WeightCollection &w)
    {
        return w * multiple;
    }

    /**
     * @brief Scalar divided by a collection (see the FIXME).
     *
     * @param multiple scalar.
     * @param w collection.
     * @return w / multiple.
     */
    inline WeightCollection operator/(const MAdouble64 &multiple, const WeightCollection &w)
    {
        // FIXME: returns w / multiple instead of multiple / w (element-wise).
        return w / multiple;
    }

    /**
     * @brief Add a value to all the weights (scalar on the left).
     *
     * @param multiple value.
     * @param w collection.
     * @return a shifted copy.
     */
    inline WeightCollection operator+(const MAdouble64 &multiple, const WeightCollection &w)
    {
        return w + multiple;
    }

    /**
     * @brief Scalar minus a collection (see the FIXME).
     *
     * @param multiple scalar.
     * @param w collection.
     * @return w - multiple.
     */
    inline WeightCollection operator-(const MAdouble64 &multiple, const WeightCollection &w)
    {
        // FIXME: returns w - multiple instead of multiple - w (element-wise).
        return w - multiple;
    }
}

#endif
