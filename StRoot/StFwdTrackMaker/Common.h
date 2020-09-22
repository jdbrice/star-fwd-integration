#ifndef FWD_TRACK_MAKER_COMMON_H
#define FWD_TRACK_MAKER_COMMON_H

class FwdTrackerUtils {
    static void labelAxis( TAxis *_x, vector<string> _labels )
    {
       if ( nullptr == _x ) return;

       for ( unsigned int i = 0; i <= static_cast<unsigned int>(_x->GetNbins()); i++ ) {
          if ( i < _labels.size() )
             _x->SetBinLabel( i + 1, _labels[i].c_str() );
       }
    }
};

#endif