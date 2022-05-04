namespace Pika.JsonRpc;

public sealed class SessionAddTorrent
    : Method<SessionAddTorrent.Request, SessionAddTorrent.Response>
{
    public class Request {}

    public class Response {}

    public override Task<Response> Execute(Request req)
    {
        throw new NotImplementedException();
    }
}
