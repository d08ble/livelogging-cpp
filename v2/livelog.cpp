//
//  livelog.cpp
//  ACPULanguage
//
//  Created by The Dream on 15.07.2022.
//

#include <stdio.h>
#include "livelog.h"

using namespace acpul;

// LCNode [

void LCNode::attributeSetValue(float strId, float v) {
    int attrId = _llog->attrid_by_strid(strId);
    
    _attributesValue[attrId] = v;
}

float LCNode::attributeGetValue(float strId) {
    int attrId = _llog->attrid_by_strid(strId);
    
    auto it = _attributesValue.find(attrId);
    if (it != _attributesValue.end())
        return it->second;
    
    return -1;
}

void LCNode::attributeMapSetValue(float strId, float key, float v) {
    int attrId = _llog->attrid_by_strid(strId);

    _attributesMapValue[attrId][key] = v;
}

float LCNode::attributeMapGetValue(float strId, float key) {
    int attrId = _llog->attrid_by_strid(strId);
    
    auto it = _attributesMapValue.find(attrId);
    if (it != _attributesMapValue.end()) {
        auto it1 = it->second.find(key);
        if (it1 != it->second.end())
            return it1->second;
    }
    
    return -1;
}

float LCNode::attributeSetObject(float strId, float objId) {
    if (!LCNode_objeck_lock(this, objId))
        return -1;

    int attrId = _llog->attrid_by_strid(strId);

    // release old object
   
    auto it = _attributesObject.find(attrId);
    if (it != _attributesObject.end())
        LCNode_objeck_unlock(this, it->second);

    _attributesObject[attrId] = objId;
    
    return objId;
}


float LCNode::attributeGetObject(float strId) {
    int attrId = _llog->attrid_by_strid(strId);

    // release object
    
    auto it = _attributesObject.find(attrId);
    if (it != _attributesObject.end())
        return it->second;

    return -1;
}

float LCNode::attributeReleaseObject(float strId, float objId) {
    int attrId = _llog->attrid_by_strid(strId);

    auto it = _attributesObject.find(attrId);
    if (it != _attributesObject.end()) {
        LCNode_objeck_unlock(this, it->second);
        _attributesObject.erase(it);
        return 0;
    }

    return -1;
}

void LCNode::attributeReleaseAllObjects() {
//    lock();
    for (auto it = _attributesObject.begin(); it != _attributesObject.end(); it++) {
        LCNode_objeck_unlock(this, it->second);
    }
    _attributesObject.clear();
//    unlock();
}

// LCNode ]
