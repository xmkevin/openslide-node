#include "openslideobject.h"

Nan::Persistent<v8::Function> OpenSlideObject::constructor;

OpenSlideObject::OpenSlideObject(string fileName) {
    _fileName = fileName;
}

OpenSlideObject::~OpenSlideObject() {
  if (_osr != NULL) {
    openslide_close(_osr);
  }
}

void OpenSlideObject::Init(v8::Local<v8::Object> exports) {
    Nan::HandleScope scope;

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("OpenSlideObject").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl,"open",Open);

    constructor.Reset(tpl->GetFunction());
    exports->Set(Nan::New("OpenSlideObject").ToLocalChecked(), tpl->GetFunction());
}

void OpenSlideObject::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.IsConstructCall()) {
    // Invoked as constructor: `new OpenSlideObject(...)`
    v8::String::Utf8Value val(info[0]->ToString());
    std::string filePath (*val);
    OpenSlideObject *obj = new OpenSlideObject(filePath);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    // Invoked as plain function `OpenSlideObject(...)`, turn into construct call.
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = { info[0] };
    v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void OpenSlideObject::Open(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());
    openslide_t *osr = openslide_open(obj->_fileName.c_str());
    bool success = osr != NULL;
    if (!success) {
      info.GetReturnValue().Set(Nan::New(success));
      return;
    }
    // Assign osr to member variable
    obj->_osr = osr;

    // Store level count
    int32_t levelCount = openslide_get_level_count(osr);
    obj->_levelCount = levelCount;

    // Store dimensions
    for (int i = 0; i < levelCount; i++) {
      int64_t w, h;
      double downsample;
      openslide_get_level_dimensions(osr,i,&w,&h);
      downsample = openslide_get_level_downsample(osr,i);
      obj->_levelWidths.push_back(w);
      obj->_levelHeights.push_back(h);
      obj->_levelDownsamples.push_back(downsample);
    }

    // Properties
    const char* const *pNames = openslide_get_property_names(osr);
    int i = 0; 
    while (pNames[i] != 0) {
      const char *pValue = openslide_get_property_value(osr,pNames[i]);
      obj->_properties[pNames[i]] = pValue;
    }
    
    // Return success
    info.GetReturnValue().Set(Nan::New(success));
}