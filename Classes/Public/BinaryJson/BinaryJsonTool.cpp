//
//  BinaryJsonTool.cpp
//  Underworld_Client
//
//  Created by burst on 16/4/15.
//
//

#include "BinaryJsonTool.h"
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "Utils.h"
#include "UWJsonHelper.h"

using namespace std;
using namespace rapidjson;

#define INT_BYTES (4)
#define INVALID   (-1)
#ifndef M_RETURN_FALSE_IFNOT
#define M_RETURN_FALSE_IFNOT(cond)   if(!(cond)) return false;
#endif //IF RETURN

static void encodeInt(int32_t value, string& out)
{
    out.append((char*)&value, INT_BYTES);
}

static void encodeString(const string& value, string& out)
{
    int32_t length = static_cast<int32_t>(value.size());
    encodeInt(length, out);
    out.append(value);
}

static void encode(const rapidjson::Value &root, string& out)
{
    rapidjson::Type type = root.GetType();
    switch (type) {
        case rapidjson::Type::kNumberType:
        {
            encodeInt(root.GetInt(), out);
        }
            break;
        case rapidjson::Type::kStringType:
        {
            encodeString(root.GetString(), out);
        }
            break;
        case Type::kFalseType:
        {
            out.append(1, 0);
        }
            break;
        case Type::kTrueType:
        {
            out.append(1, 1);
        }
            break;
        case Type::kArrayType:
        {
            encodeInt(root.Size(), out);
            for (auto iter = root.Begin(); iter != root.End(); ++iter) {
                const rapidjson::Value& child = *iter;
                encode(child, out);
            }
        }
            break;
        case Type::kObjectType:
        {
            for (auto iter = root.MemberBegin(); iter != root.MemberEnd(); ++iter) {
                const rapidjson::Value& child = iter->value;
                encode(child, out);
            }
        }
            break;
        default:
            break;
    }
}

class stream
{
private:
    int32_t pos = 0;
    const string& buf;
public:
    stream(const string& src)
    :buf(src)
    {
    }
    
    int read(string& dst, int32_t len)
    {
        if (pos + len > buf.size()) {
            return INVALID;
        }
        dst.assign(buf, pos, len);
        pos += len;
        return len;
    }
    
    int readByte(unsigned int &dst)
    {
        if (pos >= buf.size()) {
            return INVALID;
        }
        dst = buf.at(pos++);
        return 1;
    }
    
    void reset()
    {
        pos = 0;
    }
};

static bool decodeInt(stream& out, int32_t& dst)
{
    string tmp;
    if(out.read(tmp, INT_BYTES) == INVALID) {
        return false;
    }
    memcpy((char *)&dst, tmp.c_str(), INT_BYTES);
    return true;
}

static bool decodeString(stream& out, string& dst)
{
    int32_t length;
    if (!decodeInt(out, length) || length < 0) {
        return false;
    }
    return out.read(dst, length) != INVALID;
}

static void addChild(rapidjson::Value &child, rapidjson::Value &parent, rapidjson::Document::AllocatorType& allocator, const char* name)
{
    if (name) {
        parent.AddMember(StringRef(name), child, allocator);
    } else {
        parent.PushBack(child, allocator);
    }
}

static void printJson(const rapidjson::Value& node)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    node.Accept(writer);
    printf("[%s]\n", buffer.GetString());
}

static bool decode(stream& out, const rapidjson::Value &sample, rapidjson::Value &parent, rapidjson::Document::AllocatorType& allocator, const char* name = nullptr, bool outermost = false)
{
    rapidjson::Type type = sample.GetType();
    switch (type) {
        case rapidjson::Type::kNumberType:
        {
            rapidjson::Value ret(kNumberType);
            int32_t num;
            M_RETURN_FALSE_IFNOT(decodeInt(out, num));
            ret.SetInt(num);
            addChild(ret, parent, allocator, name);
        }
            break;
        case rapidjson::Type::kStringType:
        {
            rapidjson::Value ret(kStringType);
            string dst;
            M_RETURN_FALSE_IFNOT(decodeString(out, dst));
            ret.SetString(dst.c_str(), (int)dst.size(), allocator);
            addChild(ret, parent, allocator, name);
        }
            break;
        case Type::kFalseType:
        case Type::kTrueType:
        {
            rapidjson::Value ret(kTrueType);
            unsigned int b;
            M_RETURN_FALSE_IFNOT(out.readByte(b));
            ret.SetBool(b != 0);
            addChild(ret, parent, allocator, name);
        }
            break;
        case Type::kArrayType:
        {
            rapidjson::Value ret(kArrayType);
            const rapidjson::Value& child = *(sample.Begin());
            int len;
            M_RETURN_FALSE_IFNOT(decodeInt(out, len));
            for (; len > 0; len--) {
                M_RETURN_FALSE_IFNOT(decode(out, child, ret, allocator));
            }
            addChild(ret, parent, allocator, name);
        }
            break;
        case Type::kObjectType:
        {
            if (outermost) {
                for (auto iter = sample.MemberBegin(); iter != sample.MemberEnd(); ++iter) {
                    const rapidjson::Value& child = iter->value;
                    M_RETURN_FALSE_IFNOT(decode(out, child, parent, allocator, iter->name.GetString()));
                }
            } else {
                rapidjson::Value ret(kObjectType);
                for (auto iter = sample.MemberBegin(); iter != sample.MemberEnd(); ++iter) {
                    const rapidjson::Value& child = iter->value;
                    M_RETURN_FALSE_IFNOT(decode(out, child, ret, allocator, iter->name.GetString()));
                }
                addChild(ret, parent, allocator, name);
            }
        }
            break;
        default:
        {
            return false;
        }
            break;
    }
    return true;
}

BinaryJsonTool::BinaryJsonTool()
{
    
}

BinaryJsonTool::~BinaryJsonTool()
{
    clearTemplates();
}

void BinaryJsonTool::clearTemplates()
{
    for (map<int, const rapidjson::Value*>::iterator iter = templates.begin(); iter != templates.end(); ++iter) {
        const rapidjson::Value* jsonDict = iter->second;
        delete jsonDict;
    }
    templates.clear();
}

void BinaryJsonTool::initTemplates(const std::string& data, const string& keyField)
{
    clearTemplates();
    vector<string> lines;
    Utils::split(lines, data, "\n");
    for (int i = 0; i < lines.size(); ++i) {
        const string& str = lines.at(i);
        rapidjson::Document *jsonDict = new rapidjson::Document();
        jsonDict->Parse<0>(str.c_str());
//        if (jsonDict->HasParseError())
//        {
//            printf("GetParseError %u\n", jsonDict->GetParseError());
//        }
        int code = UWJsonHelper::getIntValue_json(*jsonDict, keyField.c_str(), -1);
        if (code > 0) {
            templates.insert(make_pair(code, jsonDict));
        }
    }
}

string BinaryJsonTool::encode(const rapidjson::Value &root) const
{
    string ret;
    ::encode(root, ret);
    return ret;
}


bool BinaryJsonTool::decode(const std::string &src, rapidjson::Document& document) const
{
    M_RETURN_FALSE_IFNOT(!src.empty())
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    document.SetObject();
    stream out(src);
    int code;
    M_RETURN_FALSE_IFNOT(decodeInt(out, code) && templates.find(code) != templates.end());
    const rapidjson::Value *sample = templates.at(code);
    out.reset();
    M_RETURN_FALSE_IFNOT(::decode(out, *sample, document, allocator, nullptr, true));
    return true;
}
