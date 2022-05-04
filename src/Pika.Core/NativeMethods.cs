using System.Runtime.InteropServices;

namespace Pika.Core;

public static class NativeMethods
{
    private const string LibPikaCore = "/home/viktor/dev/picotorrent/server/lib/cmake-build-release/libpikacore.so";

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
}
