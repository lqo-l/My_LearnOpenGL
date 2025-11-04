#pragma once
#include <vector>
#include <string>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 使用 X-Macro 定义材质列表（添加新材质时，只在这里修改）
#define MATERIAL_LIST \
    /* 宝石类 */ \
    X(emerald,          "Emerald",          "绿宝石",      glm::vec3(0.0215f, 0.1745f, 0.0215f),      glm::vec3(0.07568f, 0.61424f, 0.07568f),    glm::vec3(0.633f, 0.727811f, 0.633f),      76.8f) \
    X(jade,             "Jade",             "翡翠",        glm::vec3(0.135f, 0.2225f, 0.1575f),       glm::vec3(0.54f, 0.89f, 0.63f),             glm::vec3(0.316228f, 0.316228f, 0.316228f), 12.8f) \
    X(obsidian,         "Obsidian",         "黑曜石",      glm::vec3(0.05375f, 0.05f, 0.06625f),      glm::vec3(0.18275f, 0.17f, 0.22525f),       glm::vec3(0.332741f, 0.328634f, 0.346435f), 38.4f) \
    X(pearl,            "Pearl",            "珍珠",        glm::vec3(0.25f, 0.20725f, 0.20725f),      glm::vec3(1.0f, 0.829f, 0.829f),            glm::vec3(0.296648f, 0.296648f, 0.296648f), 11.264f) \
    X(ruby,             "Ruby",             "红宝石",      glm::vec3(0.1745f, 0.01175f, 0.01175f),    glm::vec3(0.61424f, 0.04136f, 0.04136f),    glm::vec3(0.727811f, 0.626959f, 0.626959f), 76.8f) \
    X(turquoise,        "Turquoise",        "绿松石",      glm::vec3(0.1f, 0.18725f, 0.1745f),        glm::vec3(0.396f, 0.74151f, 0.69102f),      glm::vec3(0.297254f, 0.30829f, 0.306678f), 12.8f) \
    /* 金属类 */ \
    X(brass,            "Brass",            "黄铜",        glm::vec3(0.329412f, 0.223529f, 0.027451f), glm::vec3(0.780392f, 0.568627f, 0.113725f), glm::vec3(0.992157f, 0.941176f, 0.807843f), 27.89743f) \
    X(bronze,           "Bronze",           "青铜",        glm::vec3(0.2125f, 0.1275f, 0.054f),       glm::vec3(0.714f, 0.4284f, 0.18144f),       glm::vec3(0.393548f, 0.271906f, 0.166721f), 25.6f) \
    X(chrome,           "Chrome",           "铬",          glm::vec3(0.25f, 0.25f, 0.25f),            glm::vec3(0.4f, 0.4f, 0.4f),                glm::vec3(0.774597f, 0.774597f, 0.774597f), 76.8f) \
    X(copper,           "Copper",           "铜",          glm::vec3(0.19125f, 0.0735f, 0.0225f),     glm::vec3(0.7038f, 0.27048f, 0.0828f),      glm::vec3(0.256777f, 0.137622f, 0.086014f), 12.8f) \
    X(gold,             "Gold",             "金",          glm::vec3(0.24725f, 0.1995f, 0.0745f),     glm::vec3(0.75164f, 0.60648f, 0.22648f),    glm::vec3(0.628281f, 0.555802f, 0.366065f), 51.2f) \
    X(silver,           "Silver",           "银",          glm::vec3(0.19225f, 0.19225f, 0.19225f),   glm::vec3(0.50754f, 0.50754f, 0.50754f),    glm::vec3(0.508273f, 0.508273f, 0.508273f), 51.2f) \
    /* 塑料类 */ \
    X(black_plastic,    "Black Plastic",    "黑色塑料",    glm::vec3(0.0f, 0.0f, 0.0f),               glm::vec3(0.01f, 0.01f, 0.01f),             glm::vec3(0.50f, 0.50f, 0.50f),            32.0f) \
    X(cyan_plastic,     "Cyan Plastic",     "青色塑料",    glm::vec3(0.0f, 0.1f, 0.06f),              glm::vec3(0.0f, 0.50980392f, 0.50980392f),  glm::vec3(0.50196078f, 0.50196078f, 0.50196078f), 32.0f) \
    X(green_plastic,    "Green Plastic",    "绿色塑料",    glm::vec3(0.0f, 0.0f, 0.0f),               glm::vec3(0.1f, 0.35f, 0.1f),               glm::vec3(0.45f, 0.55f, 0.45f),            32.0f) \
    X(red_plastic,      "Red Plastic",      "红色塑料",    glm::vec3(0.0f, 0.0f, 0.0f),               glm::vec3(0.5f, 0.0f, 0.0f),                glm::vec3(0.7f, 0.6f, 0.6f),               32.0f) \
    X(white_plastic,    "White Plastic",    "白色塑料",    glm::vec3(0.0f, 0.0f, 0.0f),               glm::vec3(0.55f, 0.55f, 0.55f),             glm::vec3(0.70f, 0.70f, 0.70f),            32.0f) \
    X(yellow_plastic,   "Yellow Plastic",   "黄色塑料",    glm::vec3(0.0f, 0.0f, 0.0f),               glm::vec3(0.5f, 0.5f, 0.0f),                glm::vec3(0.60f, 0.60f, 0.50f),            32.0f) \
    /* 橡胶类 */ \
    X(black_rubber,     "Black Rubber",     "黑色橡胶",    glm::vec3(0.02f, 0.02f, 0.02f),            glm::vec3(0.01f, 0.01f, 0.01f),             glm::vec3(0.4f, 0.4f, 0.4f),               10.0f) \
    X(cyan_rubber,      "Cyan Rubber",      "青色橡胶",    glm::vec3(0.0f, 0.05f, 0.05f),             glm::vec3(0.4f, 0.5f, 0.5f),                glm::vec3(0.04f, 0.7f, 0.7f),              10.0f) \
    X(green_rubber,     "Green Rubber",     "绿色橡胶",    glm::vec3(0.0f, 0.05f, 0.0f),              glm::vec3(0.4f, 0.5f, 0.4f),                glm::vec3(0.04f, 0.7f, 0.04f),             10.0f) \
    X(red_rubber,       "Red Rubber",       "红色橡胶",    glm::vec3(0.05f, 0.0f, 0.0f),              glm::vec3(0.5f, 0.4f, 0.4f),                glm::vec3(0.7f, 0.04f, 0.04f),             10.0f) \
    X(white_rubber,     "White Rubber",     "白色橡胶",    glm::vec3(0.05f, 0.05f, 0.05f),            glm::vec3(0.5f, 0.5f, 0.5f),                glm::vec3(0.7f, 0.7f, 0.7f),               10.0f) \
    X(yellow_rubber,    "Yellow Rubber",    "黄色橡胶",    glm::vec3(0.05f, 0.05f, 0.0f),             glm::vec3(0.5f, 0.5f, 0.4f),                glm::vec3(0.7f, 0.7f, 0.04f),              10.0f) \
    /* 自定义材质 */ \
    X(custom,           "Custom",           "自定义材质1", glm::vec3(0.2f, 0.1f, 0.06f),             glm::vec3(0.5f, 0.25f, 0.15f),              glm::vec3(0.5f, 0.5f, 0.5f),              32.0f)



// 生成枚举类
enum class MatID{
#define X(enumName, ...) enumName,
    MATERIAL_LIST
#undef X
};


class Material {
public:
    Material(const std::string& name, const std::string& desc, 
             glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float shininess, std::optional<MatID> id = std::nullopt)
        : name(name), desc(desc), Ka(Ka), Kd(Kd), Ks(Ks), shininess(shininess), id(id) {}

    // 简化构造函数（只有光照参数）
    Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float shininess)
        : name("Custom"), desc("自定义材质"), Ka(Ka), Kd(Kd), Ks(Ks), shininess(shininess), id(std::nullopt) {}

    // ID预设构造
    Material(MatID id){
        const Material& preset = getPreset(id);
        *this = preset;
        this->id = id;
    }

    // 默认构造函数
    Material() : Material(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 32.0f) {}

    void setMaterial(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float shininess){
        this->Ka = Ka;
        this->Kd = Kd;
        this->Ks = Ks;
        this->shininess = shininess;
    }

    void setMaterial(MatID id){
        const Material& preset = getPreset(id);
        *this = preset;
        this->id = id;
    }

    void setMaterial(int id){
        if(id < 0) return;
        if(id >= MATERIAL_COUNT) return;
        setMaterial(static_cast<MatID>(id));
    }

    // 返回预设材质id，非预设材质返回custom的id
    int getID() const{
        if(id.has_value()) 
            return static_cast<int>(id.value());
        return static_cast<int>(MatID::custom);
    }

    // 判断是否是未修改的预设材质(无ID认为非预设)
    bool isModified() const {
        if(!id.has_value()) return false;
        return *this != getPreset(id.value());
    }

    void setName(const std::string& name) { this->name = name; }
    void setDesc(const std::string& desc) { this->desc = desc; }

    static std::string getMatIDName(MatID id){
        return getPreset(id).name;
    }

    static int getPresetCount(){
        return MATERIAL_COUNT;
    }

    //用于ImGui的便利方法
    static const std::vector<const char*>& getPresetNames() {
        static std::vector<const char*> names;
        if (names.empty()) {
            names.reserve(getPresetCount());
            for (const auto& material : g_materialPresets) {
                names.emplace_back(material.name.c_str());
            }
        }
        return names;
    }

    bool operator==(const Material& other) const{
        return Ka == other.Ka && Kd == other.Kd && Ks == other.Ks && shininess == other.shininess;
    }

    bool operator!=(const Material& other) const{
        return !(*this == other);
    }

private:
    // 静态方法根据id获取预设材质
    static const Material& getPreset(MatID id){
        if(static_cast<int>(id)>= MATERIAL_COUNT){
            throw std::out_of_range("Material::getPreset: MatID out of range");
        }
        return g_materialPresets[static_cast<int>(id)];
    }

    static const std::vector<Material>& getAllPresets(){
        return g_materialPresets;
    }


public:
    std::string name; // 材质名字
    std::string desc; // 材质描述
    glm::vec3 Ka; // ambient反射系数     // 一般认为是物体颜色
    glm::vec3 Kd; // diffusion反射系数   // 一般认为是物体颜色，等于Ka
    glm::vec3 Ks; // specular反射系数
    float shininess; // 高光指数
    
private:
    std::optional<MatID> id; // 预设材质ID（无则为自定义材质）

    static const std::vector<Material> g_materialPresets; // 材质预设列表（运行期常量,声明不定义不能使用inline，否则重定义）
    static constexpr int MATERIAL_COUNT = // 统计预设材质数量（编译期常量，隐含inline）
#define X(...) + 1
    0 MATERIAL_LIST;
#undef X

};

inline const std::vector<Material> Material::g_materialPresets{
#define X(enumName, name, desc, Ka, Kd, Ks, shininess) Material{name, desc, Ka, Kd, Ks, shininess, MatID::enumName},
    MATERIAL_LIST
#undef X
};