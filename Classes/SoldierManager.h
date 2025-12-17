#include "cocos2d.h"
#include <vector>
#include "Soldier.h"
#include "Barbarian.h"
#include "Giant.h"
class SoldierManager
{
public:
    static SoldierManager* getInstance();

    Soldier* createSoldier(int type, cocos2d::Vec2 pos);

    cocos2d::Vector<Soldier*>& getSoldiers() { return _soldiers; }
    // 清理已经死亡或被移除的士兵
    void cleanDeadSoldiers();

    // 重置
    void reset();
private:
    SoldierManager();

    // 32 × 24 的格子表//用于标记哪里有建筑物
    bool grid[30][16];

    // 存储所有存活的士兵
    cocos2d::Vector<Soldier*> _soldiers;
};