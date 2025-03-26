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
/**
 * @brief 修改資料
 *
 * @param web 網址列
 * @param id 資料的ID編號
 * @param value API內文
 * @return int 如果成功回傳200
 */
int16_t RagicAPI::writeList_Json(JsonObject &injson, POST_Parameters_t &parm)
{
    bool testsw = false;
    int16_t httpCode = ERROR_CODE_HTTP_ERROR;

    if (WiFi.status() == WL_CONNECTED)
    {
        String WEB = (parm.web) + (parm.id != 0 ? '/' + (String)parm.id : "");
        WEB += "?api";
        WEB += parm.doFormula ? "&doFormula=true" : "";
        WEB += parm.doDefaultValue ? "&doDefaultValue=true" : "";
        WEB += parm.doLinkLoad ? "&doLinkLoad=true" : "&doLinkLoad=first";
        WEB += parm.doWorkflow ? "&doWorkflow=true" : "";
        WEB += parm.checkLock ? "&checkLock=true" : "";
        csHTTPClient.begin(WEB);
        csHTTPClient.addHeader("Content-Type", "application/json");
        String strPOST = "";
        serializeJson(injson, strPOST);        
        if (testsw)
        {
            Serial.println(WEB);
            Serial.println(strPOST);
        }
        httpCode = csHTTPClient.POST(strPOST);

        if (httpCode == HTTP_CODE_OK)
        {
            // DynamicJsonDocument returndoc(parm.jsSize);
            JsonDocument filter;
            filter["ragicId"] = true;
            filter["status"] = true;
            //不知為何不能直接透過csHTTPClient.getStream()取得資料
             DeserializationError error = deserializeJson(*parm.json, csHTTPClient.getString(), DeserializationOption::Filter(filter));
            if (error)
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "反序列化失敗:%s\n", error.c_str());
                if (testsw)
                {
                    serializeJsonPretty(*parm.json, Serial);
                    Serial.println();
                    //Serial.println(str);
                }
                // return -1;
            }
            if ((*parm.json).as<JsonObject>()["status"].as<String>() != "SUCCESS")
            {
                if (testsw)
                {
                    serializeJson(injson, Serial);
                    Serial.println();
                    serializeJsonPretty(*parm.json, Serial);
                    Serial.println();
                }
                if (_CONSOLE_PRINT_LEVEL >= _PRINT_LEVEL_WARNING)
                {
                    _CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, "寫入失敗!");
                    //  String strError = "";
                    // serializeJsonPretty(injson, strError);
                    //_CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, strError);
                    //  serializeJsonPretty((*parm.json), strError);
                }
                httpCode = ERROR_CODE_OTHER_ERROR;
            }
            else
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_INFO, "OK!ID=%s\n", (*parm.json).as<JsonObject>()["ragicId"].as<String>().c_str());
            }
        }
        else
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "HTTP ERROR:%s\n", HTTPClient::errorToString(httpCode).c_str());
    }
    csHTTPClient.end();
    return httpCode;
}
int16_t RagicAPI::readList_Json(GET_Parameters_t &parm)
{
    bool testsw = false;
    int16_t httpCode = ERROR_CODE_HTTP_ERROR;
    if (WiFi.status() == WL_CONNECTED)
    {
        String WEB = (parm.web) + (parm.id != 0 ? '/' + (String)parm.id : "");
        WEB += "?api";
        parm.naming ? WEB += "&naming=FNAME" : WEB += "&naming=EID";
        if (!parm.subtables)
            WEB += "&subtables=0";
        if (parm.listing)
            WEB += "&listing"; // 是否只讀取列表頁有的資料(""/"coufig")
        if (parm.info)
            WEB += "&info"; // 是否連帶讀取建立修改時間、人(""/"coufig")
        if (parm.approval)
            WEB += "&approval"; // 是否連帶讀取審核資料(""/"coufig")//?會自動將人名轉換成郵件
        if (parm.conversation)
            WEB += "&conversation"; // 是否連帶讀取郵件往來紀錄(""/"coufig")
        if (parm.commen)
            WEB += "&commen"; // 是否連帶讀取對話紀錄(""/"coufig")
        if (parm.ignoreMask)
            WEB += "&ignoreMask"; // 是否完整顯示密碼欄位(""/"coufig")
        if (parm.ignoreFixedFilter)
            WEB += "&ignoreFixedFilter"; // 是否取消固定篩選(""/"coufig")//*需有SYSAdmin權限
        if (parm.reverse)
            WEB += "&reverse"; // 反轉資料進來的順序(false=新先舊後/true=舊先新後)//?無效?
        if (parm.bbcode)
            WEB += "&bbcode"; // 是否用BBCODE翻譯(false=HTML/true=BBCODE)//?沒測
        if (parm.other != "")
            WEB += '&' + parm.other;
        if (testsw)
        {
            _CONSOLE_PRINTF(_PRINT_LEVEL_INFO, "WEB=%s\n", WEB.c_str());
        }
        csHTTPClient.begin(WEB);
        httpCode = csHTTPClient.GET();
        if (httpCode == HTTP_CODE_OK)
        {
            DeserializationError error = deserializeJson(*parm.json, csHTTPClient.getStream());
            if (error)
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "反序列化失敗:%s\n", error.c_str());

            if (testsw)
            {
                serializeJsonPretty(*parm.json, Serial);
                // WiFiClient *stream = csHTTPClient.getStreamPtr();
                // while (stream->available())
                //     Serial.write(stream->read());
                Serial.println();
            }
        }
        else
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "HTTP ERROR:%s\n", HTTPClient::errorToString(httpCode).c_str());
    }
    csHTTPClient.end();
    return httpCode;
}
int16_t RagicAPI::writeList_Json(JsonObject *objIn)
{
    int16_t httpCode = -1;
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
        WEB += (obj["doFormula"].isNull() || obj["doFormula"].as<bool>()) ? "&doFormula=true" : "";
        WEB += (obj["doDefaultValue"].isNull() || obj["doDefaultValue"].as<bool>()) ? "&doDefaultValue=true" : "";
        WEB += (obj["doLinkLoad"].isNull() || obj["doLinkLoad"].as<bool>()) ? "&doLinkLoad=true" : "&doLinkLoad=first";
        WEB += (obj["doWorkflow"].isNull() || obj["doWorkflow"].as<bool>()) ? "&doWorkflow=true" : "";
        WEB += (obj["checkLock"].isNull() || obj["checkLock"].as<bool>()) ? "&checkLock=true" : "";
        csHTTPClient.begin(WEB);
        csHTTPClient.setTimeout(intTimeOut);
        csHTTPClient.addHeader("Content-Type", "application/json");
        if (testsw)
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "WEB:%s\n", WEB.c_str());

        httpCode = csHTTPClient.POST(obj["data"].as<String>());

        if (httpCode != HTTP_CODE_OK)
        {
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "HTTP ERROR:%s\n", HTTPClient::errorToString(httpCode).c_str());
            return ERROR_CODE_HTTP_ERROR;
        }
        else
        {
            JsonDocument filter;
            filter["ragicId"] = true;
            filter["status"] = true;
            // FIXME待測試是否成功將回傳資料寫進傳入的Obj
            JsonDocument returnDoc;
            DeserializationError error = deserializeJson(returnDoc, csHTTPClient.getStream());
            JsonObject objReturn = returnDoc.as<JsonObject>();
            obj["return"].set(objReturn);
            returnDoc.clear();
            objReturn = obj["return"];
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
                httpCode = ERROR_CODE_OTHER_ERROR;
            }
            else
            {
                if (testsw)
                    _CONSOLE_PRINTF(_PRINT_LEVEL_INFO, "寫入成功!ID=%d\n", objReturn["ragicId"].as<uint32_t>());
            }
        }
    }

    csHTTPClient.end();
    return 1;
}

void taskRagic(void *pvParam)
{
    while (!isConnect(FUNCTION_CODE_HAVE_IP))
        _DELAY_MS(1000);
    JsonObject *objIN;
    RagicAPI *ptrRagic = (RagicAPI *)pvParam;

    while (true)
    {
        if (xQueueReceive(ptrRagic->queueRagic, &objIN, 0) == pdPASS)
        {
            // serializeJsonPretty((*objIN), Serial);
            if (!(*objIN)["Write"].isNull())
            {
                JsonObject obj = (*objIN)["Write"];
                int16_t returnCode = ptrRagic->writeList_Json(&obj);
                if (returnCode <= 0)
                    _CONSOLE_PRINTF(_PRINT_LEVEL_INFO, "錯誤代碼:%d!\n", returnCode);
            }
            else if (!(*objIN)["Read"].isNull())
            {
                JsonObject obj = (*objIN)["Read"];
            }
        }
        _DELAY_MS(50);
    }
}