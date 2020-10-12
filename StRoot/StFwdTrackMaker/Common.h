#ifndef FWD_TRACK_MAKER_COMMON_H
#define FWD_TRACK_MAKER_COMMON_H

#include "TAxis.h"
#include <vector>
#include <string>


class FwdTrackerUtils {
    public:
    static void labelAxis( TAxis *axis, std::vector<std::string> labels )
    {
       if ( !axis ) return;

       for ( int i = 0; i <= axis->GetNbins(); i++ ) {
          if ( i < labels.size() )
             axis->SetBinLabel( i + 1, labels[i].c_str() );
       }
    }
};

#endif