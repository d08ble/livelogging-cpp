/*
Title: LiveLogging++

URL: https://github.com/d08ble/livelogging-cpp

MIT License

Copyright (c) 2014-2018 Web3 Crypto Wallet Team

 info@web3cryptowallet.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

// __LIVELOG_H [

#ifndef __LIVELOG_H
#define __LIVELOG_H

#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include <time.h>
#include <unistd.h>

#define LLOG_LEVEL_MAX 1000

namespace acpul {

class LCNode;
class LiveLog;
class LiveLogEvent;

LiveLog *llogGetAcpulData();

LCNode *LCNode_new(LiveLog *llog, LCNode *parent, std::wstring name);
bool LCNode_delete(LiveLog *llog, LCNode *node);
float LCNode_get_id(LCNode *node);
bool LCNode_event(LCNode *node, LiveLogEvent &event);

bool LCNode_objeck_lock(LCNode *node, float objId);
bool LCNode_objeck_unlock(LCNode *node, float objId);

#include "./LiveLogBasic.h"
#include "./LCNode.h"
#include "./LiveLogEvent.h"

    // LiveLog [

    class LiveLog : public LiveLogBasic {
        LCNode *_tree;
        std::string _filename;
        bool _modified;

        time_t _timeFlushPrev;

        int _flushTimeout;
        double _flushTimeDelta;
        
        // PATH [
        
        std::vector<std::wstring> _pathStrings;
        
        std::vector<std::wstring> _pathStringsInternal;

        bool _splitterBackslash;
        bool _splitterDot;

        // PATH ]
        // ATTRIBUTES [

#if 0
        // todo: vv::buffer compare
        struct eqname {
            inline size_t operator()(const float *s) const {
                size_t hash = 1;
                if (!s)
                    return 0;
                for (; *s; ++s)
                    hash = hash * 5 + ((size_t)*s);
                return hash;
            }
            inline bool operator()(const float *s1, const float *s2) const {
                return fscmp(s1, s2);
            }
        };
        
        typedef unordered_map<const float *, int, eqname, eqname > smap;
#endif

        typedef std::map<float, int> map_strid_attrid;
        typedef std::map<std::wstring, int> map_str_attrid;

        typedef std::map<int, float> rmap_strid_attrid; // reverse map_strid_attrid
        typedef std::map<int, std::wstring> rmap_str_attrid; // reverse map_str_attrid

        map_strid_attrid _attrStrIdAttrId;
        map_str_attrid _attrStrAttrId;

        rmap_strid_attrid _rattrStrIdAttrId; // reverse _attrStrIdAttrId
        rmap_str_attrid _rattrStrAttrId; // reverse _rattrStrAttrId

        int _attrId;
        
        // ATTRIBUTES ]

    public:

        // ATTRIBUTES 1 [

        int attrid_new() {
            return ++_attrId;
        }

        bool attrid_by_strid_check(float strId) {
            auto it = _attrStrIdAttrId.find(strId);
            return (it != _attrStrIdAttrId.end());
        }

        int attrid_by_strid(float strId) {
            auto it = _attrStrIdAttrId.find(strId);
            if (it != _attrStrIdAttrId.end())
                return it->second;

            int attrId = attrid_new();
            _attrStrIdAttrId[strId] = attrId;
            _rattrStrIdAttrId[attrId] = strId;

            return attrId;
        }

        int attrid_by_str(float strId, const wchar_t *s) {
            if (!s)
                return -1;
            auto it = _attrStrAttrId.find(s);
            if (it != _attrStrAttrId.end()) {
                int attrId = it->second;
                if (strId > 0) {
                    _attrStrIdAttrId[strId] = attrId;
                    _rattrStrIdAttrId[attrId] = strId;
                }
                return attrId;
            }

            int attrId = attrid_new();
            _attrStrAttrId[s] = attrId;
            _rattrStrAttrId[attrId] = s;

            if (strId > 0) {
                _attrStrIdAttrId[strId] = attrId;
                _rattrStrIdAttrId[attrId] = strId;
            }

            return attrId;
        }
        
        float strid_by_attrid(int attrId) {
            auto it = _rattrStrIdAttrId.find(attrId);
            if (it != _rattrStrIdAttrId.end()) {
                return it->second;
            }
            return -1;
        }

        // ATTRIBUTES 1 ]

        // EVENTS [
        
        typedef LiveLogEvent Event;

        std::vector<Event> _events;
        bool _eventsOn;
        int _eventLastIndex;

        // EVENTS ]

        pthread_mutex_t _lock;
        pthread_mutex_t _lockEvents;
        
    public:
        LiveLog(const wchar_t *name, const char *filename)
        : LiveLogBasic(filename)
        , _filename(filename)
        , _modified(true)
        , _flushTimeout(0)
        , _eventsOn(false)
        , _attrId(0)
        {
            eventsInit();
            
            pthread_mutex_init(&_lock, NULL);

            _splitterBackslash = true;
            _splitterDot = false;
            
            _tree = LCNode_new(this, NULL, name);
        }

        ~LiveLog()
        {
            LCNode_delete(this, _tree);
            _tree = NULL;
        }

        void setFilename(std::string filename) {
            _filename = filename;
        }

        std::string filename() {
            return _filename;
        }

        void setFlushTimeout(int flushTimeout) {
            _flushTimeout = flushTimeout;
        }
        
        // PATH [
        
        void pathPush(const wchar_t *name)
        {
            _pathStrings.push_back(std::wstring(name));
        }

        void pathPop()
        {
            if (_pathStrings.size() > 0)
                _pathStrings.pop_back();
        }

        void pathClear()
        {
            _pathStrings.clear();
        }
        
        void pathSelect(LCNode *node)
        {
            pathClear();
            pathSelectNode(node->parent());
        }
        
        void pathSelectNode(LCNode *node)
        {
            if (!node) {
                return;
            }
            pathSelectNode(node->parent());
            
            _pathStrings.push_back(node->name());
        }

        void setPathSplitter(bool backslash, bool dot) {
            _splitterBackslash = backslash;
            _splitterDot = dot;
        }

        std::vector<std::wstring> &parsePath(std::wstring *path)
        {
            // path is selector if null
            if (!path)
                return _pathStrings;
            
            // or parse string
            _pathStringsInternal.clear();
            std::vector<std::wstring> &vstrings = _pathStringsInternal;

            if (*path == L"")
                return vstrings; // root

            std::wstringstream ss(*path);
            std::wstring item;
            if (_splitterBackslash) {
                while (std::getline(ss, item, L'/')) {
                    vstrings.push_back(item);
                }
            }
            else if (_splitterDot) {
                while (std::getline(ss, item, L'.')) {
                    vstrings.push_back(item);
                }
            }

            return vstrings;
        }
        
        std::wstring pathGet()
        {
            std::wstringstream ss;
            const wchar_t *splitter;
            if (_splitterBackslash) {
                splitter = L"/";
            } else {
                splitter = L".";
            }
            for (int i = 0; i < _pathStrings.size(); i++) {
                if (i > 0)
                    ss << splitter;
                ss << _pathStrings[i];
            }
            return ss.str();
        }
        
        // PATH ]
        // LOCK [

        void lock() {
            pthread_mutex_lock(&_lock);
        }
        
        void unlock() {
            pthread_mutex_unlock(&_lock);
        }

        // LOCK ]
        // FIND [

        LCNode *find(std::wstring *path)
        {
            if (!_tree) {
                return NULL;
            }
            
            // WARNING NO LOCK!
//            lock();
            
            std::vector<std::wstring> &vstrings = parsePath(path);
        
            LCNode *node = _tree->getNode(vstrings);

//            unlock();

            return node;
        }

        LCNode *find(const std::vector<std::wstring> &vstrings)
        {
            LCNode *node = _tree->getNode(vstrings);
            return node;
        }
        
        // FIND ]

        const std::wstring get(std::wstring *path)
        {
            // WARNING NO LOCK!
            
            LCNode *node = find(path);
            
            if (!node)
                return L"";
            
            return node->text();
        }
        
        LCNode *log(std::wstring *path, std::wstring s, bool replace)
        {
            std::vector<std::wstring> &vstrings = parsePath(path);

            LCNode *node = log(vstrings, s, replace);
            
            return node;
        }
        
        LCNode *log(const std::vector<std::wstring> &vstrings, std::wstring s, bool replace) {
            if (!_tree) {
                return NULL;
            }
            
            lock();

            _modified = true;

            LCNode *node = _tree->log(this, vstrings, s, replace);

            // EVENT ADD PUT/REPLACE [

            if (_eventsOn) {
                Event event = Event(
                    replace ? Event::Type::Replace : Event::Type::Put,
                    LCNode_get_id(node),
                    -1,
                    L"",
                    s
                );
                eventsAdd(event);
            }

            // EVENT ADD PUT/REPLACE ]
            
            unlock();

            return node;
        }


        void log(std::wstring *path, std::wstring s)
        {
            log(path, s, false);
        }

        void replace(std::wstring *path, std::wstring s)
        {
            log(path, s, true);
        }

        void erase(std::wstring *path)
        {
            lock();
            
            _modified = true;

            LCNode *node = find(path);

            // EVENT ADD ERASE [
            
            // todo: !!!

            if (_eventsOn) {
                Event event = Event(
                    Event::Type::Erase,
                    LCNode_get_id(node),
                    -1,
                    L"",
                    L""
                );
                eventsAdd(event);
            }

            // EVENT ADD ERASE ]

            unlock();
        }

        // WRAPPER [
        
        LCNode *find(const wchar_t *path)
        {
            if (path) {
                std::wstring p(path);
                return find(&p);
            }
            else {
                return find((std::wstring *)NULL);
            }
        }

        // Bad? need review empty line yes or not
        LCNode *findOrCreate(const wchar_t *path, std::wstring s)
        {
            LCNode *node = find(path);
            if (node)
                return node;
            return log(path, s);
        }
//        LCNode *find(std::vector<std::wstring> &vstrings);

        LCNode *findOrCreate(const std::vector<std::wstring> &vstrings, std::wstring s)
        {
            LCNode *node = find(vstrings);
            if (node)
                return node;
            return log(vstrings, s, false);
        }

        LCNode *log(const wchar_t *path, std::wstring s, bool replace)
        {
            if (path) {
                std::wstring p(path);
                return log(&p, s, replace);
            }
            else {
                return log((std::wstring *)NULL, s, replace);
            }
        }

        LCNode *log(const wchar_t *path, std::wstring s)
        {
            return log(path, s, false);
        }

        LCNode *replace(const wchar_t *path, std::wstring s)
        {
            return log(path, s, true);
        }
        
        void erase(const wchar_t *path)
        {
            if (path) {
                std::wstring p(path);
                erase(&p);
            }
            else {
                erase((std::wstring *)NULL);
            }
        }
        
        // WRAPPER ]
        // EVENTS [

        void eventsInit() {
            pthread_mutex_init(&_lockEvents, NULL);
            _eventLastIndex = 0;
        }
        
        void eventsLock() {
            pthread_mutex_lock(&_lockEvents);
        }

        void eventsUnlock() {
            pthread_mutex_unlock(&_lockEvents);
        }

        void eventsAdd(Event &event) {
            eventsLock();
            _events.push_back(event);
            eventsUnlock();
        }

        int eventsCount() {
            eventsLock();
            int size = _events.size();
            eventsUnlock();
            return size;
        }
        
        Event eventsGet(int index) {
            pthread_mutex_lock(&_lockEvents);
            Event event = _events[index];
            pthread_mutex_unlock(&_lockEvents);
            return event;
        }

        void eventsClear(Event &event) {
            eventsLock();
            _events.clear();
            eventsUnlock();
        }

        // EVENTS ]
        // ATTRIBUTES [
        
        
        
        // ATTRIBUTES ]

        void flush(bool force = false)
        {
            if (!_modified)
                return;

            // check time diff [

            if (!force && _flushTimeout > 0) {
                time_t timeEnd;
                time(&timeEnd);
                double timeDiff;
                timeDiff = difftime(timeEnd, _timeFlushPrev);
                _timeFlushPrev = timeEnd;
                _flushTimeDelta += timeDiff;

                if (_flushTimeDelta < _flushTimeout)
                    return;

                _flushTimeDelta = 0;
            }

            // check time diff ]

            if (_filename == "")
                return;

            // clear hack
            _fs->close();

//            std::locale::global(std::locale(""));

            delete _fs;
//            _fs = new std::wofstream(_filename.c_str(), std::ios::out|std::ios::app);
//            _fs = new std::wofstream(_filename.c_str(), std::ios::out|std::ios::binary);
            _fs = new std::wofstream(_filename.c_str());

            _fs->imbue(std::locale(""));
/*
            *_fs << wstring(L"й") << "\n";

            static int i = 0;
            *_fs << "# LLL [\n" << ++i << "\n# LLL ]\n";
            _fs->flush();
*/
            flushNode(_tree);

//            *_fs << wstring(L"й") << "\n";
            _fs->flush();
//            _fs->close();

            _modified = false;
        }

        void flushNode(LCNode *node)
        {
            if (!node)
                return;

            begin(node->name());

            put(node->text());

            //for (std::vector<LCNode *>::iterator it = node->itemsIndex().begin(); it != node->itemsIndex().end(); it++) {
//            flushNode(*it);
            for (int i = 0; i < node->itemsIndex().size(); i++) {
                flushNode(node->itemsIndex()[i]);
            }

            end();
        }
        
        void push(std::wstring path) {
            _stack.push_back(_path);
            _path = path;
        }
        
        void pop() {
            if (_stack.size() > 0) {
                _path = _stack.back();
                _stack.pop_back();
            }
            else {
                _path = L"";
            }
        }
        
        void begin2(std::wstringstream &ss, std::wstring path) {
            push(path);

            ss << L"# " << _path << L"[\n";
        }
        
        void put2(std::wstringstream &ss, std::wstring s)
        {
            ss << s << L"\n";
        }
        
        void end2(std::wstringstream &ss)
        {
            ss << L"# " << _path << L"]\n";

            pop();
        }

        void putAttributes(std::wstringstream &ss, LCNode *node);

        void encodeNode(std::wstringstream &ss, LCNode *node)
        {
            if (!node)
                return;

            begin2(ss, node->name());

            putAttributes(ss, node);

            put2(ss, node->text());

            for (int i = 0; i < node->itemsIndex().size(); i++) {
                encodeNode(ss, node->itemsIndex()[i]);
            }

            end2(ss);
        }
        
        void parseString(std::wstring prefix, const std::wstring& input);


        std::wstring encode() {
            std::wstringstream ss;
            
            lock();
            
            encodeNode(ss, _tree);

            unlock();
            
            std::wstring s = ss.str();
            return s;
        }
        
        void decode(std::wstring s) {
            parseString(L"#", s);
        }
    };

    // LiveLog ]

//typedef LiveLog LiveLog2;
}

#endif

// __LIVELOG_H ]
