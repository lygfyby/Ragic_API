#include <Ragic_API.h>
void RagicAPI::Begin(const char *Authorization)
{
    csHTTPClient.setAuthorization(Authorization);
    csHTTPClient.setTimeout(1500);
}
void RagicAPI::Begin(const char *uesr, const char *password)
{
    csHTTPClient.setAuthorization(uesr, password);
    csHTTPClient.setTimeout(1500);
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
    bool testsw = true;
    int16_t httpCode = NO_connect;

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
        if (testsw)
        {
            Serial.println(WEB);
            serializeJson(injson, Serial);
            Serial.println();
        }
        httpCode = csHTTPClient.POST(strPOST);

        if (httpCode == HTTP_CODE_OK)
        {
            // DynamicJsonDocument returndoc(parm.jsSize);
            JsonDocument filter;
            filter["ragicId"] = true;
            filter["status"] = true;
            // DeserializationError error = deserializeJson(*parm.json, csHTTPClient.getStream(), DeserializationOption::Filter(filter));
            DeserializationError error = deserializeJson(*parm.json, csHTTPClient.getStream());
            if (error)
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "反序列化失敗:%s\n", error.c_str());
                if (testsw)
                {
                    serializeJsonPretty(*parm.json, Serial);
                    Serial.println();
                }
                // return -1;
            }
            if ((*parm.json).as<JsonObject>()["status"].as<String>() != "SUCCESS")
            {
                if (testsw)
                {
                    serializeJsonPretty(*parm.json, Serial);
                    Serial.println();
                }
                if (_CONSOLE_PRINT_LEVEL >= _PRINT_LEVEL_WARNING)
                {
                    _CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, "寫入失敗!");
                    // serializeJsonPretty(*parm.json, Serial);
                    //  String strError = "";
                    // serializeJsonPretty(injson, strError);
                    //_CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, strError);
                    //  serializeJsonPretty((*parm.json), strError);
                }
                httpCode = OTHER_ERROR;
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
    int16_t httpCode = NO_connect;
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
    bool testsw = true;
    int16_t httpCode = NO_connect;
    JsonObject obj = (*objIn);
    if (!obj.containsKey("web") || !obj.containsKey("data"))
    {
        _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "參數缺失!\n");
        serializeJsonPretty(obj, Serial);
        return PARAMETER_MISSING;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        testsw = obj.containsKey("test");
        String WEB = obj["web"].as<String>() +
                     ((obj.containsKey("id") && obj["id"].as<uint32_t>() != 0) ? '/' + obj["id"].as<String>() : "");
        WEB += "?api";
        WEB += (!obj.containsKey("doFormula") || obj["doFormula"].as<bool>()) ? "&doFormula=true" : "";
        WEB += (!obj.containsKey("doDefaultValue") || obj["doDefaultValue"].as<bool>()) ? "&doDefaultValue=true" : "";
        WEB += (!obj.containsKey("doLinkLoad") || obj["doLinkLoad"].as<bool>()) ? "&doLinkLoad=true" : "&doLinkLoad=first";
        WEB += (!obj.containsKey("doWorkflow") || obj["doWorkflow"].as<bool>()) ? "&doWorkflow=true" : "";
        WEB += (!obj.containsKey("checkLock") || obj["checkLock"].as<bool>()) ? "&checkLock=true" : "";
        csHTTPClient.begin(WEB);
        csHTTPClient.addHeader("Content-Type", "application/json");
        if (testsw)
        {
            serializeJsonPretty(obj, Serial);
            //_CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "WEB:%s\nDATA:%s\n", WEB.c_str(), obj["data"].as<const char *>());
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "WEB:%s\n", WEB.c_str());
        }

        httpCode = csHTTPClient.POST(obj["data"].as<String>());

        if (httpCode == HTTP_CODE_OK)
        {
            // DynamicJsonDocument returndoc(parm.jsSize);
            JsonDocument filter;
            filter["ragicId"] = true;
            filter["status"] = true;
            //FIXME 不知為何直接deserializeJson(obj["return"]) 會跳出記憶體不足錯誤
            JsonDocument testdoc;
            DeserializationError error = deserializeJson(testdoc, csHTTPClient.getStream());
            JsonObject objReturn = testdoc.as<JsonObject>();
            obj["return"].set(objReturn);
            if (error)
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "反序列化失敗:%s\n", error.c_str());
                if (testsw)
                {
                    serializeJsonPretty(objReturn, Serial);
                    Serial.println();
                }
                // return -1;
            }
            if (objReturn["status"].as<String>() != "SUCCESS")
            {
                if (testsw)
                {
                    serializeJsonPretty(objReturn, Serial);
                    Serial.println();
                }
                if (_CONSOLE_PRINT_LEVEL >= _PRINT_LEVEL_WARNING)
                {
                    _CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, "寫入失敗!");
                    // serializeJsonPretty(*parm.json, Serial);
                    //  String strError = "";
                    // serializeJsonPretty(injson, strError);
                    //_CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, strError);
                    //  serializeJsonPretty((*parm.json), strError);
                }
                httpCode = OTHER_ERROR;
            }
            else
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_INFO, "OK!ID=%s\n", objReturn["ragicId"].as<String>().c_str());
            }
        }
        else
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "HTTP ERROR:%s\n", HTTPClient::errorToString(httpCode).c_str());
    }
    csHTTPClient.end();
    return httpCode;
}
