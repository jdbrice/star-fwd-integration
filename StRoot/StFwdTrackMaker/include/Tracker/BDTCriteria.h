#ifndef BDT_Criteria_h
#define BDT_Criteria_h

#include "TMVA/Reader.h"
#include "Criteria/ICriterion.h"


/** Criterion: the difference of the sqrt(x^2 + y^2) of two hits.
*/
class BDTCrit2 : public KiTrack::ICriterion{

public:


  
    BDTCrit2 ( float scoreMin , float scoreMax, std::string wfile ){
        _scoreMax = scoreMax;  
        _scoreMin = scoreMin;

        _name = "Crit2_BDT";
        _type = "2Hit";
        weightFile = wfile;

        _saveValues = false;

        
    }


    float EvalDeltaPhi( KiTrack::IHit*a, KiTrack::IHit*b ){
        // TODO: work on branchless version?
        float ax = a->getX();
        float ay = a->getY();
        float bx = b->getX();
        float by = b->getY();

        float phia = atan2( ay, ax );
        float phib = atan2( by, bx );
        float deltaPhi = phia - phib;

        if (deltaPhi > M_PI) deltaPhi -= 2*M_PI;           //to the range from -pi to pi
        if (deltaPhi < -M_PI) deltaPhi += 2*M_PI;           //to the range from -pi to pi
        
        if (( by*by + bx*bx < 0.0001 )||( ay*ay + ax*ax < 0.0001 )) deltaPhi = 0.; // In case one of the hits is too close to the origin

        deltaPhi = 180.*fabs( deltaPhi ) / M_PI;
        return deltaPhi;
    }
    float EvalDeltaRho( KiTrack::IHit*a, KiTrack::IHit*b ){
        float ax = a->getX();
        float ay = a->getY();
        float bx = b->getX();
        float by = b->getY();

        float rhoA =  sqrt( ax*ax + ay*ay );
        float rhoB =  sqrt( bx*bx + by*by );

        float deltaRho = rhoA - rhoB;
        return deltaRho;
    }

    float EvalRZRatio( KiTrack::IHit*a, KiTrack::IHit*b ){
        float ax = a->getX();
        float ay = a->getY();
        float az = a->getZ();

        float bx = b->getX();
        float by = b->getY();
        float bz = b->getZ();

        // the square is used, because it is faster to calculate with the squares than with sqrt, which takes some time!
        double ratioSquared = 0.; 
        if ( az-bz  != 0. ) 
            ratioSquared = ( (ax-bx)*(ax-bx) + (ay-by)*(ay-by) + (az-bz)*(az-bz) ) / ( (az-bz) * ( az-bz ) );

        return sqrt(ratioSquared);
    }

    float EvalStraightTrackRatio( KiTrack::IHit*a, KiTrack::IHit*b ){
        float ax = a->getX();
        float ay = a->getY();
        float az = a->getZ();

        float bx = b->getX();
        float by = b->getY();
        float bz = b->getZ();

        //the distance to (0,0) in the xy plane
        double rhoASquared = ax*ax + ay*ay;
        double rhoBSquared = bx*bx + by*by;

        double ratioSquared = 0;
        if( (rhoBSquared >0.) && ( az != 0. ) ){ //prevent division by 0
            // the square is used, because it is faster to calculate with the squares than with sqrt, which takes some time!
            ratioSquared = ( ( rhoASquared * ( bz*bz )  ) / ( rhoBSquared * ( az*az )  ) );
        }

        return sqrt( ratioSquared );
    }
  
  virtual bool areCompatible( KiTrack::Segment* parent , KiTrack::Segment* child ){


    if ( reader == nullptr ){
        BDTCrit2::reader = new TMVA::Reader("!Color:!Silent");

        // setup the inputs
        BDTCrit2::reader->AddVariable("Crit2_RZRatio", &BDTCrit2::Crit2_RZRatio);
        BDTCrit2::reader->AddVariable("Crit2_DeltaRho", &BDTCrit2::Crit2_DeltaRho);
        BDTCrit2::reader->AddVariable("Crit2_DeltaPhi", &BDTCrit2::Crit2_DeltaPhi);
        BDTCrit2::reader->AddVariable("Crit2_StraightTrackRatio", &BDTCrit2::Crit2_StraightTrackRatio);

        BDTCrit2::reader->BookMVA("BDT method", weightFile.c_str());
    }

    if (( parent->getHits().size() == 1 )&&( child->getHits().size() == 1 )){
    } //a criterion for 1-segments
    else {
        std::stringstream s;
        s << "Crit2_BDT::This criterion needs 2 segments with 1 hit each, passed was a "
        <<  parent->getHits().size() << " hit segment (parent) and a "
        <<  child->getHits().size() << " hit segment (child).";

        throw KiTrack::BadSegmentLength( s.str() );
    }

    KiTrack::IHit* a = parent->getHits()[0];
    KiTrack::IHit* b = child-> getHits()[0];


    //first check, if the distance to (0,0) rises --> such a combo could not reach the IP
    

    // compute input values
    BDTCrit2::Crit2_DeltaPhi = EvalDeltaPhi( a, b );
    BDTCrit2::Crit2_DeltaRho = EvalDeltaRho( a, b );
    BDTCrit2::Crit2_RZRatio  = EvalRZRatio( a, b );
    BDTCrit2::Crit2_StraightTrackRatio  = EvalStraightTrackRatio( a, b );

    float score = BDTCrit2::reader->EvaluateMVA("BDT method");

    if (_saveValues){
      _map_name_value["Crit2_BDT"] = score;
      _map_name_value["Crit2_BDT_DeltaPhi"] = BDTCrit2::Crit2_DeltaPhi;
      _map_name_value["Crit2_BDT_DeltaRho"] = BDTCrit2::Crit2_DeltaRho;
      _map_name_value["Crit2_BDT_RZRatio"] = BDTCrit2::Crit2_RZRatio;
      _map_name_value["Crit2_BDT_StraightTrackRatio"] = BDTCrit2::Crit2_StraightTrackRatio;
    }

    if ( score < _scoreMin || score > _scoreMax ) return false;
    return true;
  }
  
  virtual ~BDTCrit2(){};
  

private:
  
  float _scoreMin{};
  float _scoreMax{};
  std::string weightFile;
  static TMVA::Reader *reader;
  // values input to BDT
  static float Crit2_RZRatio, Crit2_DeltaRho, Crit2_DeltaPhi, Crit2_StraightTrackRatio;
  
  
  
};




class BDTCrit3 : public KiTrack::ICriterion{

public:
  
    BDTCrit3 ( float scoreMin , float scoreMax, std::string wfile ){
        _scoreMax = scoreMax;  
        _scoreMin = scoreMin;

        _name = "Crit3_BDT";
        _type = "3Hit";
        weightFile = wfile;

        _saveValues = false;

        
    }


    float Eval3DAngle(KiTrack::IHit*a, KiTrack::IHit*b, KiTrack::IHit*c ){

        float ax = a->getX();
        float ay = a->getY();
        float az = a->getZ();

        float bx = b->getX();
        float by = b->getY();
        float bz = b->getZ();

        float cx = c->getX();
        float cy = c->getY();
        float cz = c->getZ();


        float ux = bx - ax;
        float uy = by - ay;
        float uz = bz - az;

        float vx = cx - bx;
        float vy = cy - by;
        float vz = cz - bz;

        //In the numerator there is the vector product of u and v   
        double numerator= ux*vx + uy*vy + uz*vz;

        //In the denominator there are the lengths of u and v (here squared)
        double uSquared= ux*ux + uy*uy + uz*uz;
        double vSquared= vx*vx + vy*vy + vz*vz;

        double denomSquared = uSquared * vSquared;

        if ( denomSquared > 0.){ //don't divide by 0

            double cosThetaSquared = numerator * numerator / ( uSquared * vSquared );
            if( cosThetaSquared > 1. ) cosThetaSquared = 1; // prevent rounding errors: cosTheta can mathematically never be bigger than 1!!!
         
            return acos( sqrt( cosThetaSquared ) ) * 180. / M_PI;
        }

        return 180;
    }

    float Eval2DAngle(KiTrack::IHit*a, KiTrack::IHit*b, KiTrack::IHit*c ){

        float ax = a->getX();
        float ay = a->getY();

        float bx = b->getX();
        float by = b->getY();

        float cx = c->getX();
        float cy = c->getY();


        float ux = bx - ax;
        float uy = by - ay;


        float vx = cx - bx;
        float vy = cy - by;


        //In the numerator there is the vector product of u and v   
        double numerator= ux*vx + uy*vy;

        //In the denominator there are the lengths of u and v (here squared)
        double uSquared= ux*ux + uy*uy;
        double vSquared= vx*vx + vy*vy;

        double denomSquared = uSquared * vSquared;

        if ( denomSquared > 0.){ //don't divide by 0
              
            double cosThetaSquared = numerator * numerator / ( uSquared * vSquared );
            if( cosThetaSquared > 1. ) cosThetaSquared = 1; // prevent rounding errors: cosTheta can mathematically never be bigger than 1!!!

            return acos( sqrt( cosThetaSquared ) ) * 180. / M_PI;

            // if (_saveValues){
            //     _map_name_value["Crit3_2DAngle_cos2DAngleSquared"] =  cosThetaSquared;
            //     _map_name_value["Crit3_2DAngle"] = acos( sqrt( cosThetaSquared ) ) * 180. / M_PI;
            // }

            // if (cosThetaSquared < _cosAngleMin*_cosAngleMin) return false;
            // if (cosThetaSquared > _cosAngleMax*_cosAngleMax) return false;

        }
        return 180;
    }

    float EvalChangeRZRatio(KiTrack::IHit*a, KiTrack::IHit*b, KiTrack::IHit*c ){

        float ax = a->getX();
        float ay = a->getY();
        float az = a->getZ();
        
        float bx = b->getX();
        float by = b->getY();
        float bz = b->getZ();
        
        float cx = c->getX();
        float cy = c->getY();
        float cz = c->getZ();

        // The rz ratios squared
        
        double ratioSquaredParent = 0.; 
        if ( az-bz  != 0. ) ratioSquaredParent = ( (ax-bx)*(ax-bx) + (ay-by)*(ay-by) + (az-bz)*(az-bz) ) / ( (az-bz) * ( az-bz ) );
        
        double ratioSquaredChild = 0.; 
        if ( cz-bz  != 0. ) ratioSquaredChild = ( (cx-bx)*(cx-bx) + (cy-by)*(cy-by) + (cz-bz)*(cz-bz) ) / ( (cz-bz) * ( cz-bz ) );

        double ratioOfRZRatioSquared = 0.;
        
        if (ratioSquaredChild != 0.) ratioOfRZRatioSquared = ratioSquaredParent / ratioSquaredChild;
        
        return sqrt( ratioOfRZRatioSquared );
    }


    virtual bool areCompatible( KiTrack::Segment* parent , KiTrack::Segment* child ){

        if ( reader == nullptr ){
            BDTCrit3::reader = new TMVA::Reader("!Color:!Silent");

            // setup the inputs
            
            BDTCrit3::reader->AddVariable("Crit3_3DAngle", &BDTCrit3::Crit3_3DAngle);
            BDTCrit3::reader->AddVariable("Crit3_ChangeRZRatio", &BDTCrit3::Crit3_ChangeRZRatio);
            BDTCrit3::reader->AddVariable("Crit3_2DAngle", &BDTCrit3::Crit3_2DAngle);
            

            BDTCrit3::reader->BookMVA("BDT3 method", weightFile.c_str());
        }

        if (( parent->getHits().size() == 2 )&&( child->getHits().size() == 2 )){
        } //a criterion for 1-segments
        else {
            std::stringstream s;
            s << "Crit3_BDT::This criterion needs 2 segments with 1 hit each, passed was a "
            <<  parent->getHits().size() << " hit segment (parent) and a "
            <<  child->getHits().size() << " hit segment (child).";

            throw KiTrack::BadSegmentLength( s.str() );
        }

        KiTrack::IHit* a = child->getHits()[0];
        KiTrack::IHit* b = child->getHits()[1];
        KiTrack::IHit* c = parent-> getHits()[1];

        // compute input values
        BDTCrit3::Crit3_2DAngle         =  Eval2DAngle( a, b, c );
        BDTCrit3::Crit3_3DAngle         =  Eval3DAngle( a, b, c );
        BDTCrit3::Crit3_ChangeRZRatio   =  EvalChangeRZRatio( a, b, c );
        
        float score = BDTCrit3::reader->EvaluateMVA("BDT3 method");

        if (_saveValues){
            _map_name_value["Crit3_BDT"] = score;
            _map_name_value["Crit3_BDT_2DAngle"] = BDTCrit3::Crit3_2DAngle;
            _map_name_value["Crit3_BDT_3DAngle"] = BDTCrit3::Crit3_3DAngle;
            _map_name_value["Crit3_BDT_ChangeRZRatio"] = BDTCrit3::Crit3_ChangeRZRatio;
            
        }

        if ( score < _scoreMin || score > _scoreMax ) return false;
        return true;
    }
  
    virtual ~BDTCrit3(){};

private:
  
    std::string weightFile;
    float _scoreMin{};
    float _scoreMax{};
    static TMVA::Reader *reader;
    // values input to BDT
    static float Crit3_ChangeRZRatio, Crit3_3DAngle, Crit3_2DAngle;
  
};

#endif