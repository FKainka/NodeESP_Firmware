#include "FSEditor.h"
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>


#define SPIFFS_MAXLENGTH_FILEPATH 32
const char *excludeListFile = "/editor/exclude_files.txt";

typedef struct ExcludeListS {
        char *item;
        ExcludeListS *next;
} ExcludeList;

static ExcludeList *excludes = NULL;

static bool matchWild(const char *pattern, const char *testee) {
        const char *nxPat = NULL, *nxTst = NULL;

        while (*testee) {
                if (( *pattern == '?' ) || (*pattern == *testee)) {
                        pattern++; testee++;
                        continue;
                }
                if (*pattern=='*') {
                        nxPat=pattern++; nxTst=testee;
                        continue;
                }
                if (nxPat) {
                        pattern = nxPat+1; testee=++nxTst;
                        continue;
                }
                return false;
        }
        while (*pattern=='*') {pattern++;}
        return (*pattern == 0);
}

static bool addExclude(const char *item){
        size_t len = strlen(item);
        if(!len) {
                return false;
        }
        ExcludeList *e = (ExcludeList *)malloc(sizeof(ExcludeList));
        if(!e) {
                return false;
        }
        e->item = (char *)malloc(len+1);
        if(!e->item) {
                free(e);
                return false;
        }
        memcpy(e->item, item, len+1);
        e->next = excludes;
        excludes = e;
        return true;
}

static void loadExcludeList(fs::FS &_fs, const char *filename){
        static char linebuf[SPIFFS_MAXLENGTH_FILEPATH];
        fs::File excludeFile=_fs.open(filename, "r");
        if(!excludeFile) {
                //addExclude("/*.js.gz");
                return;
        }
#ifdef ESP32
        if(excludeFile.isDirectory()) {
                excludeFile.close();
                return;
        }
#endif
        if (excludeFile.size() > 0) {
                uint8_t idx;
                bool isOverflowed = false;
                while (excludeFile.available()) {
                        linebuf[0] = '\0';
                        idx = 0;
                        int lastChar;
                        do {
                                lastChar = excludeFile.read();
                                if(lastChar != '\r') {
                                        linebuf[idx++] = (char) lastChar;
                                }
                        } while ((lastChar >= 0) && (lastChar != '\n') && (idx < SPIFFS_MAXLENGTH_FILEPATH));

                        if(isOverflowed) {
                                isOverflowed = (lastChar != '\n');
                                continue;
                        }
                        isOverflowed = (idx >= SPIFFS_MAXLENGTH_FILEPATH);
                        linebuf[idx-1] = '\0';
                        if(!addExclude(linebuf)) {
                                excludeFile.close();
                                return;
                        }
                }
        }
        excludeFile.close();
}

static bool isExcluded(fs::FS &_fs, const char *filename) {
        if(excludes == NULL) {
                loadExcludeList(_fs, excludeListFile);
        }
        ExcludeList *e = excludes;
        while(e) {
                if (matchWild(e->item, filename)) {
                        return true;
                }
                e = e->next;
        }
        return false;
}

// WEB HANDLER IMPLEMENTATION


FSEditor::FSEditor(const fs::FS& fs, const fs::FS& fs_s,bool sd, const String& username, const String& password)
        : _fs(fs)
        ,_fs_s(fs_s)
        ,_sd(sd)
        ,_username(username)
        ,_password(password)
        ,_authenticated(false)
        ,_startTime(0)
{
}

FS FSEditor::getPath(String& path){
        if (path.startsWith("/sd/")) {
                _isSd = true;
                path.replace("/sd/", "/");
                if(_sd) return _fs_s;
        }
        else if (path.startsWith("/intern/")) {
                _isSd = false;
                path.replace("/intern/", "/");
        }
        return _fs;
}

bool FSEditor::canHandle(AsyncWebServerRequest *request){
        if(request->url().equalsIgnoreCase("/edit")) {
                if(request->method() == HTTP_GET) {
                        if(request->hasParam("list"))
                                return true;
                        if(request->hasParam("edit")) {
                                String path = request->arg("edit"); //!!! param
                                FS fs = getPath(path);
                                request->_tempFile = fs.open(path, "r");
                                if(!request->_tempFile) {
                                        return false;
                                }
#ifdef ESP32
                                if(request->_tempFile.isDirectory()) {
                                        request->_tempFile.close();
                                        return false;
                                }
#endif
                        }
                        if(request->hasParam("download")) {
                                String path = request->arg("download");
                                FS fs = getPath(path);
                                request->_tempFile = fs.open(path, "r");
                                if(!request->_tempFile) {
                                        return false;
                                }
#ifdef ESP32
                                if(request->_tempFile.isDirectory()) {
                                        request->_tempFile.close();
                                        return false;
                                }
#endif
                        }
                        request->addInterestingHeader("If-Modified-Since");
                        return true;
                }
                else if(request->method() == HTTP_POST)
                        return true;
                else if(request->method() == HTTP_DELETE)
                        return true;
                else if(request->method() == HTTP_PUT)
                        return true;

        }
        return false;
}


/**
 * [createPathJson description]
 * @param  fs   [description]
 * @param  path ohne SD Prefix
 * @param  sd   [description]
 * @param  msg  [description]
 * @return      [description]
 */
bool FSEditor::createPathJson(FS fs, String path, bool sd, JsonArray& msg){
        File dir = fs.open(path);

        if (!dir.isDirectory()) {
                dir.close();
                return false;
        }
        dir.rewindDirectory();

        while(true) {
                File entry = dir.openNextFile();
                if (!entry) break;

                String filename = entry.name();
                sd ? filename = "/sd"+filename : filename = "/intern"+filename;
                //Serial.println(filename);
                if (isExcluded(_fs,filename.c_str())) {
                        //Serial.println ("Excluded File: " + filename);
                        entry.close();
                        continue;
                }

                JsonObject& msg_element = msg.createNestedObject();
                bool directory = entry.isDirectory();
                msg_element["type"] = directory ? "dir" : "file";
                msg_element["name"] = filename;

                entry.close();
        }
        dir.close();

        return true;
}

/**
 * Fix for FS.exists Bug with SPIFFS (https://github.com/espressif/arduino-esp32/issues/1264)
 * @param  paht filename
 * @return      true if exists
 */
bool FSEditor::exists(String path, FS fs){
        if(!_isSd) return SPIFFS.exists(path);
        else return fs.exists(path);
}


void FSEditor::handleRequest(AsyncWebServerRequest *request){
        if(_username.length() && _password.length() && !request->authenticate(_username.c_str(), _password.c_str()))
                return request->requestAuthentication();

        if(request->method() == HTTP_GET) {
                if(request->hasParam("list")) {
                        String path = request->getParam("list")->value();

                        DynamicJsonBuffer jsonBuffer;
                        JsonArray& root = jsonBuffer.createArray();

                        if (path == "/") { //root
                                JsonObject& msg_element = root.createNestedObject();
                                msg_element["type"] ="dir";
                                msg_element["name"] = "/intern/";
                                if (_sd) {
                                        JsonObject& msg_element2 = root.createNestedObject();
                                        msg_element2["type"] ="dir";
                                        msg_element2["name"] = "/sd/";
                                }
                        }
                        else{ //sub folde
                                bool fs_sd = path.startsWith("/sd/");
                                FS fs = getPath(path);
                                if (!createPathJson(fs, path,fs_sd, root)) Serial.println("Error");
                        }

                        AsyncResponseStream *response = request->beginResponseStream("text/json");
                        root.printTo(*response);
                        request->send(response);
                }
                else if(request->hasParam("edit") || request->hasParam("download")) {
                        request->send(request->_tempFile, request->_tempFile.name(), String(), request->hasParam("download"));
                }
                else {
                        const char * buildTime = __DATE__ " " __TIME__ " GMT";
                        if (request->header("If-Modified-Since").equals(buildTime)) {
                                request->send(304);
                        } else {
                                //Edit Page
                                AsyncWebServerResponse *response = request->beginResponse(_fs, "/edit/index.htm", "text/html", false);
                                //AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", edit_htm_gz, edit_htm_gz_len);
                                response->addHeader("Content-Encoding", "gzip");
                                response->addHeader("Last-Modified", buildTime);
                                request->send(response);
                        }
                }
        } else if(request->method() == HTTP_DELETE) {
                if(request->hasParam("path", true)) {
                        String path = request->getParam("path", true)->value();
                        FS fs = getPath(path);
                        if(_isSd && (path.indexOf('.')<0)) {
                                SD.rmdir(path) ? request->send(200, "", "DELETE: "+path) : request->send(500);
                        }else{
                                fs.remove(path) ? request->send(200, "", "DELETE: "+path) : request->send(500);
                        }
                } else
                        request->send(404);
        } else if(request->method() == HTTP_POST) {
                if(request->hasParam("data", true, true)) {
                        String path = request->getParam("data", true, true)->value();
                        FS fs = getPath(path);
                        //  Serial.println("HTTP POST: " + path);

                        if (exists(path, fs)) {
                                request->send(200, "", "UPLOADED: "+request->getParam("data", true, true)->value());
                        }
                        else {
                                request->send(500);
                        }
                }
                else {
                        request->send(500);
                }
        } else if(request->method() == HTTP_PUT) {
                if(request->hasParam("path", true)) {
                        String filename = request->getParam("path", true)->value();
                        FS fs = getPath(filename);
                        //Serial.println("HTTP PUT: " + filename);
                        if(exists(filename, fs)) {
                                request->send(200);
                        } else {
                                if(_isSd && (filename.indexOf('.')<0)) {
                                        //SD and not file -> create folder
                                        SD.mkdir(filename) ? request->send(200, "", "CREATE FOLDER: "+filename) : request->send(500);
                                }
                                else{
                                        fs::File f = fs.open(filename, "w");
                                        if(f) {
                                                f.write((uint8_t)0x00);
                                                f.close();
                                                request->send(200, "", "CREATE: "+filename);
                                        } else {
                                                request->send(500);
                                        }
                                }
                        }
                } else
                        request->send(400);
        }
}

void FSEditor::handleUpload(AsyncWebServerRequest *request, const String& filename_c, size_t index, uint8_t *data, size_t len, bool final){
        if(!index) {
                if(!_username.length() || request->authenticate(_username.c_str(),_password.c_str())) {
                        _authenticated = true;
                        String filename =filename_c;
                        FS fs = getPath(filename);
                        request->_tempFile = fs.open(filename, "w");
                        _startTime = millis();
                }
        }
        if(_authenticated && request->_tempFile) {
                if(len) {
                        request->_tempFile.write(data,len);
                }
                if(final) {
                        request->_tempFile.close();
                }
        }
}
