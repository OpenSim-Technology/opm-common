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

#define BOOST_TEST_MODULE UnitTests
#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/Units/UnitSystemMap.hpp>
#include <opm/parser/eclipse/Units/UnitSystem.hpp>
#include <opm/parser/eclipse/Units/Dimension.hpp>
#include <opm/parser/eclipse/Units/ConversionFactors.hpp>



using namespace Opm;

BOOST_AUTO_TEST_CASE(CreateDimension) {
    Dimension length("L" , 1);
    BOOST_CHECK_EQUAL("L" , length.getName());
    BOOST_CHECK_EQUAL(1 , length.getSIScaling());
}

BOOST_AUTO_TEST_CASE(makeComposite) {
    Dimension composite = Dimension::makeComposite("L*L*L/t" , 100);
    BOOST_CHECK_EQUAL("L*L*L/t" , composite.getName());
    BOOST_CHECK_EQUAL(100 , composite.getSIScaling());
}


BOOST_AUTO_TEST_CASE(CreateDimensionInvalidNameThrows) {
    BOOST_CHECK_THROW(Dimension(" " , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension(".LX" , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension("*" , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension("/" , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension("2" , 1) , std::invalid_argument);
    BOOST_CHECK_NO_THROW(Dimension("1" , 1));
}


BOOST_AUTO_TEST_CASE(CreateUnitSystem) {
    UnitSystem system("Metric");
    BOOST_CHECK_EQUAL("Metric" , system.getName());
}


BOOST_AUTO_TEST_CASE(UnitSystemEmptyHasNone) {
    UnitSystem system("Metric");
    BOOST_CHECK_EQUAL( false , system.hasDimension("L"));
    BOOST_CHECK_EQUAL( false , system.hasDimension("LXY"));
}



BOOST_AUTO_TEST_CASE(UnitSystemGetMissingDimensionThrows) {
    UnitSystem system("Metric");
    BOOST_CHECK_THROW( system.getDimension("L") , std::invalid_argument );
}


BOOST_AUTO_TEST_CASE(UnitSystemAddDimensions) {
    UnitSystem system("Metric");
    system.addDimension("L" , 1 );
    system.addDimension("t" , 86400 );

    Dimension length = system.getDimension("L");
    Dimension time = system.getDimension("t");
    BOOST_CHECK_EQUAL(1     , length.getSIScaling());
    BOOST_CHECK_EQUAL(86400 , time.getSIScaling());

    system.addDimension("L" , 0.3048);
    length = system.getDimension("L");
    BOOST_CHECK_EQUAL(0.3048 , length.getSIScaling());
}


BOOST_AUTO_TEST_CASE(UnitSystemParseInvalidThrows) {
    UnitSystem system("Metric");
    BOOST_CHECK_THROW( system.parse("//") , std::invalid_argument);
    BOOST_CHECK_THROW( system.parse("L * L / t") , std::invalid_argument);

    system.addDimension("L" , 3.00 );
    system.addDimension("t" , 9.0 );
    
    Dimension volumePerTime = system.parse("L*L*L/t");
    BOOST_CHECK_EQUAL("L*L*L/t" , volumePerTime.getName() );
    BOOST_CHECK_EQUAL(3.0 , volumePerTime.getSIScaling());
}    



void checkSystemHasRequiredDimensions(std::shared_ptr<const UnitSystem> system) {
    BOOST_CHECK( system->hasDimension("1"));
    BOOST_CHECK( system->hasDimension("L"));
    BOOST_CHECK( system->hasDimension("m"));
    BOOST_CHECK( system->hasDimension("t"));
    BOOST_CHECK( system->hasDimension("K"));
    BOOST_CHECK( system->hasDimension("P"));
}



BOOST_AUTO_TEST_CASE(CreateMetricSystem) {
    std::shared_ptr<UnitSystem> system = std::shared_ptr<UnitSystem>( UnitSystem::newMETRIC() );
    checkSystemHasRequiredDimensions( system );

    BOOST_CHECK_EQUAL( Metric::Length       , system->getDimension("L").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Mass         , system->getDimension("m").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Time         , system->getDimension("t").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Permeability , system->getDimension("K").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Pressure     , system->getDimension("P").getSIScaling() );
}



BOOST_AUTO_TEST_CASE(CreateFieldSystem) {
    std::shared_ptr<UnitSystem> system = std::shared_ptr<UnitSystem>( UnitSystem::newFIELD() );
    checkSystemHasRequiredDimensions( system );

    BOOST_CHECK_EQUAL( Field::Length       , system->getDimension("L").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Mass         , system->getDimension("m").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Time         , system->getDimension("t").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Permeability , system->getDimension("K").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Pressure     , system->getDimension("P").getSIScaling() );
}



BOOST_AUTO_TEST_CASE(CreateUnitMap) {
    UnitSystemMap systemMap; 
    systemMap.addSystem( std::shared_ptr<UnitSystem>( UnitSystem::newMETRIC() ));
    systemMap.addSystem( std::shared_ptr<UnitSystem>( UnitSystem::newFIELD() ));

    BOOST_CHECK( systemMap.hasSystem("METRIC"));
    BOOST_CHECK( systemMap.hasSystem("meTRIC"));
    BOOST_CHECK( systemMap.hasSystem("meTRic"));
    BOOST_CHECK( systemMap.hasSystem("Field"));
    
    BOOST_CHECK_EQUAL( false , systemMap.hasSystem("NoNotThisOne"));
    BOOST_CHECK_THROW( systemMap.getSystem( "NoNotThisOne") , std::invalid_argument);
}


