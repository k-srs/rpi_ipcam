#include <drogon/drogon.h>

int main()
{
    drogon::app().registerHandler(
        "/health",
        [](const drogon::HttpRequestPtr &,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            Json::Value body;
            body["status"] = "ok";

            callback(drogon::HttpResponse::newHttpJsonResponse(body));
        },
        {drogon::Get});

    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setThreadNum(2)
        .run();

    return 0;
}
