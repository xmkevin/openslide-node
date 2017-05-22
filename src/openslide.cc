#include <nan.h>
#include <openslide/openslide.h>

void OpenSlide_Detect_Vendor(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  
  if (info.Length() == 0) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  v8::Local<v8::String> filePath = info[0]->ToString();
  const char *fileName = *filePath;
  char *vendor = openslide_detect_vendor(fileName);

  v8::Local<v8::String> vendorString = Nan::New(vendor);
  info.GetReturnValue().Set(vendorString);
}

void Init(v8::Local<v8::Object> exports) {
  exports->Set(Nan::New("detect_vendor").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(OpenSlide_Detect_Vendor)->GetFunction());
}

NODE_MODULE(openslide, Init)