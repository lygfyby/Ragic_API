#include <Ragic_API.h>

void RagicAPI::Begin(const char *Authorization)
{
    csHTTPClient.setAuthorization(Authorization);
}
void RagicAPI::Begin(const char *uesr, const char *password)
{
    csHTTPClient.setAuthorization(uesr, password);
}
void RagicAPI::setTimeOut(uint16_t value)
{
    intTimeOut = value;
}
int16_t RagicAPI::writeList_Json(JsonObject *objIn)
{
    int16_t httpCode = HTTP_CODE_OK;
    JsonObject obj = (*objIn);
    bool testsw = true;
    testsw = !obj["test"].isNull();
    if (testsw)
        serializeJsonPretty(obj, Serial);

    if (obj["web"].isNull() || obj["data"].isNull())
    {
        _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "參數缺失!\n");
        serializeJsonPretty(obj, Serial);
        return ERROR_CODE_PARAMETER_MISSING;
    }
    if (!isConnect(Function_e ::FUNCTION_CODE_HAVE_IP))
    {
        _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "網路錯誤!\n");
        return ERROR_CODE_WIFI_DISCONNECTED;
    }
    else
    {

        String WEB = obj["web"].as<String>() +
                     ((obj["id"].isNull() && obj["id"].as<uint32_t>() != 0) ? '/' + obj["id"].as<String>() : "");
        WEB += "?api";
        WEB += (obj["doFormula"].isNull() || !obj["doFormula"].as<bool>()) ? "&doFormula=true": "" ;
        WEB += (obj["doDefaultValue"].isNull() || !obj["doDefaultValue"].as<bool>()) ? "&doDefaultValue=true":"" ;
        WEB += (obj["doLinkLoad"].isNull() || !obj["doLinkLoad"].as<bool>()) ?  "&doLinkLoad=true":"&doLinkLoad=first" ;
        WEB += (obj["doWorkflow"].isNull() || !obj["doWorkflow"].as<bool>()) ?  "&doWorkflow=true":"" ;
        WEB += (obj["checkLock"].isNull() || !obj["checkLock"].as<bool>()) ?  "&checkLock=true":"" ;

        csHTTPClient.begin(WEB);
        csHTTPClient.setTimeout(intTimeOut);
        csHTTPClient.addHeader("Content-Type", "application/json");
        if (testsw)
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "WEB:%s\n", WEB.c_str());

        httpCode = csHTTPClient.POST(obj["data"].as<String>());

        if (httpCode != HTTP_CODE_OK)
        {
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "HTTP ERROR:%s\n", HTTPClient::errorToString(httpCode).c_str());
            return httpCode;
        }
        else
        {
            JsonDocument docReturn;
            DeserializationError error;
            if (!obj["filter"].isNull())
            {
                JsonObject filter = obj["filter"];
                error = deserializeJson(docReturn, csHTTPClient.getString(), DeserializationOption::Filter(filter));
            }
            else
            {
                error = deserializeJson(docReturn, csHTTPClient.getString());
            }
            JsonObject objReturn = docReturn.as<JsonObject>();
            obj["return"] = objReturn;
            if (error)
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "反序列化失敗:%s\n", error.c_str());
                serializeJsonPretty(objReturn, Serial);
                Serial.println();
                return ERROR_CODE_DESERIALIZE_ERROR;
            }
            if (testsw)
            {
                serializeJsonPretty(objReturn, Serial);
                Serial.println();
            }
            if (objReturn["status"].as<String>() != "SUCCESS")
            {
                if (testsw)
                    _CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, "寫入失敗!");
                serializeJsonPretty(objReturn, Serial);
                Serial.println();
                httpCode = HTTPC_ERROR_STREAM_WRITE;
            }
            else
            {
                if (testsw)
                    _CONSOLE_PRINTF(_PRINT_LEVEL_INFO, "寫入成功!ID=%d\n", objReturn["ragicId"].as<uint32_t>());
            }
        }
    }

    csHTTPClient.end();
    return httpCode;
}
int16_t RagicAPI::readList_Json(JsonObject *objIn)
{
    int16_t httpCode = HTTP_CODE_OK;
    JsonObject obj = (*objIn);
    bool testsw = true;
    testsw = !obj["test"].isNull();
    if (testsw)
        serializeJsonPretty(obj, Serial);

    if (obj["web"].isNull())
    {
        _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "參數缺失!\n");
        serializeJsonPretty(obj, Serial);
        return ERROR_CODE_PARAMETER_MISSING;
    }
    if (!isConnect(Function_e ::FUNCTION_CODE_HAVE_IP))
    {
        _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "網路錯誤!\n");
        return ERROR_CODE_WIFI_DISCONNECTED;
    }
    else
    {

        String WEB = obj["web"].as<String>() +
                     ((obj["id"].isNull() && obj["id"].as<uint32_t>() != 0) ? '/' + obj["id"].as<String>() : "");
        WEB += "?api";
        WEB += (obj["naming"].isNull() || obj["naming"].as<bool>()) ?  "&naming=FNAME":"&naming=EID" ;
        WEB += (obj["subtables"].isNull() || obj["subtables"].as<bool>()) ? "&subtables=0": "" ;
        WEB += (obj["listing"].isNull() || obj["listing"].as<bool>()) ? "" : "&listing";
        WEB += (obj["info"].isNull() || obj["info"].as<bool>()) ? "" : "&info";
        WEB += (obj["approval"].isNull() || obj["approval"].as<bool>()) ? "" : "&approval";
        WEB += (obj["conversation"].isNull() || obj["conversation"].as<bool>()) ? "" : "&conversation";
        WEB += (obj["commen"].isNull() || obj["commen"].as<bool>()) ? "" : "&commen";
        WEB += (obj["ignoreMask"].isNull() || obj["ignoreMask"].as<bool>()) ? "" : "&ignoreMask";
        WEB += (obj["ignoreFixedFilter"].isNull() || obj["ignoreFixedFilter"].as<bool>()) ? "" : "&ignoreFixedFilter";
        WEB += (obj["reverse"].isNull() || obj["reverse"].as<bool>()) ? "" : "&reverse";
        WEB += (obj["bbcode"].isNull() || obj["bbcode"].as<bool>()) ? "" : "&bbcode";
        WEB += (obj["other"].isNull() || obj["other"].as<String>() == "") ? "" : "&" + obj["other"].as<String>();

        csHTTPClient.begin(WEB);
        csHTTPClient.setTimeout(intTimeOut);
        csHTTPClient.addHeader("Content-Type", "application/json");
        if (testsw)
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "WEB:%s\n", WEB.c_str());

        httpCode = csHTTPClient.GET();

        if (httpCode != HTTP_CODE_OK)
        {
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "HTTP ERROR:%s\n", HTTPClient::errorToString(httpCode).c_str());
            return ERROR_CODE_HTTP_ERROR;
        }
        else
        {
            JsonDocument docReturn;
            DeserializationError error;
            if (!obj["filter"].isNull())
            {
                JsonObject filter = obj["filter"];
                filter["status"] = true;
                filter["msg"] = true;
                filter["code"] = true;
                error = deserializeJson(docReturn, csHTTPClient.getString(), DeserializationOption::Filter(filter));
            }
            else
            {
                error = deserializeJson(docReturn, csHTTPClient.getString());
            }
            JsonObject objReturn = docReturn.as<JsonObject>();
            obj["return"] = objReturn; // 將objReturn的內容直接複製到obj["return"]
            if (error)
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "反序列化失敗:%s\n", error.c_str());
                serializeJsonPretty(objReturn, Serial);
                Serial.println();
                return ERROR_CODE_DESERIALIZE_ERROR;
            }
            if (!objReturn["status"].isNull())
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "讀取失敗\n");
                serializeJsonPretty(objReturn, Serial);
                Serial.println();
                return ERROR_CODE_OTHER_ERROR;
            }
            if (testsw)
            {
                serializeJsonPretty(objReturn, Serial);
                Serial.println();
            }
            docReturn.clear();
        }
    }

    csHTTPClient.end();
    return httpCode;
}
//[ ]待完善Read、Respond
//[@]兼容性需再提升
void taskRagic(void *pvParam)
{
    RagicAPI *ptrRagicAPI;
    if (pvParam != nullptr)
    {
        ptrRagicAPI = ((RagicAPI *)pvParam);
    }
    else
    {
        _CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, "輸入參數錯誤!");
        vTaskDelete(NULL);
    }

    const char *taskName = pcTaskGetName(NULL);

    JsonObject *objIN;
    while (!isConnect(FUNCTION_CODE_HAVE_IP))
    {
        if (xQueueReceive(ptrRagicAPI->queueRagic, &objIN, 0) == pdPASS)
            ;
        _DELAY_MS(100);
    }

    while (true)
    {
        if (xQueueReceive(ptrRagicAPI->queueRagic, &objIN, 0) == pdPASS)
        {
            if (!(*objIN)["Write"].isNull())
            {
                JsonObject obj = (*objIN)["Write"];
                int16_t returnCode = ptrRagicAPI->writeList_Json(&obj);
                if (returnCode <= 0)
                    _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "錯誤代碼:%d!\n", returnCode);
                (*objIN)["Respond"].to<JsonObject>()["status"] = returnCode;
            }
            else if (!(*objIN)["Read"].isNull())
            {
                JsonObject obj = (*objIN)["Read"];
                int16_t returnCode = ptrRagicAPI->readList_Json(&obj);
                (*objIN)["Respond"].to<JsonObject>()["status"] = returnCode;
            }
        }
        _DELAY_MS(100);
    }
}