//
//  LiveLogEvent.h
//  ACPUUI
//
//  Created by The Dream on 23.12.2023.
//

#ifndef LiveLogEvent_h
#define LiveLogEvent_h

// LiveLogEvent [

class LiveLogEvent {
public:
    enum Type {
        Put = 0,
        Replace = 1,
        Erase = 2,
        
        AddNode = 3,
        RemoveNode = 4,
        UpdateNode = 5
    };
    time_t _time;
    Type _type;
    float _nodeId;
    float _subnodeId;
    std::wstring _path;
    std::wstring _msg;
    LiveLogEvent(Type type, float nodeId, float subnodeId, std::wstring path, std::wstring msg)
    : _type(type)
    , _nodeId(nodeId)
    , _subnodeId(subnodeId)
    , _path(path)
    , _msg(msg)
    {
        time(&_time);
    }
};

// LiveLogEvent ]

#endif /* LiveLogEvent_h */
