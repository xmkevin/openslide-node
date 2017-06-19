#include <nan.h>
#include <openslide/openslide.h>
#include <string>

using namespace std;
using namespace Nan;
using namespace v8;

class OpenSlideObject : public Nan::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
    private:
        explicit OpenSlideObject(std::string fileName);
        ~OpenSlideObject();
        
        static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static void Open(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static Nan::Persistent<v8::Function> constructor;

        openslide_t *_osr;
        std::string _fileName;
        int32_t _levelCount;
        int64_t[] _levelWidths;
        int64_t[] _levelHeights;
        double[] _levelDownsamples;


};