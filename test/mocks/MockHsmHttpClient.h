#pragma once

#include "ApplicationMiddleware.h"
#include "DistributedHsmService.h"
#include "WebApp.h"

class MockHsmMiddleware : public hestia::ApplicationMiddleware {
  public:
    hestia::HttpResponse::Ptr call(
        const hestia::HttpRequest& request,
        hestia::AuthorizationContext& user,
        hestia::HttpEvent,
        hestia::responseProviderFunc func)
    {
        user = m_test_user;
        return func(request);
    }

    hestia::AuthorizationContext m_test_user;
};

class MockHsmWebApp : public hestia::WebApp {
  public:
    MockHsmWebApp(hestia::DistributedHsmService* hsm_service);
};

class MockHsmHttpClient : public hestia::HttpClient {
  public:
    void add_app(
        hestia::DistributedHsmService* hsm_service, const std::string& address)
    {
        m_apps[address] = std::make_unique<MockHsmWebApp>(hsm_service);
    }

    void make_request(
        const hestia::HttpRequest& request,
        hestia::HttpClient::completionFunc completion_func,
        hestia::Stream* stream,
        std::size_t,
        progressFunc) override;

    std::unordered_map<std::string, std::unique_ptr<MockHsmWebApp>> m_apps;
};