#include <DFRobot_HTTPClient.h>

#if defined(ARDUINO_ARCH_ESP8266)
std::unique_ptr<BearSSL::WiFiClientSecure>_wificlient_http(new BearSSL::WiFiClientSecure);
#endif

void DFRobot_HTTPClient::init()
{
    _params = "";
    _body = "";
    _httpcode = 0;
#if defined(ARDUINO_ARCH_ESP8266)
    _wificlient_http->setInsecure();
#endif
}

void DFRobot_HTTPClient::addParam(const String& name, const String& value)
{
    if (_params == "")
        _params = _params + "?" + name + "=" + value;
    else
        _params = _params + "&" + name + "=" + value;
}

void DFRobot_HTTPClient::addHeader(const String& name, const String& value)
{
    _httpclient.addHeader(name, value);
}

void DFRobot_HTTPClient::addString(const String& text)
{
    _body += text;
}

void DFRobot_HTTPClient::GET(String url, float timeout)
{
    _httpclient.setTimeout(timeout);
#if defined(ARDUINO_ARCH_ESP8266)
    _httpclient.begin(*_wificlient_http, url + _params);
#else
    _httpclient.begin(url + _params);
#endif
    _httpcode = _httpclient.GET();
}

void DFRobot_HTTPClient::POST(String url, float timeout)
{
    _httpclient.setTimeout(timeout);
#if defined(ARDUINO_ARCH_ESP8266)
    _httpclient.begin(*_wificlient_http, url);
#else
    _httpclient.begin(url);
#endif
    _httpcode = _httpclient.POST(_params.substring(1) + "\r\n" + _body);
}

String DFRobot_HTTPClient::getLine()
{
    return (_httpcode > 0) ? _httpclient.getLine() : "Error on HTTP request";
}

String DFRobot_HTTPClient::getString()
{
    return (_httpcode > 0) ? _httpclient.getString() : "Error on HTTP request";
}
