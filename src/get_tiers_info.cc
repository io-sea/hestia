#include "../test/data_placement_engine/eejit.h"
#include "hestia.h"
#include "tiers.h"
#include <iostream>

std::list<hestia::hsm_tier> hestia::get_tiers_info(){
    dpe::Eejit dpe(tiers);
    std::list<hestia::hsm_tier> info = dpe.get_tiers_info();

   /* 
    std::list<hestia::hsm_tier> info {};
    for(auto it= tids.begin(); it !=tids.end();++it){
        for(auto is= all_info.begin(); is !=all_info.end();++is){
            if(*it == is->id){
                info.push_back(*is);
            }
        }
    }
*/

    return info;
}

