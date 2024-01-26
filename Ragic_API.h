#ifndef RAGIC_API_H
#define RAGIC_API_H
#include <Template.h>
class RagicAPI
{
private:
    HTTPClient csHTTPClient;
    String csAuthorization;
String csStr;
public:
    
    //String csFrom;
    //uint32_t csID;
    //JsonObject csJsojt;
    //DynamicJsonDocument *csdoc;
    typedef struct GET_Parameters_t
    {
        bool subtables = false;         //是否讀取子表格(0/1)
        bool naming = false;            //是否用名稱作為key("EID"=數字，"FNAME"=標題名稱);
        bool listing = false;           //是否只讀取列表頁有的資料(""/"coufig")
        bool info = false;              //是否連帶讀取建立修改時間、人(""/"coufig")
        bool approval = false;          //是否連帶讀取審核資料(""/"coufig")//?會自動將人名轉換成郵件
        bool conversation = false;      //是否連帶讀取郵件往來紀錄(""/"coufig")
        bool commen = false;            //是否連帶讀取對話紀錄(""/"coufig")
        bool ignoreMask = false;        //是否完整顯示密碼欄位(""/"coufig")
        bool ignoreFixedFilter = false; //是否取消固定篩選(""/"coufig")//*需有SYSAdmin權限
        bool reverse = false;           //反轉資料進來的順序(false=新先舊後/true=舊先新後)//?無效?
        bool bbcode = false;            //是否用BBCODE翻譯(false=HTML/true=BBCODE)//?沒測
        String other = "";
        String web="";
        uint16_t id=0;
        JsonDocument * json=0;
    } GET_Parameters_t;
    typedef struct POST_Parameters_t
    {
        bool doFormula = true;      //變更/新建前是否先進行公式重算[false/true]//!如果為是則工作流不會執行
        bool doDefaultValue = true; //變更/新建前是否先載入預設值[false/true]
        bool doLinkLoad = true;     //變更/新建前是否先計算公式再載入連結[true/first]
        bool doWorkflow = true;     //是否執行相關的工作流[false/true]
        bool checkLock = true;      //是否略過上鎖的資料[false/true]
        String other = "";
        String web="";
        uint16_t id=0;
        JsonDocument * json=0;
    } POST_Parameters_t;
    static const enum ERROR_CODE_e {
        NO_connect = -100,
        OTHER_ERROR=-1,
        SIZE_TOBIG
    } ERROR_CODE_e;
    void Begin(const char *Authorization);

    void Begin(const char *uesr, const char *password);

    int16_t writeList_Json(JsonObject& injson,  POST_Parameters_t &parm);

    int16_t readList_Json( GET_Parameters_t &parm);    
};
#endif