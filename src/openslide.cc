#include <nan.h>
#include <openslide/openslide.h>
#include <string>

using namespace std;
using namespace Nan;
using namespace v8;

void Detect_Vendor(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  
  if (info.Length() == 0) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  v8::String::Utf8Value val(info[0]->ToString());
  std::string filePath (*val);
  //const char *vendor = openslide_detect_vendor(filePath.c_str());

  info.GetReturnValue().Set(Nan::New<String>(filePath.c_str()).ToLocalChecked());
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("detect_vendor").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(Detect_Vendor)->GetFunction());
}

NODE_MODULE(openslide, Init)