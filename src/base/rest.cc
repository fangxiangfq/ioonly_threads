#include "rest.h"

namespace Rest
{
    MsgMap JsonBuilder::msgmap_;

    JsonParser::JsonParser(std::string& body) 
    {
        Json::Reader reader;
        parse_ = reader.parse(body, root_);
    }
    
    bool JsonParser::createParse(std::string& terno, std::string& ip, uint16_t& port) 
    {
        if(!deleteParse(terno))
            return false;
        Json::Value tmp = root_["ip"];
        if(!tmp || !tmp.isString())
        {
            return false;
        }

        ip = tmp.asString();
        tmp = Json::Value::null;
        tmp = root_["port"];
        if(!tmp || !tmp.isUInt())
        {
            return false;
        }

        port = static_cast<uint16_t>(tmp.asUInt());

        return port;
    }
    
    bool JsonParser::deleteParse(std::string& terno) 
    {
        if(!parse_)
            return parse_;
        Json::Value tmp = root_["terno"];
        if(!tmp || !tmp.isString())
        {
            return false;
        }
        else
        {
            terno = tmp.asString();
            return true;
        }
    }
    
    bool JsonParser::routeParse(std::string& src, std::vector<std::string>& dst) 
    {
        if(!deleteParse(src))
            return false;
        Json::Value arr = root_["dst"];
        if(!arr || !arr.isArray())
        {
            return false;
        }
        else
        {
            dst.clear();
            for(unsigned int i = 0 ; i < arr.size(); ++i)
            {
                if(!arr[i].isString())
                    return false;
                dst.push_back(arr[i].asString());
            }
            return true;
        }
    }
    
    JsonBuilder::JsonBuilder(const Code& code) 
    {
        root_["code"] = static_cast<int>(code);
        root_["msg"] = msgmap_[code];
    }
    
    //todo change to templates
    JsonBuilder::JsonBuilder(const Code& code, const std::string& key, const uint16_t& value) 
    {
        root_["code"] = static_cast<int>(code);
        root_["msg"] = msgmap_[code];

        Json::Value data;
        data[key] = value;

        root_["data"] = data;
    }
    
    JsonBuilder::JsonBuilder(const Code& code, const std::string& key, const std::string& str) 
    {
        root_["code"] = static_cast<int>(code);
        root_["msg"] = msgmap_[code];

        Json::Value data;
        data[key] = str;

        root_["data"] = data;
    }
    
    std::string JsonBuilder::toString() 
    {
        Json::FastWriter writer;
        return writer.write(root_);
    }
}