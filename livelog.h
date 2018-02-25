//
// Created by d08ble on 2/22/18.
//

// __LIVELOG_H [

#ifndef __LIVELOG_H
#define __LIVELOG_H

#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>

namespace acpul {


    class LiveLogBasic {
    protected:
        std::wofstream _fs;
        std::wstring _path;
        std::vector<std::wstring> _stack;
//        bool _empty;

    public:
        LiveLogBasic(const char *filename)
        : _path(L"")
//        , _empty(false)
        , _fs(filename)
        {}

        ~LiveLogBasic()
        {}

        void begin(std::wstring path)
        {
            _stack.push_back(_path);
            _path = path;
            _fs << L"# " << _path << L"[\n";
//            _empty = true;
        }

        void end()
        {
//            if (!_empty) {
//                _fs << L"# " << _path << L"]\n";
//            }
//            _empty = true;
            _fs << L"# " << _path << L"]\n";
            _path = L"";

            if (_stack.size() > 0) {
                _path = _stack.back();
                _stack.pop_back();
            }
        }

        void put(std::wstring s)
        {
//            if (_empty) {
//                _fs << L"# " << _path << L"[\n";
//                _empty = false;
//            }

            _fs << s << L"\n";
        }

        void flush()
        {
            _fs.flush();
        }
    };

    class LCNode {
        std::wstring _name;
        std::map<std::wstring, LCNode *> _items;
        std::vector<LCNode *> _itemsIndex;
        std::wstringstream _ss;

    public:
        LCNode(std::wstring name)
        : _name(name)
        {}

        ~LCNode()
        {
            clear();
        }

        const std::wstring &name()
        {
            return _name;
        }

        const std::wstring text()
        {
            return _ss.str();
        }

        const std::map<std::wstring, LCNode *> &items()
        {
            return _items;
        }

        const std::vector<LCNode *> &itemsIndex()
        {
            return _itemsIndex;
        }

        void clear()
        {
            for (auto it = _items.begin(); it != _items.end(); it++) {
                delete it->second;
            }
            _items.clear();
            _itemsIndex.clear();
        }

        void log(std::vector<std::wstring> &path, std::wstring s)
        {
            LCNode *node = allocNode(path, 0);
            if (!node)
                return;

            node->put(s);
        }

        void put(std::wstring s)
        {
            _ss << s;
        }

        LCNode *allocNode(std::vector<std::wstring> &path, int i)
        {
            if (path.size() <= i)
                return this;

            std::wstring name = path[i];
            auto it = _items.find(name);
            LCNode *node;

            if (it == _items.end()) {
                node = _items[name] = new LCNode(name);
                _itemsIndex.push_back(node);
            }
            else
                node = it->second;

            return node->allocNode(path, i + 1);
        }

        void deleteNode(std::vector<std::wstring> path)
        {}
    };

    class LiveLog : public LiveLogBasic {
        LCNode _tree;
        std::string _filename;

    public:
        LiveLog(const char *filename)
        : LiveLogBasic(filename)
        , _tree(L"LiveLog++")
        , _filename(filename)
        {}

        ~LiveLog()
        {}

        void log(std::wstring path, std::wstring s)
        {
            std::vector<std::wstring> vstrings;

            std::wstringstream ss(path);
            std::wstring item;
            while (std::getline(ss, item, L'/')) {
                vstrings.push_back(item);
                //*(result++) = item;
            }

            _tree.log(vstrings, s);
        }

        void replace(std::wstring path, std::wstring s)
        {}

        void erase(std::wstring path)
        {}

        void flush()
        {
            // clear hack
            _fs.close();
            _fs = std::wofstream(_filename);

            flushNode(&_tree);

            _fs.flush();
        }

        void flushNode(LCNode *node)
        {
            if (!node)
                return;

            begin(node->name());

            put(node->text());

            for (auto it = node->itemsIndex().begin(); it != node->itemsIndex().end(); it++) {
                flushNode(*it);
            }

            end();
        }
    };
}

#endif

// __LIVELOG_H ]
