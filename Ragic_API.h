/**
 * @file Ragic_API.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-03-27
 *
 * @copyright Copyright (c) 2025
 * @note Postman 測試時需在Headers 新增Key:Authorization,Value:Basic ${API-KEY}
 */
#ifndef RAGIC_API_H
#define RAGIC_API_H
#include <Template.h>
#define _E2S(x) #x
class RagicAPI
{
private:
    HTTPClient csHTTPClient;
    String csAuthorization;
    String csStr;
    uint16_t intTimeOut = 1500;

public:
    // String csFrom;
    // uint32_t csID;
    // JsonObject csJsojt;
    // DynamicJsonDocument *csdoc;
    typedef struct GET_Parameters_t
    {
        bool subtables = false;         // 是否讀取子表格(0/1)
        bool naming = false;            // 是否用數字作為key("EID"=數字，"FNAME"=標題名稱);
        bool listing = false;           // 是否只讀取列表頁有的資料("false"/"true")
        bool info = false;              // 是否連帶讀取建立修改時間、人("false"/"true")
        bool approval = false;          // 是否連帶讀取審核資料(""/"coufig")//?會自動將人名轉換成郵件
        bool conversation = false;      // 是否連帶讀取郵件往來紀錄(""/"coufig")
        bool commen = false;            // 是否連帶讀取對話紀錄(""/"coufig")
        bool ignoreMask = false;        // 是否完整顯示密碼欄位(""/"coufig")
        bool ignoreFixedFilter = false; // 是否取消固定篩選(""/"coufig")//*需有SYSAdmin權限
        bool reverse = false;           // 反轉資料進來的順序(false=新先舊後/true=舊先新後)//?無效?
        bool bbcode = false;            // 是否用BBCODE翻譯(false=HTML/true=BBCODE)//?沒測


        String other = "";              // 其他自訂義Parameter
        String web = "";
        uint16_t id = 0;
        void ParametersToJson(JsonObject &obj) 
        {
            obj["subtables"] = subtables;
            obj["naming"] = naming;
            obj["listing"] = listing;
            obj["info"] = info;
            obj["approval"] = approval;
            obj["conversation"] = conversation;
            obj["commen"] = commen;
            obj["ignoreMask"] = ignoreMask;
            obj["ignoreFixedFilter"] = ignoreFixedFilter;
            obj["reverse"] = reverse;
            obj["bbcode"] = bbcode;
        }
        // 從 JSON 中讀取資料到 struct
        void ParametersFromJson( JsonObject &obj)
        {
             subtables=obj["subtables"];
             naming=obj["naming"];
             listing=obj["listing"];
             info=obj["info"];
             approval=obj["approval"];
             conversation=obj["conversation"];
             commen=obj["commen"];
             ignoreMask=obj["ignoreMask"];
             ignoreFixedFilter=obj["ignoreFixedFilter"];
             reverse=obj["reverse"];
             bbcode=obj["bbcode"];
        }
    } GET_Parameters_t;
    typedef struct POST_Parameters_t
    {
        bool doFormula = true;      // 變更/新建前是否先進行公式重算[false/true]//!如果為是則工作流不會執行
        bool doDefaultValue = true; // 變更/新建前是否先載入預設值[false/true]
        bool doLinkLoad = true;     // 變更/新建前是否先計算公式再載入連結[true/first]
        bool doWorkflow = true;     // 是否執行相關的工作流[false/true]
        bool checkLock = true;      // 是否略過上鎖的資料[false/true]
        String other = "";
        String web = "";
        uint32_t id = 0;
        // 將 struct 資料寫入 JSON
        void ParametersToJson(JsonObject &obj) 
        {
            obj["doFormula"] = doFormula;
            obj["doDefaultValue"] = doDefaultValue;
            obj["doLinkLoad"] = doLinkLoad;
            obj["doWorkflow"] = doWorkflow;
            obj["checkLock"] = checkLock;
        }
        // 從 JSON 中讀取資料到 struct
        void ParametersFromJson( JsonObject &obj)
        {
            doFormula = obj["doFormula"];
            doDefaultValue = obj["doDefaultValue"];
            doLinkLoad = obj["doLinkLoad"];
            doWorkflow = obj["doWorkflow"];
            checkLock = obj["checkLock"];
        }
    } POST_Parameters_t;

    static const enum ERROR_CODE_e {
        ERROR_CODE_HTTP_ERROR = -100,
        ERROR_CODE_WIFI_DISCONNECTED,
        ERROR_CODE_PARAMETER_MISSING,
        ERROR_CODE_DESERIALIZE_ERROR,
        ERROR_CODE_SIZE_TOBIG,
        ERROR_CODE_OTHER_ERROR
    } ERROR_CODE_e;

    QueueHandle_t queueRagic = xQueueCreate(1, sizeof(JsonObject *));
    String RAGIC_USER = "T3F5bHNOQk9uNlZnaFA4SEpwSmZtQ3RFWWR2bWFodjJhL204MlU5RmcvRXBZckswUGxtZjBveGtKZzdtSnpJQi8vUFA3T0FKNHFrPQ==";

    void Begin(const char *Authorization);

    void Begin(const char *uesr, const char *password);
    void setTimeOut(uint16_t value);

    int16_t writeList_Json(JsonObject *objIn);
    int16_t readList_Json(JsonObject *objIn);
};
void taskRagic(void *pvParam);
#endif