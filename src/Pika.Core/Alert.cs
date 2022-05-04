namespace Pika.Core;

public abstract class Alert
{
    public string Message { get; internal set; }
    public int Type { get; internal set; }
}

public sealed class SomeAlert : Alert
{
}
