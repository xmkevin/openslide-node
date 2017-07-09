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

    v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();
    Nan::SetAccessor(itpl,
                      Nan::New("levelCount").ToLocalChecked(),
                      OpenSlideObject::GetLevelCount);
    Nan::SetAccessor(itpl,
                      Nan::New("levelWidths").ToLocalChecked(),
                      OpenSlideObject::GetLevelWidths);
    Nan::SetAccessor(itpl,
                      Nan::New("levelHeights").ToLocalChecked(),
                      OpenSlideObject::GetLevelHeights);
    Nan::SetAccessor(itpl,
                      Nan::New("levelDownsamples").ToLocalChecked(),
                      OpenSlideObject::GetLevelDownsamples);

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
      i++;
    }
    
    // Return success
    info.GetReturnValue().Set(Nan::New(success));
}

NAN_GETTER(OpenSlideObject::GetLevelCount) {
  OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->_levelCount));
}

NAN_GETTER(OpenSlideObject::GetLevelWidths) {
  OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());
  std::vector<int64_t> levelWidths = obj->_levelWidths;
  
  v8::Local<v8::Array> result = Nan::New<v8::Array>(levelWidths.size());
  for ( size_t i = 0; i < levelWidths.size(); i++) {
    int64_t w = levelWidths[i];
    Nan::Set(result,i, Nan::New<v8::Number>(w));
  }

  info.GetReturnValue().Set(result);
}

NAN_GETTER(OpenSlideObject::GetLevelHeights) {
  OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());
  std::vector<int64_t> levelHeights = obj->_levelHeights;
  
  v8::Local<v8::Array> result = Nan::New<v8::Array>(levelHeights.size());
  for ( size_t i = 0; i < levelHeights.size(); i++) {
    int64_t w = levelHeights[i];
    Nan::Set(result,i, Nan::New<v8::Number>(w));
  }

  info.GetReturnValue().Set(result);
}

NAN_GETTER(OpenSlideObject::GetLevelDownsamples) {
  OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());
  std::vector<double> downsamples = obj->_levelDownsamples;
  
  v8::Local<v8::Array> result = Nan::New<v8::Array>(downsamples.size());
  for ( size_t i = 0; i < downsamples.size(); i++) {
    double w = downsamples[i];
    Nan::Set(result,i, Nan::New<v8::Number>(w));
  }

  info.GetReturnValue().Set(result);
}