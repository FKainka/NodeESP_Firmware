#ifndef FSEditor_H_
#define FSEditor_H_
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
class FSEditor: public AsyncWebHandler {
  private:
    fs::FS _fs;
    fs::FS _fs_s;
    String _username;
    String _password;
    bool _authenticated;
    bool _sd;
    bool _isSd;
    uint32_t _startTime;
    FS getPath(String& path);
    bool createPathJson(FS fs, String path, bool sd, JsonArray& msg);
    bool exists(String path, FS fs);
  public:
    FSEditor(const fs::FS& fs, const fs::FS& fs_s, bool sd, const String& username=String(), const String& password=String());
    virtual bool canHandle(AsyncWebServerRequest *request) override final;
    virtual void handleRequest(AsyncWebServerRequest *request) override final;
    virtual void handleUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) override final;
    virtual bool isRequestHandlerTrivial() override final {return false;}
};

#endif
