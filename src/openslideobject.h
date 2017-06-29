#include <nan.h>
#include <openslide/openslide.h>
#include <string>
#include <map>

using namespace std;
using namespace Nan;
using namespace v8;

class OpenSlideObject : public Nan::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
    private:
        explicit OpenSlideObject(std::string fileName);
        ~OpenSlideObject();
        // Methods
        static Nan::Persistent<v8::Function> constructor;
        static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
        static void Open(const Nan::FunctionCallbackInfo<v8::Value>& info);
        // Properties
        static void GetLevelCount(const Nan::FunctionCallbackInfo<v8::Value>& info);
        //Fields
        openslide_t *_osr;
        std::string _fileName;
        int32_t _levelCount;
        std::vector<int64_t> _levelWidths;
        std::vector<int64_t> _levelHeights;
        std::vector<double> _levelDownsamples;
        std::map<std::string,std::string> _properties;

};