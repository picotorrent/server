using System.Runtime.InteropServices;
using System.Threading.Channels;

namespace Pika.Core;

public interface ISessionSubscriber : IDisposable
{
    IAsyncEnumerable<Alert> ReadAllAsync();
}

internal sealed class SessionSubscriber : ISessionSubscriber
{
    private readonly ChannelReader<Alert> _channelReader;
    private readonly Action _disposer;

    public SessionSubscriber(ChannelReader<Alert> channelReader, Action disposer)
    {
        _channelReader = channelReader ?? throw new ArgumentNullException(nameof(channelReader));
        _disposer = disposer ?? throw new ArgumentNullException(nameof(disposer));
    }

    public void Dispose()
    {
        _disposer.Invoke();
    }

    public IAsyncEnumerable<Alert> ReadAllAsync()
    {
        return _channelReader.ReadAllAsync();
    }
}

public interface ISession
{
    ISessionSubscriber CreateSubscriber();

    static ISession New() => new Session();
}

internal class Session : ISession, IDisposable
{
    private readonly IntPtr _handle;
    private readonly Task _alerter;
    private readonly HashSet<Channel<Alert>> _alertChannels = new();

    public Session()
    {
        _handle = NativeMethods.lt_session_create();
        _alerter = Task.Factory.StartNew(ReadAlerts);
    }

    public ISessionSubscriber CreateSubscriber()
    {
        var channel = Channel.CreateUnbounded<Alert>();
        _alertChannels.Add(channel);
        return new SessionSubscriber(
            channel.Reader,
            () => _alertChannels.Remove(channel));
    }

    private void ReleaseUnmanagedResources()
    {
        NativeMethods.lt_session_destroy(_handle);
    }

    public void Dispose()
    {
        ReleaseUnmanagedResources();
        GC.SuppressFinalize(this);
    }

    ~Session()
    {
        ReleaseUnmanagedResources();
    }

    private Task ReadAlerts()
    {
        while (true)
        {
            if (NativeMethods.lt_session_wait_for_alert(_handle))
            {
                NativeMethods.lt_session_pop_alerts(
                    _handle,
                    PopAlertsCallback);
            }
        }
    }

    private async void PopAlertsCallback(IntPtr ptr)
    {
        var alert = Marshal.PtrToStructure<NativeMethods.lt_alert>(ptr);
        foreach (var channel in _alertChannels)
        {
            await channel.Writer.WriteAsync(new SomeAlert {Message = alert.message, Type = alert.type});
        }
    }
}
