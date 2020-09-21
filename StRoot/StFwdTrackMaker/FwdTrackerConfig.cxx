#include "FwdTrackerConfig.h"
#include "TString.h"

// template specializations
template <>
string FwdTrackerConfig::get( string path, string dv) {
   if ( false == exists( path ) )
       return dv;
   return nodes[ path ];
}

// specialization for bool adds recognition of strings "true" and "false" (lower case)
template <>
bool FwdTrackerConfig::get( string path, bool dv ){
    if ( false == exists( path ) )
        return dv;

    if ( nodes[path] == "false" )
       return false;

    if ( nodes[path] == "true" )
       return true;

    return static_cast<bool>(get<int>( path, 0 ));
}

// get as ROOT TString
template <>
TString FwdTrackerConfig::get<TString>(string path, TString dv) {
    if (false == exists(path))
        return dv;
    TString r(get<std::string>(path));
    return r;
}