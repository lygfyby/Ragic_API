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
    bool testsw = false;
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
        serializeJson(injson, strPOST);
        httpCode = csHTTPClient.POST(strPOST);
        if (httpCode == HTTP_CODE_OK)
        {
            // DynamicJsonDocument returndoc(parm.jsSize);
            StaticJsonDocument<200> filter;
            filter["ragicId"] = true;
            filter["status"] = true;
            if (testsw)
            {
                WiFiClient *stream = csHTTPClient.getStreamPtr();
                while (stream->available())
                    Serial.write(stream->read());
            }
            DeserializationError error = deserializeJson(*parm.json, csHTTPClient.getStream(), DeserializationOption::Filter(filter));
            if (error)
            {
                _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "反序列化失敗:%s\n", error.c_str());

                // return -1;
            }
            if ((*parm.json).as<JsonObject>()["status"].as<String>() != "SUCCESS")
            {
                if (_CONSOLE_PRINT_LEVEL >= _PRINT_LEVEL_WARNING)
                {
                    _CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, "寫入失敗!");
                    String strError = "";
                    serializeJson(injson, strError);
                    _CONSOLE_PRINTLN(_PRINT_LEVEL_WARNING, strError);
                    serializeJsonPretty((*parm.json), strError);
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
                WiFiClient *stream = csHTTPClient.getStreamPtr();
                while (stream->available())
                    Serial.write(stream->read());
                Serial.println("END");
            }
        }
        else
            _CONSOLE_PRINTF(_PRINT_LEVEL_WARNING, "HTTP ERROR:%s\n", HTTPClient::errorToString(httpCode).c_str());
    }
    csHTTPClient.end();
    return httpCode;
}
