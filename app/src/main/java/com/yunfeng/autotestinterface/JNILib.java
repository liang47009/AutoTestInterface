package com.yunfeng.autotestinterface;

/**
 * xll
 * Created by xll on 2017/11/24.
 */
public class JNILib {
    public static native String stringFromJNI();

    public static native void sendmsg(String text);

    public static native void sendservermsg(String text);

    public static native void startClient();

    public static native void startServer();


}

class asdf extends JNILib {

}

class sdafasf {

    public void set(JNILib lib) {

    }

    public void main() {
        asdf asdfs = new asdf();
        this.set(asdfs);
    }
}