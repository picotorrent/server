namespace Pika.JsonRpc;

public abstract class Method<TIn, TOut>
    where TIn : class, new()
    where TOut : class, new()
{
    public abstract Task<TOut> Execute(TIn req);
}
