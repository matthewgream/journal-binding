#include <napi.h>
#include <systemd/sd-journal.h>
#include <string>

class JournalReader : public Napi::ObjectWrap<JournalReader> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "JournalReader", {
            InstanceMethod("open", &JournalReader::Open),
            InstanceMethod("addMatch", &JournalReader::AddMatch),
            InstanceMethod("next", &JournalReader::Next),
            InstanceMethod("seekTail", &JournalReader::SeekTail),
            InstanceMethod("close", &JournalReader::Close),
        });
        
        exports.Set("JournalReader", func);
        return exports;
    }
    
    JournalReader(const Napi::CallbackInfo& info) : Napi::ObjectWrap<JournalReader>(info) {
        j = nullptr;
    }
    
    ~JournalReader() {
        if (j) sd_journal_close(j);
    }

private:
    sd_journal* j;
    
    Napi::Value Open(const Napi::CallbackInfo& info) {
        if (sd_journal_open(&j, SD_JOURNAL_LOCAL_ONLY) < 0) {
            return Napi::Boolean::New(info.Env(), false);
        }
        return Napi::Boolean::New(info.Env(), true);
    }
    
    Napi::Value AddMatch(const Napi::CallbackInfo& info) {
        if (!j) return Napi::Boolean::New(info.Env(), false);
        
        std::string match = info[0].As<Napi::String>().Utf8Value();
        sd_journal_add_match(j, match.c_str(), match.length());
        return Napi::Boolean::New(info.Env(), true);
    }
    
    Napi::Value SeekTail(const Napi::CallbackInfo& info) {
        if (!j) return Napi::Boolean::New(info.Env(), false);
        sd_journal_seek_tail(j);
        return Napi::Boolean::New(info.Env(), true);
    }
    
    Napi::Value Next(const Napi::CallbackInfo& info) {
        if (!j) return info.Env().Null();
        
        if (sd_journal_next(j) <= 0) {
            return info.Env().Null();
        }
        
        Napi::Object entry = Napi::Object::New(info.Env());
        
        const void* data;
        size_t length;
        SD_JOURNAL_FOREACH_DATA(j, data, length) {
            std::string field((const char*)data, length);
            size_t eq = field.find('=');
            if (eq != std::string::npos) {
                std::string key = field.substr(0, eq);
                std::string value = field.substr(eq + 1);
                entry.Set(key, value);
            }
        }
        
        return entry;
    }
    
    Napi::Value Close(const Napi::CallbackInfo& info) {
        if (j) {
            sd_journal_close(j);
            j = nullptr;
        }
        return info.Env().Undefined();
    }
};

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    return JournalReader::Init(env, exports);
}

NODE_API_MODULE(journal, Init)
