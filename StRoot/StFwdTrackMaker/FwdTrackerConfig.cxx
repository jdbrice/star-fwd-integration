#include "FwdTrackerConfig.h"

const std::string FwdTrackerConfig::valDNE = std::string( "<DNE/>" );
const std::string FwdTrackerConfig::pathDelim = std::string( "." );
const std::string FwdTrackerConfig::attrDelim = std::string( ":" );

// template specializations
template <>
std::string FwdTrackerConfig::get( std::string path, std::string dv) {
    FwdTrackerConfig::canonize( path );
   if ( false == exists( path ) )
       return dv;
   return this->nodes[ path ];
}

// specialization for bool adds recognition of strings "true" and "false" (lower case)
template <>
bool FwdTrackerConfig::get( std::string path, bool dv ){
    FwdTrackerConfig::canonize( path );
    if ( false == exists( path ) )
        return dv;

    if ( this->nodes[path] == "false" )
       return false;

    if ( this->nodes[path] == "true" )
       return true;

    return static_cast<bool>(get<int>( path, 0 ));
}

// get as ROOT TString
template <>
TString FwdTrackerConfig::get<TString>(std::string path, TString dv) {
    FwdTrackerConfig::canonize( path );
    if (false == exists(path))
        return dv;
    TString r(get<std::string>(path));
    return r;
}