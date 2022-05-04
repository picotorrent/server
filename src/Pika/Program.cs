var builder = WebApplication.CreateBuilder(args);
builder.Services.AddControllers();
builder.Services.AddSingleton(_ => Pika.Core.ISession.New());

var app = builder.Build();
app.MapControllers();
await app.RunAsync();
