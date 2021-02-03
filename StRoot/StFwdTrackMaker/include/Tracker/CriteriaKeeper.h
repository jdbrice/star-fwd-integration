#ifndef CRITERIA_KEEPER_H
#define CRITERIA_KEEPER_H

#include "Criteria/Criteria.h"

class CriteriaKeeper : public KiTrack::ICriterion {

  public:
    CriteriaKeeper(KiTrack::ICriterion *child) {
        mChild = child;
        values.clear();

        _name = mChild->getName();
        _type = mChild->getType();
    }

    ~CriteriaKeeper() {
        if (mChild)
            delete mChild;
        mChild = nullptr;
    }

    virtual bool areCompatible(KiTrack::Segment *parent, KiTrack::Segment *child) {
        bool result = mChild->areCompatible(parent, child);



        // capture the primary result of the criteria
        float value = mChild->getMapOfValues()[mChild->getName()];
        bool same_track = false;
        int track_id = -1;

        // two hit criteria
        if ((parent->getHits().size() == 1) && (child->getHits().size() == 1)) {
            KiTrack::IHit *a = parent->getHits()[0];
            KiTrack::IHit *b = child->getHits()[0];

            same_track = (static_cast<FwdHit *>(a)->_tid == static_cast<FwdHit *>(b)->_tid && static_cast<FwdHit *>(a)->_tid != 0);
            if (same_track)
                track_id = static_cast<FwdHit *>(a)->_tid;

            if ( _name == "Crit2_RZRatio" ){
                std::map < std::string , float > _map_name_value = mChild->getMapOfValues();
                _map_name_value["Crit2_RZRatio_x1"] = a->getX();
                _map_name_value["Crit2_RZRatio_y1"] = a->getY();
                _map_name_value["Crit2_RZRatio_z1"] = a->getZ();
                _map_name_value["Crit2_RZRatio_x2"] = b->getX();
                _map_name_value["Crit2_RZRatio_y2"] = b->getY();
                _map_name_value["Crit2_RZRatio_z2"] = b->getZ();
                        
                allValues.push_back( _map_name_value );
            }
        }

        // three hit criteria (two two-segments)
        if ((parent->getHits().size() == 2) && (child->getHits().size() == 2)) {
            KiTrack::IHit *a = child->getHits()[0];
            KiTrack::IHit *b = child->getHits()[1];
            KiTrack::IHit *c = parent->getHits()[1];

            same_track = (static_cast<FwdHit *>(a)->_tid == static_cast<FwdHit *>(b)->_tid && static_cast<FwdHit *>(b)->_tid == static_cast<FwdHit *>(c)->_tid && static_cast<FwdHit *>(a)->_tid != 0);
            if (same_track)
                track_id = static_cast<FwdHit *>(a)->_tid;
        }

        values.push_back(value);
        track_ids.push_back(track_id);

        return result;
    }

    std::vector<float> getValues( ) {
        return values;
    }
    std::vector<std::map < std::string , float >> getAllValues() {
        return allValues;
    }
    std::vector<int> getTrackIds() {
        return track_ids;
    }

    void clear() {
        values.clear();
        track_ids.clear();
    }

  protected:
    ICriterion *mChild = nullptr;

    std::vector<float> values;
    std::vector<std::map < std::string , float >> allValues;
    std::vector<int> track_ids;
};


#endif