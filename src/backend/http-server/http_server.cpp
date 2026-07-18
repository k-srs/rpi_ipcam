#include <drogon/drogon.h>

int main()
{
    drogon::app().registerHandler(
        "/api/v1/config",
        [](const drogon::HttpRequestPtr &,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            LOG_INFO << req->methodString() << " " << req->getPath();

            Json::Value body;
            body["status"] = "ok";
            auto response = drogon::HttpResponse::newHttpJsonResponse(body);
            callback(response);
        },
        {drogon::Get});

    drogon::app().registerHandler(
        "/api/v1/config",
        [](const drogon::HttpRequestPtr &request,
           std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
            LOG_INFO << req->methodString() << " " << req->getPath();

            const auto config = request->getJsonObject();
            if (config == nullptr || !config->isObject()) {
                auto response = drogon::HttpResponse::newHttpResponse();
                response->setStatusCode(drogon::k400BadRequest);
                callback(response);
                return;
            }

            auto response = drogon::HttpResponse::newHttpResponse();
            response->setStatusCode(drogon::k204NoContent);
            callback(response);
        },
        {drogon::Put});

    auto &app = drogon::app();

    app.addListener("0.0.0.0", 8080);
    app.setThreadNum(2);
    app.run();

    return 0;
}
