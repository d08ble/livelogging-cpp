//
//  LiveLogBasic.h
//  ACPUUI
//
//  Created by The Dream on 23.12.2023.
//

#ifndef LiveLogBasic_h
#define LiveLogBasic_h

// LiveLogBasic [

class LiveLogBasic {
protected:
    std::wofstream *_fs;
    FILE *_file;
    std::wstring _path;
    std::vector<std::wstring> _stack;
//        bool _empty;

public:
    LiveLogBasic(const char *filename)
    : _path(L"")
//        , _empty(false)
    , _fs(new std::wofstream(filename))
    , _file(NULL)
    {}

    ~LiveLogBasic()
    {}
/*
    void open(const char *path)
    {
        if (!_file) {
            _file = fopen(path, "w");
        }
    }

    void close()
    {
        if (_file) {
            fclose(_file);
            _file = NULL;
        }
    }

    void write(wstring s)
    {
        //fwrite()
    }
*/
    void begin(std::wstring path)
    {
        _stack.push_back(_path);
        _path = path;
//            write(L"# " + _path + L"\n");
        *_fs << L"# " << _path << L"[\n";
//            _empty = true;

        printf("# %S [\n", _path.c_str());
        _fs->flush();
    }

    void end()
    {
//            if (!_empty) {
//                _fs << L"# " << _path << L"]\n";
//            }
//            _empty = true;
        *_fs << L"# " << _path << L"]\n";

        printf("# %S ]\n", _path.c_str());
        _fs->flush();

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

        int n = wcslen(s.c_str());
//            printf("%i\n", n);
        printf("%i %S\n", n, s.c_str());

//*_fs << "QQQ\n";
        *_fs << s << L"\n";
//*_fs << "WWW\n";
        _fs->flush();
    }

    void flush()
    {
        _fs->flush();
    }
};

// LiveLogBasic ]

#endif /* LiveLogBasic_h */
