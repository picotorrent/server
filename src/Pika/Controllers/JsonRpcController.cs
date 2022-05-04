using System.Text.Json.Nodes;
using System.Text.Json.Serialization;
using Microsoft.AspNetCore.Mvc;
using Pika.Core;
using ISession = Pika.Core.ISession;

namespace Pika.Controllers;

[ApiController]
[Route("jsonrpc")]
public sealed class JsonRpcController : ControllerBase
{
    private readonly ILogger<JsonRpcController> _logger;
    private readonly ISession _session;

    public record JsonRpcRequest(
        [property: JsonPropertyName("jsonrpc")]
        string Version,
        [property: JsonPropertyName("method")]
        string Method,
        [property: JsonPropertyName("id")]
        int Id,
        [property: JsonPropertyName("params")]
        JsonNode Parameters);

    public JsonRpcController(
        ILogger<JsonRpcController> logger, ISession session)
    {
        _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        _session = session ?? throw new ArgumentNullException(nameof(session));
    }

    [HttpPost("")]
    public async Task<IActionResult> ExecuteAsync([FromBody] JsonRpcRequest request)
    {
        _session.AddTorrent(
            new AddTorrentParams
            {
                SavePath = request.Method
            });

        return Ok(request);
    }
}