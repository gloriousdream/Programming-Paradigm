#ifndef __BOOM_H__
#define __BOOM_H__

#include "cocos2d.h"
#include "Building/Building.h"

class Boom : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(Boom);

    // 基类虚函数
    virtual void upgrade() override;
    virtual std::string getNextLevelTextureName() override;

    // 设置为战斗模式（隐形）
    void setToFightMode();

    // 显形并播放爆炸动画
    void playExplodeEffect();

    // 属性 Getters
    float getTriggerRange() const { return _triggerRange; }
    float getExplosionRadius() const { return _explosionRadius; }
    int getDamage() const { return _damage; }
    bool isTriggered = false;    // 是否已经被踩到了
    float delayTimer = 0.5f;     // 延迟时间 (秒)
private:
    float _triggerRange;     // 触发范围 (走多近会炸)
    float _explosionRadius;  // 伤害范围 (炸多远)
    int _damage;             // 伤害值
};

#endif