/*
  Copyright 2013 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/ 


#include <iostream>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include <opm/parser/eclipse/Units/ConversionFactors.hpp>
#include <opm/parser/eclipse/Units/UnitSystem.hpp>
#include <vector>


namespace Opm {

    UnitSystem::UnitSystem(const std::string& unitSystem) : 
        m_name( unitSystem )
    {
        
    }

    bool UnitSystem::hasDimension(const std::string& dimension) const {
        return (m_dimensions.find( dimension ) != m_dimensions.end());
    }

    
    const Dimension& UnitSystem::getDimension(const std::string& dimension) const {
        if (hasDimension( dimension ))
            return m_dimensions.at( dimension );
        else
            throw std::invalid_argument("Dimension: " + dimension + " not recognized ");
    }


    void UnitSystem::addDimension(const std::string& dimension , double SI_factor) {
        if (hasDimension(dimension))
            m_dimensions.erase( dimension );
        
        m_dimensions.insert( std::make_pair(dimension , Dimension(dimension , SI_factor)));
    }


    const std::string& UnitSystem::getName() const {
        return m_name;
    }


    Dimension UnitSystem::parseFactor(const std::string& dimension) const {
        std::vector<std::string> dimensionList;
        boost::split(dimensionList , dimension , boost::is_any_of("*"));
        double SIfactor = 1.0;
        for (auto iter = dimensionList.begin(); iter != dimensionList.end(); ++iter) {
            Dimension dim = getDimension( *iter );
            SIfactor *= dim.getSIScaling();
        }
        return Dimension::makeComposite( dimension , SIfactor );
    }
    


    Dimension UnitSystem::parse(const std::string& dimension) const {
        bool haveDivisor;
        {
            size_t divCount = std::count( dimension.begin() , dimension.end() , '/' );
            if (divCount == 0)
                haveDivisor = false;
            else if (divCount == 1)
                haveDivisor = true;
            else
                throw std::invalid_argument("Dimension string can only have one division sign /");
        }

        if (haveDivisor) {
            std::vector<std::string> parts;
            boost::split(parts , dimension , boost::is_any_of("/"));
            Dimension dividend = parseFactor( parts[0] );
            Dimension divisor = parseFactor( parts[1] );
        
            return Dimension::makeComposite( dimension , dividend.getSIScaling() / divisor.getSIScaling() );
        } else {
            return parseFactor( dimension );
        }
    }


    UnitSystem * UnitSystem::newMETRIC() {
        UnitSystem * system = new UnitSystem("Metric");
        
        system->addDimension("1" , 1.0);
        system->addDimension("P" , Metric::Pressure );
        system->addDimension("L" , Metric::Length);
        system->addDimension("t" , Metric::Time );
        system->addDimension("m" , Metric::Mass );
        system->addDimension("K" , Metric::Permeability );
        
        return system;
    }


    
    UnitSystem * UnitSystem::newFIELD() {
        UnitSystem * system = new UnitSystem("Field");
        
        system->addDimension("1" , 1.0);
        system->addDimension("P" , Field::Pressure );
        system->addDimension("L" , Field::Length);
        system->addDimension("t" , Field::Time);
        system->addDimension("m" , Field::Mass);
        system->addDimension("K" , Field::Permeability );
        
        return system;
    }

}



