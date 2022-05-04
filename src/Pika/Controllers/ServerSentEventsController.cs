using Microsoft.AspNetCore.Mvc;
using Pika.Core;
using ISession = Pika.Core.ISession;

namespace Pika.Controllers;

[ApiController]
[Route("sse")]
public class ServerSentEventsController : ControllerBase
{
    private readonly ILogger<ServerSentEventsController> _logger;
    private readonly ISession _session;

    public ServerSentEventsController(
        ILogger<ServerSentEventsController> logger,
        ISession session)
    {
        _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        _session = session ?? throw new ArgumentNullException(nameof(session));
    }

    [HttpGet("")]
    public async Task<IActionResult> StreamAsync()
    {
        using var subscriber = _session.CreateSubscriber();

        await foreach (var alert in subscriber.ReadAllAsync())
        {
            await HttpContext.Response.WriteAsync($"data: {alert.Message}\n");
            await HttpContext.Response.WriteAsync("\n");
            await HttpContext.Response.Body.FlushAsync();
        }

        return new EmptyResult();
    }
}
