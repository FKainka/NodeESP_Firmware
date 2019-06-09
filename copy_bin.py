Import("env", "projenv")
from shutil import copyfile

def pre_upload_spiffs(source, target, env):
    print "Post Upload Spiffs"
    copyfile( env.subst("$BUILD_DIR/spiffs.bin"),env.subst("bin/spiffs.bin"))

def pre_upload_fw(source, target, env):
    print "Post Upload Firmware"
    copyfile( env.subst('$BUILD_DIR/${PROGNAME}.bin'),env.subst("bin/${PROGNAME}.bin"))
    #copyfile( env.subst("$BUILD_DIR/spiffs.bin"),env.subst("bin/spiffs.bin"))

    # do some actions

env.AddPostAction("upload", pre_upload_fw)
env.AddPostAction("$BUILD_DIR/spiffs.bin", pre_upload_spiffs)
