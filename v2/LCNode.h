//
//  LCNode.h
//  ACPUUI
//
//  Created by The Dream on 23.12.2023.
//

#ifndef LCNode_h
#define LCNode_h

// LCNode [

class LCNode {
    std::wstring _name;
    typedef std::map<std::wstring, LCNode *> mnodes;
    mnodes _items;
    std::vector<LCNode *> _itemsIndex;
    std::wstringstream _ss;

    LiveLog *_llog;
    LCNode *_parent;

    // ATTRIBUTES [

    typedef std::map<int, float> map_attrid_v;
    typedef std::map<int, float> map_attrid_object;
    typedef std::map<int, std::map<float, float> > map_attrid_map_v_v;

public:
    map_attrid_v _attributesValue;
    map_attrid_object _attributesObject;
    map_attrid_map_v_v _attributesMapValue;

    // ATTRIBUTES ]
    
public:
    LCNode(LiveLog *llog, LCNode *parent, std::wstring name)
    : _name(name)
    , _llog(llog)
    , _parent(parent)
    {}

    virtual ~LCNode()
    {
        clear(_llog);
        _llog = NULL;
        _parent = NULL;
    }
    
    LiveLog *log() {
        return _llog;
    }
    
    LCNode *parent() {
        return _parent;
    }

    void setParent(LCNode *parent) {
        _parent = parent;
    }
    
    const std::wstring &name() {
        return _name;
    }

    void clearText() {
        _ss.str(L"");
        _ss.clear();
    }

    const std::wstring text() {
        return _ss.str();
    }

    const std::map<std::wstring, LCNode *> &items() {
        return _items;
    }

    const std::vector<LCNode *> &itemsIndex() {
        return _itemsIndex;
    }

    void clear(LiveLog *llog)
    {
        for (mnodes::iterator it = _items.begin(); it != _items.end(); it++) {
            LCNode_delete(llog, it->second);
        }
        _items.clear();
        _itemsIndex.clear();
    }

    LCNode *log(LiveLog *llog, const std::vector<std::wstring> &path, std::wstring s, bool replace)
    {
        LCNode *node = allocNode(llog, path, 0);
        if (!node)
            return NULL;

        // ?? always new
        if (replace) {
            node->clearText();
        }

        node->put(s);
        
        return node;
    }

    void put(std::wstring s) {
        _ss << s << "\n";
    }

    LCNode *allocNode(LiveLog *llog, const std::vector<std::wstring> &path, int i)
    {
        if (path.size() <= i)
            return this;
        
        if (i >= LLOG_LEVEL_MAX) {
            return NULL;
        }

        std::wstring name = path[i];
        mnodes::iterator it = _items.find(name);
        LCNode *node;

        if (it == _items.end()) {
            node = _items[name] = LCNode_new(llog, this, name);
            _itemsIndex.push_back(node);
        }
        else
            node = it->second;

        return node->allocNode(llog, path, i + 1);
    }

    LCNode *getNode(const std::vector<std::wstring> &path)
    {
        LCNode *node = this;
        for (int i = 0; i < path.size(); i++) {
            std::wstring name = path[i];
            auto it = node->items().find(name);

            if (it == node->items().end()) {
                return NULL;
            }
            else
                node = it->second;
        }
        return node;
    }

    LCNode *getNode(std::vector<std::wstring> &path, int i)
    {
//            LCNode *node = new acpul::LCNode();
//            printf("getNode\n");
//            return node;
        return NULL;
    }

    void deleteNode(std::vector<std::wstring> path)
    {}

    // ATTRIBUTES 1 [

    void attributeSetValue(float strId, float v);
    float attributeGetValue(float strId);

    float attributeGetObject(float strId);
    float attributeSetObject(float strId, float objId);

    float attributeReleaseObject(float strId, float objId);
    void attributeReleaseAllObjects();

    void attributeMapSetValue(float strId, float key, float v);
    float attributeMapGetValue(float strId, float key);


    // ATTRIBUTES 1 ]

};

// LCNode ]

#endif /* LCNode_h */
