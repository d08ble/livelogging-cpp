#include "../../livelogging-cpp/livelog.h"
//#include "../../livelogging-cpp/v2/livelog.h"

using namespace acpul;

extern "C" {

LiveLog *_llog = NULL;

void livelog_init(const char *filename) {
    if (!_llog) {
	    _llog = new LiveLog(filename);
    }
}

void livelog_log(const wchar_t *path, const wchar_t *s) {
	if (_llog) {
		_llog->log(path, s);
	}
}

void livelog_flush() {
	if (_llog) {
		_llog->flush();
	}
}

}

// V2 [

#if 0 // V2
void livelog_begin(const wchar_t *name, const char *filename) {
	_llog = new LiveLog(name, filename);
}

void livelog_log(const wchar_t *path, const wchar_t *s) {
	if (_llog) {
		_llog->log(path, s);
	}
}

void livelog_flush(bool force) {
	if (_llog) {
		_llog->flush(force);
	}
}
#endif

// V2 ]
