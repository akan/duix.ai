package com.btows.ncnntest;

import android.content.res.AssetManager;
import android.view.Surface;

public class SCRFDNcnn {
    public void onMessageCallback(final int what, final int arg1, final long arg2, final String msg1,
                                  final String msg2, final Object object) {
        //Log.d("test", "onMessageCallback " + what + ", " + arg1 + ", " + arg2);
    }

    /*cfg field
    //   int     action;
  14         int     videowidth;
  15         int     videoheight;
  16         int     timeoutms;
  17         char*   defdir;
  18         char*   wenetfn;
  19         char*   unetbin;
  20         char*   unetparam;
  21         char*   unetmsk;
  22         char*   alphabin;
  23         char*   alphaparam;
  24         char*   cacertfn;
  25         char*   scrfdbin;
  26         char*   scrfdparam;
  27         char*   pfpldbin;
  28         char*   pfpldparam;
    */
    public native int createdigit(int taskid, Object msgcb);

    public native int releasedigit(int taskid);

    public native int config(String cfgtxt);

    public native int start();

    public native int stop();

    public native int netwav(String wavurl, float duration);

    public native int bgpic(String picfn);

    public native int drawmskpic(String picfn, String mskfn);

    public native int mskrstpic(String picfn, String mskfn, int[] arrbox, int index, String fgpic);

    public native int drawmskbuf(String picfn, String mskfn, byte[] arrbuf, byte[] mskbuf, int arrsize);

    public native int mskrstbuf(String picfn, String mskfn, int[] arrbox, int index, String fgpic, byte[] arrbuf, byte[] mskbuf, int arrsize);

    public native int drawonebuf(String picfn, byte[] arrbuf, int arrsize);

    public native int onerstbuf(String picfn, int[] arrbox, int index, byte[] arrbuf, int arrsize);

    public native int onewav(String wavfn, String dumpfn);

    public native int onepic(String picfn, String dumpbox);

    public native int drawpic(String picfn);

    public native int picrst(String picfn, int[] arrbox, int index, String dumpbox);

    public native int netrstpic(String picfn, int[] arrbox, int index, String dumpbox);

    public native int setOutputWindow(Surface surface);

    public native int onerst(int index, String dumpbox);

    public native int netrst(int index, String dumpbox);

    public native int reset();

    public native int initModel(String path);

    public native int startgpg(String picfn, String gpgfn);

    public native int stopgpg();

    public native int processmd5(int kind, String infn, String outfn);

    public interface Callback {
        void onMessageCallback(final int what, final int arg1, final long arg2, final String msg1,
                               final String msg2, final Object object);
    }

    static {
        System.loadLibrary("scrfdncnn");
    }
}
