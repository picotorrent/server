using System.Runtime.InteropServices;
using System.Text;

namespace Pika.Core;

public static class NativeMethods
{
    private const string LibPikaCore = "/Users/viktor/code/picotorrent/server/lib/cmake-build-release/libpikacore.dylib";

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct lt_alert
    {
        [MarshalAs(UnmanagedType.LPStr)]
        public string message;

        public int type;
    }

    public delegate void AlertNotify();
    public delegate void AlertRead(IntPtr alert);

    [DllImport(LibPikaCore)]
    public static extern IntPtr lt_session_create();

    [DllImport(LibPikaCore)]
    public static extern void lt_session_destroy(IntPtr handle);

    [DllImport(LibPikaCore)]
    public static extern void lt_session_set_alert_notify(
        IntPtr handle, AlertNotify notify);

    [DllImport(LibPikaCore)]
    public static extern void lt_session_pop_alerts(
        IntPtr handle, AlertRead cb);

    [DllImport(LibPikaCore)]
    public static extern bool lt_session_wait_for_alert(IntPtr handle);

    [DllImport(LibPikaCore)]
    public static extern IntPtr lt_atp_create();

    [DllImport(LibPikaCore)]
    public static extern void lt_atp_destroy(IntPtr handle);

    //[DllImport(LibPikaCore, CharSet = CharSet.Ansi)]
    //public static extern void lt_atp_savepath_get(IntPtr handle, out StringBuilder data);

    [DllImport(LibPikaCore, CharSet = CharSet.Unicode)]
    public static extern void lt_atp_savepath_set(
        IntPtr handle,
        string data);
}
