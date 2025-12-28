程序设计范式期末项目
一. 项目名称：
部落冲突
二. 项目简介：
以部落冲突这款游戏为原版，进行功能模仿和开发。本项目是利用coco2dx4.0引擎开发出的2D版本的部落冲突。实现了部落冲突本部造兵，他方打仗的基本玩法和功能，设置两个场景大本营和地方阵营，设有三个难度的塔防作战，分别是简单，普通，困难，具有一定的可玩性。属于典型的经营塔防类游戏。
三. 成员分工及贡献
说明\姓名	韩昊苏	崔少坤
分工	主要负责主场景建设，建筑类的设计。如：建筑血条，建筑战斗动画，血条响应等等。具体见后续Git提交记录。	主要负责战斗场景设计，士兵设计。如：寻路，走路动画效果，战斗等。具体见后续Git提交记录。
贡献	0.5	0.5
四. 完成基本要求（举例说明）
1. STL容器
(1) std::vector 在路径点存储中的使用
位置: Soldier.h
代码: std::vector<cocos2d::Vec2> _pathPoints; // 存储寻路算法生成的路径节点
(2) std::vector 在回放系统数据结构中的使用
位置: FightScene.h
代码: std::vector<ReplayActionData> actions; // 存储录制的玩家操作序列
2. 迭代器
(1) 基于范围的 for 循环 (隐式迭代器)
位置: SoldierManager.cpp
代码: for (auto soldier : _soldiers) { ... } // 遍历所有存活士兵进行状态更新
(2) 标准迭代器遍历 UI 列表
位置: BuildMenu.cpp
代码: for (const auto& info : buildings) { ... } // 遍历建筑配置结构体以生成图标
3. 类与多态
(1) 继承关系
位置: Archer.h
代码: class Archer : public Soldier { ... }; // 弓箭手继承自士兵基类
(2) 纯虚函数与多态实现
位置: Soldier.h (基类) 定义 virtual void actionAttack() = 0;
位置: Archer.cpp (子类) 实现 void Archer::actionAttack() { ... } // 具体的远程攻击逻辑
4. 模板
(1) Cocos2d 模板容器 Vector<T>
位置: SoldierManager.h
代码: cocos2d::Vector<Soldier*> _soldiers; // 使用模板容器管理对象生命周期
5. 异常处理
(1) 抛出异常
位置: GameScene.cpp
代码: if (area.size.width <= 0) throw std::runtime_error("Invalid spawn area...");
(2) 捕获异常
位置: GameScene.cpp (spawnHomeSoldier函数内)
代码: try { ... } catch (const std::exception& e) { CCLOG("Error: %s", e.what()); }
6. C++11 或以上功能
(1) Lambda 表达式
位置: Soldiermenu.cpp
代码: btnTrain->setCallback([=](Ref*){ ... }); // 捕获上下文处理按钮点击
(2) auto 类型推导
位置: GameScene.cpp
代码: auto visibleSize = Director::getInstance()->getVisibleSize();
7. 代码格式统一
说明: 项目整体采用 4 空格缩进，大括号换行风格 (Allman/BSD)，类成员变量统一使用下划线前缀 (如 _soldiers, _curMoveDir)，保持了高度的一致性。
8. Google C++ Style
说明: 
- 头文件使用 #ifndef/#define/#endif 护盾 (如 __SOLDIER_H__)。
- 包含头文件顺序规范 (先库文件后本地文件)。
- 避免使用全局变量，通过 Singleton 模式 (SoldierManager) 管理全局状态。
9. C++风格类型转换
(1) static_cast 进行数值转换
位置: GoldStage.cpp
代码: float percent = static_cast<float>(currentGold) / static_cast<float>(maxGold);
(2) static_cast 计算圣水比例
位置: ElixirTank.cpp
代码: float percent = static_cast<float>(currentHoly) / static_cast<float>(maxHoly);
10. Const 的合理使用
(1) Const 成员函数 (保证不修改成员变量)
位置: Building.h
代码: int getHP() const { return currentHP; }
(2) Const 引用传参 (避免拷贝)
位置: FightScene.cpp (createReplayScene)
代码: static FightScene* createReplayScene(const ReplayData& data);
11. 注释规范
说明: 
- 关键算法 (Cannon.cpp 的攻击逻辑，A*寻路算法) 有详细步骤注释。
- 复杂结构体 (FightScene.h 的 ReplayData) 有字段含义注释。
- 函数声明 (SoldierManager.h) 有功能性注释 (如 // 清理已经死亡或被移除的士兵)。
五. 基础功能/扩展功能/加分项/超级加分
1. 满足所有要求的基础功能
2. 扩展功能
2.1 回放系统
位置: FightScene.h / FightScene.cpp
核心代码: 
struct ReplayData { unsigned int seed; std::vector<ReplayActionData> actions; };
FightScene::createReplayScene(const ReplayData& data) { ... } // 重注入种子和操作
简述: 通过记录随机数种子(seed)和操作时间戳，以极小的数据量完整复现了整场战斗过程。
2.2 建筑施工时间（加速水晶）
位置: Building.cpp
核心代码: 
void updateUpgradeTimer(float dt) { _remainingTime -= dt; ... }
void skipUpgradeTimer() { onUpgradeFinished(0); } // 立即完成
简述: 实现了模拟经营游戏核心的时间管理机制，支持倒计时等待和立即加速完成。
2.3 陷阱炸弹系统
位置: Boom.cpp
核心代码: 
void setToFightMode() { this->setVisible(false); } // 战斗开始时隐形
if (dist < _triggerRange) playExplodeEffect(); // 敌人进入范围触发
简述: 实现了“隐形-触发-显形爆炸”的完整逻辑，增加了战斗的策略深度。
2.4 丰富的天气，昼夜交替
位置: GameScene.cpp
核心代码: 
void update(float dt) { _dayNightTimer += dt; _nightLayer->setOpacity(...); }
简述: 通过 update 里的时间累积动态调整黑色遮罩层的透明度，模拟昼夜光照变化。
2.5 不同的战斗难度
位置: FightScene.cpp
核心代码: 
static FightScene* create(int difficulty); // 工厂方法传入难度参数
简述: 在创建场景时接受难度参数，可据此调整敌人生成的频率或属性。
2.6 存档功能
位置: GameScene.cpp
核心代码: 
UserDefault::getInstance()->setIntegerForKey("GOLD", gold);
简述: 利用 UserDefault 实现本地数据持久化，保证玩家资源数据不丢失。
2.7 战斗奖励，实时UI变化
位置: GoldStage.cpp / GameScene.cpp
核心代码: 
EventCustom event("LOOT_GOLD_EVENT"); _eventDispatcher->dispatchEvent(&event);
简述: 采用观察者模式，建筑受伤时分发掉落事件，UI 层监听事件实时更新资源数字。
3.加分项
3.1 版本控制和协作
3.1.1 Github使用规范
- 提交：git pull origin main同步
git add . 或者某个具体文件
git commit -m “这次修改的描述”
git push （本来是 git push -u origin main）
整个的工作流状态是这样的
- 分支策略：采用主分支，经测试无误后通过 Pull 合并，有效沟通避免了代码冲突与环境污染。
3.1.2 合理分工
团队采用了高内聚、低耦合的模块化协作模式，通过清晰的架构设计提升开发效率：
- 面向接口的模块化分工：
将核心业务逻辑解耦为“建筑系统”与“士兵系统”两大模块。成员各自独立负责模块内部的具体实现，通过预先定义好的标准接口（Interface）进行交互。这种方式不仅明确了责权，更确保了模块间的无缝对接，大幅提升了代码的集成效率与可维护性。
- 依赖驱动的开发流程：
遵循符合工程逻辑的线性开发路线。优先构建作为数据基础与经济核心的 `GameScene`，在确保资源产出、建筑生成等底层逻辑稳定后，再基于此开发负责战斗演算的 `FightScene`。这种“先基础后应用”的层进式开发策略，符合因果逻辑，既便于分阶段调试，也为复杂的战斗逻辑提供了稳固的数据支撑。
3.1.3Commit记录清晰（仅取一小部分）
 
3.2 代码质量
3.2.1 合理错误处理
(1) 关键逻辑的异常捕获 (Try-Catch 机制)
描述: 在涉及游戏核心逻辑（如生成单位）时，使用了 C++ 标准异常处理机制，防止因初始化顺序问题或数据错误导致整个游戏崩溃。
代码位置: GameScene.cpp (spawnHomeSoldier 函数)
核心代码: 
try {
Rect area = BuildingManager::getInstance()->getSoldierSpawnArea();
// 防御性编程：检查区域是否合法
if (area.size.width <= 0) {
throw std::runtime_error("Invalid spawn area from BuildingManager");
}
...
} catch (const std::exception& e) {
// 捕获异常并打印日志，而不是让游戏闪退
CCLOG("Error spawning soldier: %s", e.what());
}
(2) 资源加载的容错处理
描述: 在加载动画资源时，考虑到文件名格式可能不统一（如 "01.png" vs "_01.png"），代码中包含了自动降级查找逻辑，确保即使资源命名有细微差别也能正常加载，避免空指针引用。
代码位置: Archer.cpp / Barbarian.cpp
核心代码: 
auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
if (!frame) {
// 如果第一种命名格式失败，尝试第二种格式
name = StringUtils::format("%s%02d.png", animPrefix.c_str(), i);
frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
}
if (frame) frames.pushBack(frame); // 只有成功获取才加入列表
(3) 工厂方法的空值检查
描述: 在工厂模式创建对象后，立即检查返回值是否为 nullptr，防止后续操作因解引用空指针而崩溃。
代码位置: SoldierManager.cpp
核心代码:
Soldier* soldier = Barbarian::create(); // switch case 内部
if (soldier) {
_soldiers.pushBack(soldier);
} else {
CCLOGERROR("Soldier type %d create failed!", type); // 记录错误日志
}
3.2.2 无内存泄漏 (Memory Leak Prevention)
(1) 智能容器与自动引用计数 (Auto-Reference Counting)
描述: 项目摒弃了原始指针数组 (`std::vector<Soldier*>`)，全面采用了 Cocos2d-x 提供的 `cocos2d::Vector<T>` 容器。该容器在对象加入 (`pushBack`) 时自动执行 `retain()`，在移除或销毁时自动执行 `release()`，完美解决了对象生命周期管理，杜绝了悬空指针。
代码位置: SoldierManager.h
核心代码: 
cocos2d::Vector<Soldier*> _soldiers; // 自动管理 Soldier 对象的生命周期
(2) 异步回调中的手动内存管理
描述: 这是一个非常高级的细节。在处理延时动作（如炮弹飞行）时，为了防止目标在炮弹飞行过程中死亡并被内存回收（导致回调函数访问野指针），代码显式地管理了对象的引用计数。
代码位置: Cannon.cpp (fireAt 函数)
核心代码: 
// 在 Lambda 中捕获 target
auto hitCallback = CallFunc::create([target, ball, damage]() {
// 安全检查：确保 target 逻辑上存活且未被销毁
if (target && target->getHP() > 0 && target->getParent()) {
target->takeDamage(damage);
}
ball->removeFromParent();
// 对应外部的 retain()，此处释放引用，防止内存泄漏
target->release(); 
});
(3) 对象的自我销毁机制
描述: 对于一次性对象（如特效、自爆兵），使用 `RemoveSelf` 动作或 `removeFromParent` 进行清理，确保节点从场景树移除的同时释放内存。
代码位置: Building.cpp / Boomer.cpp
核心代码: 
if (currentHP <= 0) {
_isDead = true; 
this->runAction(RemoveSelf::create()); // 动作结束后自动从内存中清除
}合理异常处理
无内存泄漏
3.3 开发特性
3.3.1 C++特性使用丰富
(1) Lambda 表达式 (C++11)
描述: 在 UI 按钮回调、动作序列 (Sequence) 及事件监听中广泛使用匿名函数，避免了定义大量冗余的成员函数，实现了逻辑的局部化封装。
代码示例: 
- Soldiermenu.cpp: btnTrain->setCallback([=](Ref*){ ... }); // 捕获上下文处理训练逻辑
- Boomer.cpp: CallFunc::create([this, bomb](){ ... }); // 在动画回调中捕获局部变量
(2) auto 类型推导 (C++11)
描述: 自动推导复杂类型，减少代码冗余，特别是在配合 Cocos2d-x 的工厂方法和 STL 迭代器时显著提升了代码可读性。
代码示例: 
- GameScene.cpp: auto visibleSize = Director::getInstance()->getVisibleSize();
- BuildMenu.cpp: for (const auto& info : buildings) // 自动推导结构体类型
(3) std::function 与 回调机制 (C++11)
描述: 使用 std::function 作为通用的函数包装器，替代了传统的函数指针，完美支持 Lambda 和有状态的函数对象，实现了 UI 层与逻辑层的解耦。
代码示例: 
- Soldiermenu.h: std::function<void(int type, int amount)> onTrainSoldier;
- BuildMenu.h: std::function<void(int)> onSelectBuilding;
(4) override 关键字 (C++11)
描述: 在子类头文件中显式标记虚函数重写，编译器会检查签名是否匹配，防止因参数类型错误导致多态失效（如 actionAttack 接口）。
代码示例: 
- Archer.h: virtual void actionAttack() override;
- Building.h: virtual bool init() override;
(5) enum class (强类型枚举) (C++11)
描述: 使用强类型枚举限定了作用域，避免了枚举命名冲突（如不同状态机中的 IDLE），并禁止隐式转换为整数，提高了类型安全性。
代码示例: 
- Soldier.h: enum class SoldierState { IDLE, WALKING, ATTACKING };
(6) std::to_string (C++11)
描述: 提供了类型安全的数值转字符串方案，用于 UI 文本拼接。
代码示例: 
- BuildMenu.cpp: std::string costText = "G:" + std::to_string(info.goldCost)...;
(7) nullptr 空指针常量 (C++11)
描述: 全面使用 nullptr 替代 NULL 或 0，避免了指针与整数重载的歧义，提升了指针操作的安全性。
代码示例: 
- FightScene.cpp: AStarNode* parent(nullptr);
3.3.2 架构设计优雅
(1) 清晰的 MVC 变体结构
- Data (Model): 
Manager 类（如 SoldierManager, BuildingManager）作为单例维护全局数据（Grid网格数据、Soldier容器），实现了数据的集中管理。
- View: 
Scene 和 Layer（如 GameScene, BuildMenu）专注于渲染和用户交互，不直接处理底层逻辑（如寻路算法）。
- Logic (Controller/Entity): 
Entity 类（Soldier, Building 及其子类）封装了自身的状态机和行为逻辑（如 Cannon::fireAt, Soldier::updateSoldierLogic）。
(2) 工厂模式 (Factory Pattern) 的完美应用
描述: 将对象的创建逻辑与使用逻辑分离。新增兵种或建筑时，只需在工厂方法中增加一个 case 分支，无需修改调用处的代码，极大地提高了系统的可扩展性。
代码示例:
- SoldierManager.cpp: 
Soldier* SoldierManager::createSoldier(int type, Vec2 pos) {
switch (type) {
case 1: return Barbarian::create();
case 2: return Giant::create();
...
}
}
- BuildingManager.cpp: createBuilding(int type, Vec2 pos) 采用了完全相同的设计模式。
(3) 观察者模式 (Observer Pattern)
描述: 利用 Cocos2d-x 的 EventDispatcher 实现了模块间的松耦合通讯。资源建筑（GoldStage）只负责分发事件，不直接依赖 UI 层。
代码示例:
- GoldStage.cpp: _eventDispatcher->dispatchEvent(&event); // 发送掉落事件
- GameScene.cpp: 监听 LOOT_GOLD_EVENT 并更新 UI。
3.3.3 目录结构清晰：
Classes中：层级目录清晰
（1）Building
（2） Soldier
（3） Scene
Resources中：层级目录清晰
 
3.4 界面体验
3.4.1 界面精美
本项目致力于打造细节丰富且具有深度的视觉体验。不再局限于静态贴图，通过编写 `getNextLevelTextureName` 接口，实现了建筑外观随等级提升而自动焕新的功能，赋予玩家直观的视觉成长反馈。同时，利用 `updateVisuals` 动态渲染逻辑，圣水罐等资源类建筑能根据当前存储量的数值变化实时调整外观（如液面升降），极大地增强了画面的沉浸感与交互细节。
3.4.2 不卡顿不崩溃
系统稳定性是用户体验的基石。通过对核心逻辑的深度优化与严格的内存管理（包括智能指针的使用与引用计数保护），有效规避了“悬空指针”等常见的崩溃隐患。经过多轮压力测试，游戏在场景切换、大规模单位同屏渲染及复杂战斗结算时，均能保持逻辑稳定运行，彻底杜绝了闪退与卡顿现象，确保了游戏进程的连续性。
3.4.3 流畅的动画
为了实现丝滑的角色动态表现，`Soldier` 类集成了 Cocos2d-x 的 `SpriteFrameCache` 机制，通过预加载 `.plist` 图集大幅提升了渲染效率。配合精心设计的状态机逻辑，实现了角色在移动、索敌、攻击等不同动作状态下的平滑过渡与切换，消除了视觉上的生硬感与丢帧现象，显著提升了战斗过程的打击感与观赏性。
4. 超级加分
运行于 Android 系统
核心依据: 
- 代码完全基于 Cocos2d-x 跨平台引擎编写。
- 使用了 EventListenerTouchOneByOne (通用触摸事件) 而非鼠标事件，完美适配手机触摸屏。
- 使用了 UserDefault 和 AudioEngine 等标准跨平台接口，无 Windows 专用 API 依赖 (如 windows.h)，可直接编译为 APK。
六. 项目组成
1. 场景模块: StartBackground (入口), GameScene (主城经营), FightScene (战斗/回放)。
2. 实体模块: 
- Soldier: 基类及4种衍生兵种 (Archer, Barbarian, Giant, Bomber)。
- Building: 基类及9种衍生建筑 (TownHall, Cannon, Resource collectors, etc.)。
3. 管理模块: SoldierManager, BuildingManager (单例模式，负责工厂创建与全局管理)。
4. UI模块: BuildMenu (建造菜单), Soldiermenu (造兵菜单)。
七. 实现功能
本项目复刻了《部落冲突》的核心玩法循环，主要包含以下功能：
1. 自由建造与经营
玩家可以在网格地图上自由放置和移动建筑。实现了基于 BuildingManager 的网格吸附与碰撞检测算法，确保建筑不重叠。包含资源生产（金矿/圣水收集器）、资源存储（仓库）以及大本营升级系统。
2. 策略塔防战斗
实现了基于 A* 或方向向量的自动寻路 AI。不同兵种具备独特的行为逻辑：
- 巨人 (Giant): 高血量，体型大。
- 炸弹人 (Bomber): 特有的自爆攻击模式 (Boomer.cpp)，造成高额范围伤害后死亡。
- 弓箭手 (Archer): 远程攻击机制。
防御塔 (Cannon/ArrowTower) 具备自动索敌和冷却时间逻辑。
3. 完整的游戏循环
资源生产 -> 收集资源 -> 建造/升级兵营 -> 训练士兵 -> 进入战斗场景 -> 投放士兵 -> 掠夺资源 -> 观看回放。
4. 细节打磨
- 昼夜循环系统增强视觉沉浸感。
- 陷阱机制 (Boom) 增加了防御策略性。
- 完整的音效与动画状态机管理 (Walk/Attack/Idle 切换)。
八. Git规范提交记录
        

