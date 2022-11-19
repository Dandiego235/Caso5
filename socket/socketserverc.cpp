#include <jni.h>
/* init argument struct */
JDK1_1InitArgs vm_args;
/* native method interface ptr */
JNIEnv* env;
/* denotes a Java VM */
JavaVM* jvm;
/* 1.1.x JDK */
vm_args.version = 0x00010001;
JNI_GetDefaultJavaVMInitArgs(&vm_args);
/* change the following for your system */
vm_args.classpath =
    ".;g:/java/jdk1.1.3/lib/classes.zip;";
/* load and init a Java VM */
JNI_CreateJavaVM(&jvm, &env, &vm_args);