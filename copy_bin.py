Import("env", "projenv")
from shutil import copyfile

def pre_upload_spiffs(source, target, env):
    print "Post Upload Spiffs"
    copyfile( env.subst("$BUILD_DIR/spiffs.bin"),env.subst("bin/spiffs.bin"))

def pre_upload_fw(source, target, env):
    print "Post Upload Firmware"
    copyfile( env.subst('$BUILD_DIR/${PROGNAME}.bin'),env.subst("bin/${PROGNAME}.bin"))
    #copyfile( env.subst("$BUILD_DIR/spiffs.bin"),env.subst("bin/spiffs.bin"))

    #  Create hole bin
    # esptool.py --chip esp8266 make_image -f app.text.bin -a 0x40100000 -f app.data.bin -a 0x3ffe8000 -f app.rodata.bin -a 0x3ffe8c00 app.flash.bin

env.AddPostAction("upload", pre_upload_fw)
env.AddPostAction("$BUILD_DIR/spiffs.bin", pre_upload_spiffs)
