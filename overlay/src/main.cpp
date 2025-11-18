#define TESLA_INIT_IMPL
#define STBTT_STATIC
#include <tesla.hpp>
#include <string_view>
#include "minIni/minIni.h"

namespace {

constexpr auto CONFIG_PATH = "/config/fs-patch/config.ini";
constexpr auto LOG_PATH = "/config/fs-patch/log.ini";

auto does_file_exist(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    FsFile file{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsOpenFile(&fs, path_buf, FsOpenMode_Read, &file);
    fsFileClose(&file);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

auto create_dir(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsCreateDirectory(&fs, path_buf);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

struct ConfigEntry {
    ConfigEntry(const char* _section, const char* _key, bool default_value) :
        section{_section}, key{_key}, value{default_value} {
            this->load_value_from_ini();
        }

    void load_value_from_ini() {
        this->value = ini_getbool(this->section, this->key, this->value, CONFIG_PATH);
    }

    auto create_list_item(const char* text) {
        auto item = new tsl::elm::ToggleListItem(text, value);
        item->setStateChangedListener([this](bool new_value){
            this->value = new_value;
            ini_putl(this->section, this->key, this->value, CONFIG_PATH);
        });
        return item;
    }

    const char* const section;
    const char* const key;
    bool value;
};

class GuiOptions final : public tsl::Gui {
public:
    GuiOptions() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("fs-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("Options"));
        list->addItem(config_patch_emummc.create_list_item("Patch emuMMC"));
        list->addItem(config_logging.create_list_item("Logging"));
        list->addItem(config_version_skip.create_list_item("Version skip"));

        frame->setContent(list);
        return frame;
    }

    ConfigEntry config_patch_emummc{"options", "patch_emummc", true};
    ConfigEntry config_logging{"options", "enable_logging", true};
    ConfigEntry config_version_skip{"options", "version_skip", true};
};

class GuiToggle final : public tsl::Gui {
public:
    GuiToggle() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("fs-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("FS - 0100000000000000"));
        list->addItem(config_noncasigchk_old.create_list_item("noncasigchk_old"));
        list->addItem(config_noncasigchk_old2.create_list_item("noncasigchk_old2"));
        list->addItem(config_noncasigchk_new.create_list_item("noncasigchk_new"));
        list->addItem(config_nocntchk.create_list_item("nocntchk"));
        list->addItem(config_nocntchk2.create_list_item("nocntchk2"));

        list->addItem(new tsl::elm::CategoryHeader("LDR - 0100000000000001"));
        list->addItem(config_noacidsigchk.create_list_item("noacidsigchk"));

        frame->setContent(list);
        return frame;
    }

    // FS patches
    ConfigEntry config_noncasigchk_old{"fs", "noncasigchk_old", true};
    ConfigEntry config_noncasigchk_old2{"fs", "noncasigchk_old2", true};
    ConfigEntry config_noncasigchk_new{"fs", "noncasigchk_new", true};
    ConfigEntry config_nocntchk{"fs", "nocntchk", true};
    ConfigEntry config_nocntchk2{"fs", "nocntchk2", true};
    
    // LDR patches
    ConfigEntry config_noacidsigchk{"ldr", "noacidsigchk", true};
};

class GuiLog final : public tsl::Gui {
public:
    GuiLog() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("fs-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        if (does_file_exist(LOG_PATH)) {
            struct CallbackUser {
                tsl::elm::List* list;
                std::string last_section;
            } callback_userdata{list};

            ini_browse([](const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, void *UserData){
                auto user = (CallbackUser*)UserData;
                std::string_view value{Value};

                if (value == "Skipped") {
                    return 1;
                }

                if (user->last_section != Section) {
                    user->last_section = Section;
                    user->list->addItem(new tsl::elm::CategoryHeader("Log: " + user->last_section));
                }

                #define F(x) ((x) >> 4)
                constexpr tsl::Color colour_FSPATCH{F(0), F(255), F(200), F(255)};
                constexpr tsl::Color colour_file{F(255), F(177), F(66), F(255)};
                constexpr tsl::Color colour_unpatched{F(250), F(90), F(58), F(255)};
                #undef F

                if (value.starts_with("Patched")) {
                    if (value.ends_with("(fs-patch)")) {
                        user->list->addItem(new tsl::elm::ListItem(Key, "Patched", colour_FSPATCH));
                    } else {
                        user->list->addItem(new tsl::elm::ListItem(Key, "Patched", colour_file));
                    }
                } else if (value.starts_with("Unpatched") || value.starts_with("Disabled")) {
                    user->list->addItem(new tsl::elm::ListItem(Key, Value, colour_unpatched));
                } else if (user->last_section == "stats") {
                    user->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorDescription));
                } else {
                    user->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorText));
                }

                return 1;
            }, &callback_userdata, LOG_PATH);
        } else {
            list->addItem(new tsl::elm::ListItem("No log found!"));
        }

        frame->setContent(list);
        return frame;
    }
};

class GuiMain final : public tsl::Gui {
public:
    GuiMain() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("fs-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        auto options = new tsl::elm::ListItem("Options");
        auto toggle = new tsl::elm::ListItem("Toggle patches");
        auto log = new tsl::elm::ListItem("Log");

        options->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiOptions>();
                return true;
            }
            return false;
        });

        toggle->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiToggle>();
                return true;
            }
            return false;
        });

        log->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiLog>();
                return true;
            }
            return false;
        });

        list->addItem(new tsl::elm::CategoryHeader("Menu"));
        list->addItem(options);
        list->addItem(toggle);
        list->addItem(log);

        frame->setContent(list);
        return frame;
    }
};

class FSPATCHOverlay final : public tsl::Overlay {
public:
    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiMain>();
    }
};

} // namespace

int main(int argc, char **argv) {
    create_dir("/config/");
    create_dir("/config/fs-patch/");
    return tsl::loop<FSPATCHOverlay>(argc, argv);
}