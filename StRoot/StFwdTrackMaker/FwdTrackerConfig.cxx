#include "FwdTrackerConfig.h"

const std::string FwdTrackerConfig::valDNE = std::string( "<DNE/>" );
const std::string FwdTrackerConfig::pathDelim = std::string( "." );
const std::string FwdTrackerConfig::attrDelim = std::string( ":" );

// template specializations
template <>
std::string FwdTrackerConfig::get( std::string path, std::string dv )  {
    // return default value if path DNE
    if ( false == exists( path ) )
        return dv;
    FwdTrackerConfig::canonize( path );
    // directly return string
    return ( this->nodes[ path ] );
}

template <>
std::string FwdTrackerConfig::convert( std::string str ) {
   return str;
}

// specialization for bool adds recognition of strings "true" and "false" (lower case)
template <>
bool FwdTrackerConfig::convert( std::string str ){

    if ( str == "false" )
       return false;

    if ( str == "true" )
       return true;
    // fall back to an int cast
    return static_cast<bool>(convert<int>( str ));
}

// get as ROOT TString
template <>
TString FwdTrackerConfig::convert(std::string str) {
    TString r(str);
    return r;
}