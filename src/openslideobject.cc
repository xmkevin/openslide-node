#include "openslideobject.h"

Nan::Persistent<v8::Function> OpenSlideObject::constructor;

static const size_t DATA_SIZE = 256 * 256 * 4;
static char data[DATA_SIZE];

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
    // Methods
    Nan::SetPrototypeMethod(tpl,"open",Open);
    Nan::SetPrototypeMethod(tpl,"readRegion",ReadRegion);
    Nan::SetPrototypeMethod(tpl,"getPropertyValue",GetPropertyValue);

    // Properties
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
    Nan::SetAccessor(itpl,
                      Nan::New("propertyNames").ToLocalChecked(),
                      OpenSlideObject::GetSlidePropertyNames);

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

void OpenSlideObject::ReadRegion(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());
  openslide_t *osr = obj->_osr;

  int32_t level = info[0]->Int32Value();
  int64_t x = info[1]->Int32Value();
  int64_t y = info[2]->Int32Value();
  int64_t w = info[3]->Int32Value();
  int64_t h = info[4]->Int32Value();
  if (w > 256 || h > 256) {
    info.GetReturnValue().Set(Nan::New(0));
    return;
  }

  openslide_read_region(osr,(uint32_t *)&data,x,y,level,w,h);
  int64_t dataSize = w * h * 4;
  info.GetReturnValue().Set(CopyBuffer(data,dataSize).ToLocalChecked());
}

void OpenSlideObject::GetPropertyValue(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());
  v8::String::Utf8Value val(info[0]->ToString());
  std::string propertyName (*val);
  std::string propertyValue = obj->_properties[propertyName];
  info.GetReturnValue().Set(Nan::New(propertyValue).ToLocalChecked());
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

NAN_GETTER(OpenSlideObject::GetSlidePropertyNames) {
  OpenSlideObject *obj = ObjectWrap::Unwrap<OpenSlideObject>(info.Holder());

  std::map<std::string,std::string> pMap = obj->_properties;
  v8::Local<v8::Array> result = Nan::New<v8::Array>(pMap.size());
  int i = 0;
  for (map<string,string>::iterator it = pMap.begin(); it != pMap.end(); ++it) {
    Nan::Set(result,i, Nan::New<String>(it->first).ToLocalChecked());
    i++;
  }
  info.GetReturnValue().Set(result);
}